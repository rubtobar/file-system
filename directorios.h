#include <string.h>
#include "ficheros.h"

#define ERROR_PARAMETROS -2
#define ERROR_PERMISOS -3
#define ERROR_DIRECTORIO_INTERMEDIO -4
#define ERROR_ESCRITURA -5
#define ERROR_DIRECTORIO_EXISTENTE -6
#define ERROR_NO_EXISTE -7
#define ERROR_CREAR_EN_FICHERO -8

struct entrada{
	char nombre[60]; //En el sistema de ficheros ext2 la longitud del nombre es 255
	unsigned int inodo;
};

int extraer_camino(const char *camino, char *inicial, char *final);

int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos);

int mi_create(const char *camino, unsigned char permisos);

int mi_dir(const char *camino, char *buffer);

int mi_link(const char *camino1, const char *camino2);

int mi_unlink(const char *camino);

int mi_chmod(const char *camino, unsigned char permisos);

int mi_stat(const char *camino, struct STAT *p_stat);

int mi_read(const char *camino, void *buf, unsigned int offset, unsigned int nbytes);

int mi_write(const char *camino, const void *buf, unsigned int offset, unsigned int nbytes);