#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bloques.h"
#include "ficheros_basico.h"

unsigned char *buffer[BLOCKSIZE];

int main(int argc, char const *argv[]){

	if (argv[1] == '\0'){
		printf("ERROR: introduce [Nombre disco]\n");
		return(-1);
	}
	bmount(argv[1]);					//montamos archivo
	leerSB(); //imprimimos SB

	leerAI(0);

	leerMB();

	bumount(); //desmontamos archivo

	return 0;
}