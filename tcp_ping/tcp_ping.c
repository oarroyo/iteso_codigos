/**
  @file tcp_ping.c
  @brief Codigo que genera un ping sobre TCP

  @author Alvaro Parres
  @date Feb/2013

*/

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <inttypes.h>
#include <limits.h>

#include "defaults.h"
#include "tcp_ping.h"
#include "utils/debug.h"
#include "server.h"
#include "client.h"

//Variables Globales
u_short port = CONFIG_DEFAULT_PORT;
int debugLevel = CONFIG_DEFAULT_VERBOSE;
u_int msg_count = CONFIG_DEFAULT_COUNT;
u_int msg_size = CONFIG_DEFAULT_MSGSIZE;
char ip_dst[18] = {0};

enum mode_enum {
	NOT_DEFINE = 0,
	SERVER = 1,
	CLIENT = 2
};

mode_enum mode = NOT_DEFINE;

//Prototipos Locales
int processArguments(int argc, char* argv[]);
void printHelp(const char *cmd, const short error, const char *msg=NULL);
//Fin de Prototipos Locales

int start(int argc, char *argv[]) {
	
	printf(".:: TCP PING ::.\n");
	if(!processArguments(argc, argv)) return -1;
	
	if(mode == SERVER) {
		start_server(CONFIG_LISENT_IFACE, port, CONFIG_MAX_CLIENT);
	} else if(mode == CLIENT) {
		start_client(ip_dst,port,msg_count,msg_size);
	}
	
	return 0;
}

int processArguments(int argc, char* argv[]) {

	int i;
	u_int temp;
	
	if(argc < 2) {
		printHelp(argv[0],true,"Faltan Argumentos\n");
		return false;
	}	

    if(strcmp(argv[1],"-s")==0){
		mode = SERVER;
	} else if(strcmp(argv[1],"-c")==0) {
		mode = CLIENT;
	} else {
		printHelp(argv[0],true,"Unkown Mode\n");
		return false;
	}
	
	for(i=2; i<argc; i++) {
		if(strcmp(argv[i],"-h")==0) {
			printHelp(argv[0],false);
			return true;
		}	
		
		if(strcmp(argv[i],"-p")==0) {
			temp = strtoimax(argv[++i], NULL, 10);
			if(temp == 0 || temp < MINPORT || temp > MAXPORT) {
				printHelp(argv[0],true,"Port out of range\n");
				return false;
			}
			port = temp;
		} else if(strcmp(argv[i],"-n")==0) {
			temp = strtoimax(argv[++i], NULL, 10);
			if(temp <= 0 ) {
				printHelp(argv[0],true,"Number of MSG Invalid\n");
				return false;
			}
			msg_count = temp;
		} else if(strcmp(argv[i],"-s")==0) {
			temp = strtoimax(argv[++i], NULL, 10);
			if(temp == 0 ) {
				printHelp(argv[0],true,"Invalid MSG Size\n");
				return false;
			}
			msg_size = temp;
		} else if(strcmp(argv[i],"-d")==0) {
			stpncpy(ip_dst,argv[++i],18);
		} else if(strstr(argv[i],"-v")!=NULL) {
			debugLevel = strlen(argv[i])-1;
		} else {
			printHelp(argv[0],true,"Unkown option\n");
			return false;
		}	
	}

	debug(2,"%s","\tConfiguraciones:");
	if(mode == SERVER) {
		debug(2,"\t\tModo:\t\t%s","Servidor");
		debug(2,"\t\tPuerto:\t\t%u",port);
	} else if (mode == CLIENT) {	
		debug(2,"\t\tModo:\t\t%s","Cliente");
		debug(2,"\t\tPuerto:\t\t%u",port);
		debug(2,"\t\tNum. Msg:\t%u",msg_count);
		debug(2,"\t\tTamaño Msg:\t%u Kb",msg_size);
		debug(2,"\t\tIP Dst:\t%s",ip_dst);
	}
	
	return true;
}

void printHelp(const char *cmd, const short error, const char *msg) {
	
	if(error) {
		fprintf(stderr,"Error: %s\n\n",msg);
	}
	
	printf("Use:\t%s -s [options]\n", cmd);
	printf("Use:\t%s -c [options]\n", cmd);
	printf("\n");
	printf("Modes:\n");
	printf("\t-s\t For Server Mode\n");
	printf("\t-c\t For Client Mode\n");
	printf("\n");
	printf("Global Options:\n");
	printf("\t-p\t Set port number to work on (Default %u)\n",CONFIG_DEFAULT_PORT);
	printf("\t-v[vvvv]\t Increments the verbosity of the server (Default %u)\n",CONFIG_DEFAULT_VERBOSE);
	printf("\t-h\t Prints this help message\n");
	printf("\n");
	printf("Client Options:\n");
	printf("\t-n\t Set the number of ping msg to send (Default %u)\n",CONFIG_DEFAULT_COUNT);
	printf("\t-s size\t Set the size on KBs of the msg (Default %u)\n",CONFIG_DEFAULT_MSGSIZE);
	printf("\t-d IP_ADDR\t Set the IP Dst\n");	
	printf("\n");
} 