/**
  @file main.c
  @brief Codigo Main del Software

  @author Alvaro Parres
  @date 01/2013

*/


/**
   @brief Función Main del software.
   
   Función Main del Software.
   
   @param argc Cantidad de Argumentos recividos.
   @param argv[] Arreglo de Strings con el valor de los argumentos.

   @returns Regresa siempre 1 al terminar de correr el programa.
*/

#include "tcp_ping.h"

int main(int argc, char *argv[]) {
	return start(argc, argv);
}