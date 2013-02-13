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

#include "defaults.h"
#include "tcp_ping.h"

//Variables Globales
u_short port = CONFIG_DEFAULT_PORT;
int debugLevel = CONFIG_DEFAULT_VERBOSE;
u_int msg_count = CONFIG_DEFAULT_COUNT;
u_int msg_size = CONFIG_DEFAULT_MSGSIZE;

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
	
	return 0;
}

int processArguments(int argc, char* argv[]) {
	int i;

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
	}
	
	return true;
}

void printHelp(const char *cmd, const short error, const char *msg) {
	
	if(error) {
		fprintf(stderr,"Error: %s\n\n",msg);
	}
	
	printf("Use:\t%s -s [options]\n", cmd);
	printf("Use:\t%s -c [options] IP_DST\n", cmd);
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
	printf("\t-c\t Set the number of ping msg to send (Default %u)\n",CONFIG_DEFAULT_COUNT);
	printf("\t-s size\t Set the size on KBs of the msg (Default %u)\n",CONFIG_DEFAULT_MSGSIZE);
	printf("\n");
} 