#include "ficheros_basico.h"

//lectura y escritura

int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes);

int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes);

int leer_bloque(int nbloque);

int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes);

int mi_chmod_f(unsigned int ninodo, unsigned char permisos);

int mi_truncar_f(unsigned int ninodo, unsigned int nbytes);

int mi_stat_f(unsigned int ninodo, struct STAT *p_stat);

int leer_bloque_entradas(int nbloque);