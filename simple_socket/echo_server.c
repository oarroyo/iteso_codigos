/**
* @file simple_hello.c
* @brief Ejemplo básico de un echo Server
*
* @author Alvaro Parres
* @date Feb/2013
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
	int r_bytes = 0;
	int w_bytes = 0;
	int localerror;

	struct sockaddr_in myAddr;
	struct sockaddr_in cliente = { 0 };
	socklen_t clienteLen;

	char mensaje[255];

	if(args != 2) {
		fprintf(stderr,"Error: Missing Argument\n");
		fprintf(stderr,"\tUSE: %s [PORT]\n",argv[0]);
		return -1;
	}

	port = atoi(argv[1]);
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
		printf("Esperamos una conexión");
		
		clientSocket = accept(mySocket,(struct sockaddr *)&cliente,&clienteLen);

		if(clientSocket == -1) {
			localerror = errno;
			fprintf(stderr,"Can't accept connections(%s)\n",strerror(localerror));
			close(mySocket);
			return -1;
		}

		printf("Tenemos una Conexión\n");
		do {
			bzero(mensaje,sizeof(mensaje));
			r_bytes=read(clientSocket,mensaje,sizeof(mensaje));
			if(r_bytes > 0) {
				w_bytes=write(clientSocket,mensaje,r_bytes);
				if(r_bytes != w_bytes) {
					fprintf(stderr,"Error we only write %i bytes of %i bytes\n",w_bytes, r_bytes);
				}
			}
		} while(strcmp(mensaje,"QUIT\r\n")!=0);

		printf("Cerramos el Socket del cliente\n");
		close(clientSocket);
	}

	close(mySocket);
	return 0;
}