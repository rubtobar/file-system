#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bloques.h"
#include "ficheros_basico.h"

unsigned char *buffer[BLOCKSIZE];

int main(int argc, char const *argv[]){

	if (argv[1] == '\0' || argv[2] == '\0'){
		printf("ERROR: introduce [Nombre disco][num inodo]\n");
		return(-1);
	}

	bmount(argv[1]);					//montamos archivo


	leerAI(atoi(argv[2]));

	bumount(); //desmontamos archivo

	return 0;
}