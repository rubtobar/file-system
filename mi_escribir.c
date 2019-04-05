#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bloques.h"
//#include "ficheros_basico.h"
#include "directorios.h"

int main(int argc, char const *argv[]){

	if (argv[1] == '\0' || argv[2] == '\0' || argv[3] == '\0' || argv[4] == '\0'){
		printf(" ./mi_escribir disco /ruta_fichero offset [texto]\n");
		return(-1);
	}

	bmount(argv[1]);					//montamos archivo
	size_t strsize = strlen(argv[4]); 		//guardamos el numero de bloques entrados por argumento
	char *buffer = malloc(strsize);
	int offset = atoi(argv[3]);
	memcpy(buffer,argv[4],strsize);
	int bytes_escritos = 0;

	struct STAT stat;

	mi_stat(argv[2], &stat);

	if (stat.tipo != 'f'){
		printf("ERROR: el archivo a escribir no es un fichero\n");
		bumount(); //desmontamos archivo
		return(-1);
	}

	if ((stat.permisos & 2) == 0){
		printf("ERROR: el archivo no tiene permiso de escritura\n");
		bumount(); //desmontamos archivo
		return(-1);
	}

	bytes_escritos = mi_write(argv[2], buffer,offset, strsize);
	printf("se han escrito %d bytes\n", bytes_escritos);

	printf("\n");

	free((void *)buffer);

	bumount(); //desmontamos archivo

	return 0;
}