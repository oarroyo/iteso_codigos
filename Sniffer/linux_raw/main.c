#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <netinet/if_ether.h>
#include <unistd.h>

#include <stdlib.h>
#include <arpa/inet.h>

#include <netinet/if_ether.h>  //For ETH_P_ALL


#define BUFFER_SIZE 65536

void processPacket(const unsigned char* buffer, int size);
int decode_ethernet(const unsigned char *header_start);
int decode_ip(const unsigned char *header_start, int *protocol=NULL);
int decode_tcp(const unsigned char *header_start);
int decode_icmp(const unsigned char *header_start);

void dump(const unsigned char *data_buffer, const unsigned int length);

struct eth_hd {
	unsigned char dst[6];
	unsigned char src[6];
	u_int16_t type;
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

struct icmp_hd {
	u_int8_t type;
	u_int8_t code;
	u_int16_t checksum;
	u_int32_t rest; 
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

int main(int argc, char *argv[]) {
	
	int socketfd,recv_length;
	unsigned char *buffer;
	struct sockaddr src;
	socklen_t saddr_size;
	
	socketfd = socket(AF_PACKET,SOCK_RAW, htons(ETH_P_ALL));
	setsockopt(socketfd , SOL_SOCKET , SO_BINDTODEVICE , argv[1] , strlen(argv[1])+ 1 );
	
	if(socketfd < 0) {
		printf("Can't creat socket!");
		return -1;
	}
	
	buffer = (unsigned char *) malloc(BUFFER_SIZE);
	
	printf("..:: Basic Network Sniffer ::..\n");
	
	while(true) {
		
		saddr_size = sizeof(src);
		recv_length = recvfrom(socketfd, buffer, BUFFER_SIZE, 0, &src, &saddr_size);
		if(recv_length < 0) {
			printf("Error Reading from Socket");
			return 1;
		}
		
		processPacket(buffer, recv_length);
	}
	
	close(socketfd);
	free(buffer);
	return 0;
}

void processPacket(const unsigned char* buffer, int size) {
	
	int pos=0;
	int protocol;
	
	printf("==== Got a %d bytes packet ====\n",size);
	pos += decode_ethernet(buffer);
	pos += decode_ip(buffer+pos,&protocol);
	switch(protocol) {
		case 1: //ICMP
			pos += decode_icmp(buffer+pos);
			break;
		case 6:  //TCP
			pos += decode_tcp(buffer+pos);
			break;
		case 17: //UDP
			//pos += decode_udp(buffer+pos);
			break;
		default:
			printf("==Layer 4 ::: Unknown Protocol (%u) ==\n",protocol);
			break;
	}
	
	dump(buffer+pos, size-pos);
	
	printf("\n\n");
	
	
}

int decode_ethernet(const unsigned char *header_start) {
	struct eth_hd *hd;
	int i;
	
	hd = (struct eth_hd *) header_start;
	
	printf("== Layer 2 ::: Ethernet Header ==\n");
	printf("\tSRC: %02X",hd->src[0]);
	for(i= 1; i < 6; i++) {
		printf(":%02X",hd->src[i]);
	}
	
	printf("\tDST: %02X",hd->dst[0]);
	for(i= 1; i < 6; i++) {
		printf(":%02X",hd->dst[i]);
	}
	
	printf("\tPROTO: %u\n",hd->type);
	
	return 14;
}

int decode_ip(const unsigned char *header_start, int *protocol) {
	struct ip_hd *ip;
	
	ip = (struct ip_hd *) header_start;
	
	printf("== Layer 3 :: IP Header ==\n");
	printf("\tVER: %i",ip->version);
	printf("\tSRC: %s",inet_ntoa(ip->saddr)); 
	printf("\tDST: %s\n",inet_ntoa(ip->daddr));
	printf("\tID: %u\tTTL: %u\tPROTO: %u\n",ntohs(ip->id),ip->ttl,ip->protocol);
	
	if(protocol != NULL) {
		*protocol = ip->protocol;
	}
	
	return ip->ihl*4;
}

int decode_icmp(const unsigned char *header_start) {

	struct icmp_hd* icmp;

	icmp = (icmp_hd *)header_start;
	printf("== Layer 4 :: ICMP Header ==\n");
	printf("\tType: %u ",icmp->type);
	switch(icmp->type) {
		case 0:
			printf("(Echo Reply) ");
			break;
		case 8:
			printf("(Echo Request) ");
			break;
	}
	printf("\tCode: %u\n",icmp->code);

	return 8;
	
}

int decode_tcp(const unsigned char *header_start) {
	struct tcp_hd *tcp;
	
	tcp = (struct tcp_hd *) header_start;
	
	printf("== Layer 4 :: TCP Header ==\n");
	printf("\tSource Port %u\tDst Port %u\n",ntohs(tcp->sport),ntohs(tcp->dport));
	printf("\tSeq Num: %u\t Ack Num: %u\n",ntohs(tcp->seqn),ntohs(tcp->ack));
	printf("\tOffset: %u\n",tcp->offset);

	return tcp->offset*4;	
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