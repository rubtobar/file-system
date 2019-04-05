#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "directorios.h"

int main(int argc, char const *argv[]){

	const char *camino = malloc(60);
	char *buffer;// = malloc(4096);	//no sabemos cuan larga sera la cadena con los directorios suponemos 4096 como MAX;
	struct STAT stat;

	if (argv[1] == NULL || argv[2] == NULL){
		printf("./mi_ls disco /ruta_directorio\n");
		return 0;
	}

	bmount(argv[1]);
	camino = argv[2];

	mi_stat(argv[2],&stat);

	if (stat.tipo == 'f'){
		bumount();
		printf("La ruta especificada no pertenece a un directorio\n");
		return(-1);
	}

	buffer = malloc(stat.tamEnBytesLog);

	mi_dir(camino, buffer);

	if (strcmp(buffer,"") == 0){
		printf("Directorio vacio\n");
	}else{
		printf("%s\n", buffer);
	}

	free(buffer);
	buffer = NULL;
	bumount();

	return 0;
}