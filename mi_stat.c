#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "directorios.h"

int main(int argc, char const *argv[]){	
	
	const char *camino = malloc(20);
	struct STAT stat;
	struct tm *ts;
	char atime[80];
	char mtime[80];
	char ctime[80];

	if (argv[1] == NULL || argv[2] == NULL){
		printf("./mi_stat disco /ruta\n");
		return 0;
	}

	bmount(argv[1]); //montamos disco
	camino = argv[2];

	if (mi_stat(camino, &stat) < 0){
		printf("no existe\n");
		return(-1);
	}
	
	printf("#####################{%s}#####################\n", camino);

	printf("inodo: %d\n", stat.cardinal);
	printf("tipo: ");
	if (stat.tipo == 'd'){
		printf("directorio\n");
	}else{
		printf("fichero\n");
	}

	printf("permisos: ");
	if ((stat.permisos & 4) == 0){
		printf("-");
	}else{
		printf("r");
	}
	if ((stat.permisos & 2) == 0){
		printf("-");
	}else{
		printf("w");
	}
	if ((stat.permisos & 1) == 0){
		printf("-");
	}else{
		printf("x");
	}
	printf("\n");

	printf("nlinks: %d\n", stat.nlinks);
	printf("tamEnBytesLog: %d\n", stat.tamEnBytesLog);
	printf("numBloquesOcupados: %d\n", stat.numBloquesOcupados);

	ts = localtime(&stat.atime);
	strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
	ts = localtime(&stat.mtime);
	strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
	ts = localtime(&stat.ctime);
	strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);
	
	printf("ACCESSTIME: %s \nMODIFYTIME: %s \nCHANGETTIME: %s\n",atime,mtime,ctime);

	printf("##################################################\n");

	bumount();

	return 0;
}