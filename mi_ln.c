#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "directorios.h"

int main(int argc, char const *argv[]){

	const char *camino = malloc(20);
	const char *camino2 = malloc(60);
	char *buffer = malloc(60);
	char *buffer2 = malloc(60);

	if (argv[1] == NULL || argv[2] == NULL || argv[3] == NULL){
		printf("./mi_ln disco.imagen /ruta1 /ruta2\n");
		return 0;
	}

	bmount(argv[1]);

	camino = argv[2];
	camino2 = argv[3];

	mi_link(camino, camino2);

	free(buffer);
	free(buffer2);

	bumount();

	return 0;
}