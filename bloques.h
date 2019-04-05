#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> /* Modos de apertura y función open()*/
#include "semaforo_mutex_posix.h"

#define DEBUG_BLOCK 0

#define BLOCKSIZE 1024 //bytes

int bmount(const char *camino);

int bumount();

int bwrite(unsigned int nbloque, const void *buf);

int bread(unsigned int nbloque, void *buf);

void mi_waitSem();

void mi_signalSem();