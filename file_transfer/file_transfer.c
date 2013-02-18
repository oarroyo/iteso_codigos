/**

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
#include "file_transfer.h"
#include "utils/debug.h"
#include "server.h"
#include "client.h"

//Variables Globales
u_short port = CONFIG_DEFAULT_PORT;
int debugLevel = CONFIG_DEFAULT_VERBOSE;
char ip_dst[18] = {0};
char ip_listen[18] = CONFIG_LISENT_IFACE;
char filename[255];

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
		start_server(CONFIG_LISENT_IFACE, port, CONFIG_MAX_CLIENT, filename);
	} else if(mode == CLIENT) {
		start_client(ip_dst,port,filename);
	}
	
	return 0;
}

int processArguments(int argc, char* argv[]) {

	int i;
	u_int temp;
	
	if(argc < 3) {
		printHelp(argv[0],true,"Faltan Argumentos\n");
		return false;
	}	

    if(strcmp(argv[1],"-r")==0){
		mode = SERVER;
	} else if(strcmp(argv[1],"-t")==0) {
		mode = CLIENT;
	} else {
		printHelp(argv[0],true,"Unkown Mode\n");
		return false;
	}
	
	for(i=2; i<(argc-1); i++) {
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
		} else if(strcmp(argv[i],"-d")==0) {
			stpncpy(ip_dst,argv[++i],18);
		} else if(strstr(argv[i],"-v")!=NULL) {
			debugLevel = strlen(argv[i])-1;
		} else if(strcmp(argv[i],"-i")==0) {
			stpncpy(ip_listen,argv[++i],18);	
		} else {
			printHelp(argv[0],true,"Unkown option\n");
			return false;
		}	
	}
	
	stpncpy(filename,argv[i],254);

	debug(2,"%s","\tConfiguraciones:");
	if(mode == SERVER) {
		debug(2,"\t\tModo:\t%s","Servidor");
		debug(2,"\t\tListen on:\t%s:%u",ip_listen,port);
	} else if (mode == CLIENT) {	
		debug(2,"\t\tModo:\t%s","Cliente");
		debug(2,"\t\tDestino:\t%s:%u",ip_dst,port);
	}
	debug(2,"\t\tFileName:\t%s",filename);
	
	return true;
}

void printHelp(const char *cmd, const short error, const char *msg) {
	
	if(error) {
		fprintf(stderr,"Error: %s\n\n",msg);
	}
	
	printf("Use:\t%s -r [options] FILENAME for server ( receiver )\n", cmd);
	printf("Use:\t%s -t [options] FILENAME for client ( transmiter ) \n", cmd);
	printf("\n");
	printf("Global Options:\n");
	printf("\t-p\t Set port number to work on (Default %u)\n",CONFIG_DEFAULT_PORT);
	printf("\t-v[vvvv]\t Increments the verbosity of the server (Default %u)\n",CONFIG_DEFAULT_VERBOSE);
	printf("\t-h\t Prints this help message\n");
	printf("\t-i IP_ADDRe\t Interface to listen on. (Default %s)\n",CONFIG_LISENT_IFACE);
	printf("\t-d IP_ADDR\t Set the IP Dst\n");	
	printf("\n");
} 