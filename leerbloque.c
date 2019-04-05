#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bloques.h"
#include "ficheros_basico.h"

unsigned char *buffer[BLOCKSIZE];

int main(int argc, char const *argv[]){

	if (argv[1] == '\0'|| argv[2] == '\0'|| argv[3] == '\0'){
		printf("ERROR: introduce [Nombre disco][num de bloques][p/d](p=punteros,d=datos)\n");
		return(-1);
	}

	bmount(argv[1]);					//montamos archivo

	int nbloque = atoi(argv[2]);

	if (*argv[3] == 'd'){
		leer_bloque(nbloque);
	}else if (*argv[3] == 'p'){
		imprimir_bloque(nbloque);
	}else if(*argv[3] == 'e'){
		leer_bloque_entradas(nbloque);
	}
	

	bumount(); //desmontamos archivo

	return 0;
}