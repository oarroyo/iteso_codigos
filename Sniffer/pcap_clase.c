#include <pcap.h>

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <arpa/inet.h>

#include <machine/endian.h>

void pcap_fatal(const char *failed_in, const char *errbuf);
void dump(const unsigned char *data_buffer, const unsigned int length);
void got_packet(u_char *args, const struct pcap_pkthdr *hdr, const u_char *packet);
int decode_ethernet(const unsigned char *header_start);
int decode_ip(const unsigned char *header_start);
int decode_tcp(const unsigned char *header_start);

struct eth_hd {
	unsigned char dst[6];
	unsigned char src[6];
	unsigned char type[2];
};

struct ip_hd {
#if __BYTE_ORDER == __LITTLE_ENDIAN 
	unsigned int ihl:4; 
	unsigned int version:4;
#elif __BYTE_ORDER == __BIG_ENDIAN 
	unsigned int version:4; 
	unsigned int ihl:4;
#else 
# error "Please Define Endian" 
#endif	

	u_int8_t tos;
	u_int16_t length;
	u_int16_t id;
	u_int16_t frag_offset;
	u_int8_t ttl;
	u_int8_t protocol;
	u_int16_t check;
	in_addr saddr;
	in_addr daddr;	
};

struct tcp_hd {
	u_int16_t sport;
	u_int16_t dport;
	u_int32_t seqn;
	u_int32_t ack;
	
	u_int rsv:3;
	u_int ns:1;
	u_int offset:4;

	
	u_int8_t flags;
	
	u_int16_t windows;
	u_int16_t checksum;
	u_int16_t urgent;	
};

int main(int argc, char *args[]) {
	struct pcap_pkthdr header;
	const unsigned char *packet;
	char errbuf[PCAP_ERRBUF_SIZE];
	char *device;
	pcap_t *pcap_handle;
	int i;
	
	//Para los filtros
	char *filter;
	struct bpf_program fp;	

	//PASO 1: Definir Device
	if(argc > 1) {
		device = args[1];
	} else {
		device = pcap_lookupdev(errbuf); // Nos regresa el device por default
		if(device == NULL) {
			pcap_fatal("pcap_lookupdev", errbuf);
		}
	}
	printf("Sniffing on device %s\n", device);
	
	pcap_handle = pcap_open_live(device,4096,1,1000,errbuf);
	if(pcap_handle == NULL) {
		pcap_fatal("pcap_open_live",errbuf);
	}

	if(argc > 2) {
		filter = args[2];
		if((pcap_compile(pcap_handle, &fp, filter, 1, 0))==-1) {
			pcap_fatal("pcap_compile",pcap_geterr(pcap_handle));	
		} else {
			if((pcap_setfilter(pcap_handle,&fp))== -1 ) {
				pcap_fatal("pcap_setfilter",pcap_geterr(pcap_handle));
			} else {
				printf("Setup filter: \"%s\"\n",filter);	
			}
		}
		
	}
		
	pcap_loop(pcap_handle,-1,got_packet,NULL);
	
}

void got_packet(u_char *args, const struct pcap_pkthdr *hdr, const u_char *packet) {
	int ip_hdr_length;
	int pos = 0;
	
	printf("==== Got a %d byte packet ====\n", hdr->len);
	pos += decode_ethernet(packet);
	pos += decode_ip(packet+pos);
	pos += decode_tcp(packet+pos);
	printf("[PAYLOAD]\n");
	dump(packet+pos, hdr->len-pos);
	//dump(packet, hdr->len);

}	

int decode_ip(const unsigned char *header_start) {
	struct ip_hd *ip;
	
	ip = (struct ip_hd *) header_start;
	
	printf("\t[[Layer 3 :: IP Header]]\n");
	printf("\tVER: %i",ip->version);
	printf("\tSRC: %s",inet_ntoa(ip->saddr)); 
	printf("\tDST: %s\n",inet_ntoa(ip->daddr));
	printf("\tID: %u\tTTL: %u\tPROTO: 0x%02x\n",ntohs(ip->id),ip->ttl,ip->protocol);
	
	return ip->ihl*4;
}

int decode_ethernet(const unsigned char *header_start) {
	struct eth_hd *hd;
	int i;
	
	hd = (struct eth_hd *) header_start;
	
	printf("[[Layer 2 ::: Ethernet Header]]\n");
	
	printf("[SRC: %02X",hd->src[0]);
	for(i=1; i < 6; i++) {
		printf(":%02X",hd->src[i]);
	}
	
	printf("\tDST: %02X",hd->dst[0]);
	for(i=1; i < 6; i++) {
		printf(":%02X",hd->dst[i]);
	}

	printf("\tTYPE: %02x%02x]\n",hd->type[0],hd->type[1]);
	
	return 14;
}

int decode_tcp(const unsigned char *header_start) {
	struct tcp_hd *tcp;
	int i;
	
	tcp = (struct tcp_hd *) header_start;
	
	printf("\t\t[[Layer 4 :: TCP Header]]\n");
	printf("\t\tSource Port %u\tDst Port %u\n",ntohs(tcp->sport),ntohs(tcp->dport));
	printf("\t\tSeq Num: %u\t Ack Num: %u\n",ntohs(tcp->seqn),ntohs(tcp->ack));
	printf("\t\tOffset: %u\n",tcp->offset);

	return tcp->offset*4;	
}

void pcap_fatal(const char *failed_in, const char *errbuf) {
	printf("Fatal Error in %s: %s\n",failed_in, errbuf);
	exit(1);
}	

void dump(const unsigned char *data_buffer, const unsigned int length) {
		unsigned char byte;
		unsigned int i, j;

		for(i=0; i < length; i++) {
			byte = data_buffer[i];
			printf("%02x ",data_buffer[i]); // Display Hex Number
			if( ((i%16) == 15) || (i == length-1) ) {
				for(j=0; j<15-(i%16); j++) {
					printf("   ");
				}
				printf("| ");
				for(j=(i-(i%16)); j <= i; j++) {
					byte = data_buffer[j];
					if((byte > 31) && (byte < 127)) {
						printf("%c",byte);
					} else {
						printf(".");
					}
				}
				printf("\n");
			}
		}
}		

	