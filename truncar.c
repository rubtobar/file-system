#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bloques.h"
#include "ficheros_basico.h"

int main(int argc, char const *argv[]){

	if (argv[1] == '\0' || argv[2] == '\0' || argv[3] == '\0'){
		printf("ERROR: introduce [Nombre disco][numero inodo][nblogico]\n");
		return(-1);
	}

	bmount(argv[1]);					//montamos archivo
	unsigned int ninodo = atoi(argv[2]); 		//guardamos el numero de bloques entrados por argumento
	unsigned int nblogico = atoi(argv[3]);
	
	liberar_bloques_inodo(ninodo, nblogico);

	bumount(); //desmontamos archivo

	return 0;
}