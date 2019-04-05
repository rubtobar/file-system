#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bloques.h"
#include "ficheros_basico.h"

unsigned char *buffer[BLOCKSIZE];

int main(int argc, char const *argv[]){

	if (argv[1] == '\0' || argv[2] == '\0'){
		printf("ERROR: introduce [Nombre disco][Tamaño disco]\n");
		return(-1);
	}

	int nbloque = atoi(argv[2]); 		//guardamos el numero de bloques entrados por argumento
	memset(buffer, '\0', BLOCKSIZE);		//llenamos buffer de 0's
	bmount(argv[1]);					//montamos archivo

	for (int i = 0; i < nbloque; ++i){
		bwrite(i, buffer);				//llenamos archivo de 0's
	}

	//iniciamizamos el SuperBloque
	initSB(nbloque, nbloque/4);
	//inicializamos mapa de bits a 0
	initMB();
	//iniciamos array de inodos
	initAI();

	// reservamos inodo raiz
	reservar_inodo('d',7);

	//imprimimos los valores de nuestro disco
	leerSB(); //imprimimos SB

	bumount(); //desmontamos archivo

	return 0;
}
