#include <semaphore.h>
#include <stdlib.h>

#define SEM_NAME "/mymutex" /* Usamos este nombre para el semáforo mutex */
#define SEM_INIT_VALUE 1 /* Valor inicial de los mutex */


sem_t *initSem();
void deleteSem();
void signalSem(sem_t *sem);
void waitSem(sem_t *sem);

/*
declaracion de semaforos y funciones

	-> bloques.c

funciones con semaforo implementado

	-> directorios.c -> mi_create
					 -> mi_link
					 -> mi_unlink
					 -> mi_chmod
					 -> mi_write
*/
