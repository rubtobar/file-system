#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "directorios.h"

int main(int argc, char const *argv[]){

	if (argv[1] == NULL || argv[2] == NULL){
		printf("./mi_ln disco.imagen /ruta\n");
		return 0;
	}

	bmount(argv[1]);

	mi_unlink(argv[2]);


	bumount();

	return 0;
}