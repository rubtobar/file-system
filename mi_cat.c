#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "bloques.h"
#include "directorios.h"


#define BUFFERSIZE 1500

int main(int argc, char const *argv[]){

	int filedescriptor = 1;

	if (argv[1] == '\0' || argv[2] == '\0'){
		printf("./mi_cat disco /ruta\n");
		return(-1);
	}

	bmount(argv[1]);//montamos archivo

	
	struct STAT stat;

	mi_stat(argv[2], &stat);

	if (stat.tipo == 'd'){
		printf("ERROR: el archivo a leer es un directorio\n");
		bumount(); //desmontamos archivo
		return(-1);
	}

	if ((stat.permisos & 4) == 0){
		printf("ERROR: el archivo no tiene permiso de lectura\n");
		bumount(); //desmontamos archivo
		return(-1);
	}

	if (stat.tamEnBytesLog == 0){
		printf("El archivo esta vacio\n");
		bumount(); //desmontamos archivo
		return(0);
	}
/*
	if (argc == 5){
		if ((*argv[3] == '>') && (argv[4] != '\0')){
			//abrimos el archivo
			filedescriptor = open(argv[4], O_RDWR|O_CREAT, 0666);
			//constatamos errores
			if (filedescriptor < 0) {
				printf("ERROR intentando abrir el archivo\n");
				return -1;
			}
		}
	}
	//printf("stat %d\n", stat.tamEnBytesLog);
*/
	int bloquesALeer = stat.tamEnBytesLog / BUFFERSIZE;		//bloques que hay que leer
	int bytesUltBloque = stat.tamEnBytesLog % BUFFERSIZE;	//ultimos bytes a leer
	char *bufferlectura = malloc(BUFFERSIZE);
	char *bufferBytes = malloc(bytesUltBloque);
	char *trim = bufferlectura;

	//printf("bloquesALeer %d\n", bloquesALeer);
	//printf("bytesUltBloque %d\n", bytesUltBloque);

	memset(bufferlectura,0,BUFFERSIZE);
	
	int bloque_leidos = 0;
	int i;
	int pr;
	int tr = 0;
	for (i = 0; bloque_leidos < bloquesALeer; i++){

		memset(bufferlectura,0,BUFFERSIZE);
		//printf("i: %d\n", i);
		trim = bufferlectura;
		tr = 0;
		pr = mi_read(argv[2], bufferlectura, i*BUFFERSIZE, BUFFERSIZE);
		//printf("pr%d\n", pr);
		if (pr >= 0){
			//sleep(1);
			/*
			while(!(*trim)){

				trim++;
				tr++;
			}
			*/
			write(filedescriptor,trim,BUFFERSIZE-tr);
			//printf("i: %d\n", i);
		}else{
			lseek(filedescriptor, bloque_leidos*BUFFERSIZE, SEEK_SET);
		}
		bloque_leidos++;
	}

	memset(bufferBytes,0,bytesUltBloque);
	if(bytesUltBloque > 0){
		//leemos hasta encontrar el ultimo bloque con los bytes que faltan
		int salida = 0;
		while(salida <= 0){
		//printf("i*BUFFERSIZE %d\n", i*BUFFERSIZE);

			salida = mi_read(argv[2], bufferBytes, i*BUFFERSIZE, bytesUltBloque);
			i++;
		}
		write(filedescriptor,bufferBytes,bytesUltBloque); //escribimos los ultimos bytes
	}	

	free(bufferlectura);
	free(bufferBytes);

	bumount(); //desmontamos archivo
/*
	if (argc == 5){
		if (*argv[3] == '>' && argv[4] != '\0'){
			close(filedescriptor);
		}
	}
*/
	return 0;
}