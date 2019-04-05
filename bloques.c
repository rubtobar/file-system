#include "bloques.h"

static int filedescriptor = 0;

static sem_t * mimutex;

int inside_sc = 0;

int bmount(const char *camino){

	filedescriptor = open(camino, O_RDWR|O_CREAT, 0666);

	if (filedescriptor < 0) {
		printf("ERROR intentando abrir el archivo\n");
		return -1;
	}
	else{
		if (DEBUG_BLOCK == 1){
			printf("Abriendo archivo\n");
		}
	}

	if (!mimutex) {
        mimutex = initSem();
    }


	return filedescriptor;
}


int bumount(){

	if(close(filedescriptor) < 0){
		printf("ERROR cerrando el archivo\n");
		return -1;
	}

	else{
		if (DEBUG_BLOCK == 1)	{
			printf("Cerrando el archivo\n");
		}
	}
	if (mimutex){
		deleteSem();
	}
	
		
	return 1;

}


int bwrite(unsigned int nbloque, const void *buf){
	//colocamos el puntero sobre la posicion a escribir
	if(lseek(filedescriptor, nbloque*BLOCKSIZE, SEEK_SET) < 0){
		printf("ERROR del puntero en la escritura(%u)\n",nbloque);
		return -1;
	}
	//escritura
	int bwr = write(filedescriptor, buf, BLOCKSIZE);

	if(bwr < 0){
		printf("ERROR al escribir\n");
		return -1;
	}
	if(DEBUG_BLOCK == 1){
		printf("Escritura correcta.: %d bytes escritos en bloque %d\n", bwr, nbloque);
	}
	return 1;
}


int bread(unsigned int nbloque, void *buf){
	//colocamos el puntero sobre la posicion a leer
	if(lseek(filedescriptor, nbloque*BLOCKSIZE, SEEK_SET) < 0){
		printf("ERROR del puntero el la lectura\n");
		return -1;
	}
	//lectura
	if(read(filedescriptor, buf, BLOCKSIZE) < 0){
		printf("ERROR en la lectura\n");
		return -1;
	}
	if(DEBUG_BLOCK == 1){
		printf("Lectura correcta\n");
	}
	return 1;
}

void mi_waitSem(){
    waitSem(mimutex);
}

void mi_signalSem(){
    signalSem(mimutex);
}