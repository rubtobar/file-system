#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "directorios.h"

int main(int argc, char const *argv[]){

	
	
	const char *camino = malloc(20);

	if (argv[1] == NULL || argv[2] == NULL || argv[3] == NULL){
		printf("./mi_mkdir disco permisos /ruta\n");
		return 0;
	}

	bmount(argv[1]); //montamos disco
	camino = argv[3];
	unsigned int p_inodo_dir = 0;
	unsigned int p_inodo = 0;
	unsigned int p_entrada = 0;
	char reservar = 1;
	unsigned char permisos = atoi(argv[2]);
	int salida;

	salida = buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,reservar,permisos);

	if (salida == -6){
		printf("El archivo o directorio ya existe\n");
	}
	//printf("num salida: %d\n", salida);
	//printf("inodo final: %d\n", p_inodo);
	//printf("hecho\n");

	bumount();

	return 0;
}