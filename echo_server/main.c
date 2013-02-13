/*
 * Ejemplo básico de un servidor de Sockets.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <string.h>
#include <errno.h>

int main(int args, char *argv[]) {
	
	u_int port;
	int mySocket = 0;
	int clientSocket = 0;
	int status = 0;
	int localerror;
	
	struct sockaddr_in myAddr;
	struct sockaddr_in cliente = { 0 };
	socklen_t clienteLen;
	
	char mensaje[] = "\nHOLA!\n\n";
	char mensajeIN[255];
	
	if(args < 3) {
		fprintf(stderr,"Error: Missing Argument\n");
		fprintf(stderr,"\tUSE: %s [-s|-c] [PORT] [ADDR_DST]\n",argv[0]);
		return -1;
	}
	
	port = atoi(argv[2]);
	if(port < 1 || port > 65535) {
		fprintf(stderr,"Error: Port [%i] number out of range(1-65535)\n",port);
		return -1;
	}
	
	//Iniciamos la creación del Socket
	mySocket = socket(PF_INET,SOCK_STREAM,0);
	if(mySocket == -1) {
		localerror = errno;
		fprintf(stderr,"Error: Can't create socket (%s)\n",strerror(localerror));
	}
	
	if(strcmp(argv[1],"-s")==0) {
	//Nos Adjudicamos el puerto
		bzero(&myAddr,sizeof(myAddr));
		myAddr.sin_family = AF_INET;
		myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		myAddr.sin_port = htons(port);
		
		status = bind(mySocket, (struct sockaddr *)&myAddr, sizeof(myAddr));
		
		if(status != 0) {
			localerror = errno;
			fprintf(stderr,"Error: Can't bind port %i (%s)\n",port,strerror(localerror));
			close(mySocket);
			return -1;
		}

	//Ponemos el Socket en modo de Escucha
		status = listen(mySocket,5);
		if(status == -1) {
			localerror = errno;
			fprintf(stderr,"Can't listen on socket(%s)\n",strerror(localerror));
			close(mySocket);
			return -1;
		}

	   while(1) {
	//Aceptamos una Conexión (Función Bloqueante)

			clientSocket = accept(mySocket,(struct sockaddr *)&cliente,&clienteLen);
			
			if(clientSocket == -1) {
				localerror = errno;
				fprintf(stderr,"Can't accept connections(%s)\n",strerror(localerror));
				close(mySocket);
				return -1;
			}

			printf("Tenemos una Conexión\n");
			printf("Mandamos los Mensajes\n");
			status=write(clientSocket, mensaje, sizeof(mensaje));
			
			if(status != sizeof(mensaje)) {
				fprintf(stderr,"Error we only write %i bytes of %li bytes\n",status, sizeof(mensaje));
			}

			do {
				status=read(clientSocket, mensajeIN, sizeof(mensajeIN));
				printf("Leimos del Cliente: %s - reenviamos al cliente\n",mensajeIN);
				status=write(clientSocket, mensajeIN, strlen(mensajeIN));
			} while(strcmp(mensajeIN,"ADIOS")!=0);
				
			printf("Cerramos el Socket\n");
			close(clientSocket);
		}
	} else { // INICIA CLIENTE
		//Nos Conectamos al server
		bzero(&myAddr,sizeof(myAddr));
		
		myAddr.sin_family = AF_INET;		
		status = inet_pton(AF_INET,argv[3],&myAddr.sin_addr.s_addr);
		myAddr.sin_port = htons(port);
		
		status = connect(mySocket, (struct sockaddr *)&myAddr, sizeof(myAddr));
		
		if(status != 0) {
			localerror = errno;
			fprintf(stderr,"Error: Can't connect to port %i, %s\n",port,strerror(localerror));
			close(mySocket);
			return -1;
		}

		printf("Leemos el Mensaje\n");
		status=read(mySocket, mensajeIN, sizeof(mensajeIN));
		printf("Leimos %s\n",mensajeIN);
		status=write(mySocket,"ADIOS!!!\n",strlen("ADIOS!!!\n"));
		
	}
	
	close(mySocket);
	return 0;
}