
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>


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

unsigned short csum(unsigned short *buf, int nwords);
void dump(const unsigned char *data_buffer, const unsigned int length) ;

int main(int argc, char *argv[]) {
	
	int socketfd;
	int status;
	int option = 1;
	int localerror;
	
	unsigned char *buffer;
	struct ip_hd *ip; 
	struct icmp_hd *icmp;
	
	unsigned int length;
	
	//A donde se enviara el paquete 
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	//addr.sin_port = htons(1);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");	

   //Creamos el Paquete a Enviar
	buffer = (unsigned char *) malloc(4096);
	memset(buffer,0,4096);
	
	length = sizeof(struct ip_hd) + sizeof(struct icmp_hd) + 64;
	
	ip = (struct ip_hd *) buffer;
	ip->version = 4;
	ip->ihl = 5;
	ip->tos = 0;
	ip->length = htons(length);
	ip->id = htons(1);
	ip->frag_offset = 0;
	ip-> ttl = 10;
	ip->protocol = IPPROTO_ICMP;
	ip->check = 0;
	ip->saddr.s_addr = inet_addr("127.0.0.1");
	ip->daddr.s_addr = inet_addr("127.0.0.1");
	 
	icmp = (struct icmp_hd *) (buffer + sizeof(struct ip_hd));
	
	icmp->type = 8;
	icmp->code = 0;
	icmp->checksum = 0;
	icmp->rest = 0;
	
	memset(buffer+sizeof(struct ip_hd)+sizeof(struct icmp_hd),'A',64);
	
	//ip->check = csum((unsigned short *)buffer,ip->ihl);
	//icmp->checksum = csum((unsigned short *)icmp,sizeof(struct icmp_hd)+64);
	
	printf("Paquete a enviar\n");
	dump(buffer,length);
	printf("\n");
	
	socketfd = socket(AF_INET,SOCK_RAW,IPPROTO_RAW);
	if(socketfd < 0) {
		localerror = errno;
		printf("Can't create Socket (%s)\n",strerror(localerror));
		return -1;
	}
	
	status = setsockopt(socketfd, IPPROTO_IP, IP_HDRINCL,&option,sizeof(option));
	if(status < 0) {
		printf("Can't set option of set own IP Header\n");
		return -1;
	}
	
	printf("Se enviaran %i bytes\n",length);
	status = sendto(socketfd,buffer,length,0,(struct sockaddr *) &addr,sizeof(addr));
	if(status < 0) {
		localerror = errno;
		printf("Error Sending Msg (%s)\n",strerror(localerror));
		return -1;		
	}
	printf("Se enviaron %i bytes\n",status);
	
}

unsigned short csum(unsigned short *buf, int nwords) {
	unsigned long sum;
	for(sum = 0; nwords > 0; nwords --) {
		sum += *buf++;
	}
	sum = ( sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	return sum;
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