/**
  @brief Codigo Main del Servidor

  @author Alvaro Parres
  @date 03/2013

*/

#include <limits.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "defaults.h"
#include "utils/debug.h"
#include "server.h"

//Variables Globales
u_short port = CONFIG_DEFAULT_PORT;
int debugLevel = CONFIG_DEFAULT_VERBOSE;
char baseDir[255] = CONFIG_DEFAULT_BASEDIR;

//Prototipos de Funciones
int processArguments(int argc, char* argv[]);
void printHelp(const char *cmd, const short error, const char *msg="");

int main(int argc, char *argv[]) {	
	
	printf(".:: FILE TRANSFER SERVER ::.\n");
	if(!processArguments(argc, argv)) return -1;
	
	start_server(port,baseDir);
}

int processArguments(int argc, char* argv[]) {

	int i;
	u_int temp;
		
	for(i=1; i<argc; i++) {
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
			strncpy(baseDir,argv[++i],254);
		} else if(strstr(argv[i],"-v")!=NULL) {
			debugLevel = strlen(argv[i])-1;
		} else {
			printHelp(argv[0],true,"Unkown option\n");
			return false;
		}	
	}

	debug(2,"%s","Configuraciones:");
	debug(2,"Puerto:\t\t%u",port);
	debug(2,"Base Directory:\t%s",baseDir);
	
	return true;
}

void printHelp(const char *cmd, const short error, const char *msg) {
	
	if(error) {
		fprintf(stderr,"Error: %s\n\n",msg);
	}
	
	printf("Use:\t%s [options]\n", cmd);
	printf("\n");
	printf("Options:\n");
	printf("\t-p PORT_NUMBER\t Set port number to work on (Default %u)\n",CONFIG_DEFAULT_PORT);
	printf("\t-d DIRECTORY\t Set port number to work on (Default Same)\n");	
	printf("\t-v[vvvv]\t Increments the verbosity of the server (Default %u)\n",CONFIG_DEFAULT_VERBOSE);
	printf("\t-h\t Prints this help message\n");
	printf("\n");
}