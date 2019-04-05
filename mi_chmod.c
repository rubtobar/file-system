#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "directorios.h"

int main(int argc, char const *argv[]){

	
	
	const char *camino = malloc(20);

	if (argv[1] == NULL || argv[2] == NULL || argv[3] == NULL){
		printf("./mi_chmod disco permisos /ruta\n");
		return 0;
	}

	bmount(argv[1]); //montamos disco
	camino = argv[3];
	unsigned char permisos = atoi(argv[2]);

	mi_chmod(camino,permisos);

	bumount();

	return 0;
}