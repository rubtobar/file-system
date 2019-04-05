#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include "bloques.h"

#define posSB 0 //el superbloque se escribe en el primer bloque de nuestro FS
#define tamSB 1 
#define T_INODO 128 //tamaño en bytes de un inodo
#define DEBUG_FICHERO 0
//colores
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

struct superbloque{
	unsigned int posPrimerBloqueMB; //Posición del primer bloque del mapa de bits 
	unsigned int posUltimoBloqueMB; //Posición del último bloque del mapa de bits 
	unsigned int posPrimerBloqueAI; //Posición del primer bloque del array de inodos 
	unsigned int posUltimoBloqueAI; //Posición del último bloque del array de inodos 
	unsigned int posPrimerBloqueDatos; //Posición del primer bloque de datos 
	unsigned int posUltimoBloqueDatos; //Posición del último bloque de datos 
	unsigned int posInodoRaiz; //Posición del inodo del directorio raíz 
	unsigned int posPrimerInodoLibre; //Posición del primer inodo libre 
	unsigned int cantBloquesLibres; //Cantidad de bloques libres
	unsigned int cantInodosLibres; //Cantidad de inodos libres 
	unsigned int totBloques; //Cantidad total de bloques 
	unsigned int totInodos; //Cantidad total de inodos 
	char padding[BLOCKSIZE-12*sizeof(unsigned int)]; //Relleno(12 es la cantidad de elementos que tenemos)
};

struct inodo{ //comprobar que ocupa 128 bytes haciendo un sizeof(inodo)!!!
	unsigned char tipo; //Tipo (libre, directorio o fichero)
	unsigned char permisos; //Permisos (lectura y/o escritura y/o ejecución)
	/* Por cuestiones internas de alineación de estructuras, si se está utilizando un tamaño de palabra de 4 bytes (microprocesadores de 32 bits): unsigned char reservado_alineacion1 [2]; 
	en caso de que la palabra utilizada sea del tamaño de 8 bytes (microprocesadores de 64 bits): unsigned char reservado_alineacion1 [6]; */
	unsigned char reservado_alineacion1 [6];

	time_t atime; //Fecha y hora del último acceso a datos: atime
	time_t mtime; //Fecha y hora de la última modificación de datos: mtime
	time_t ctime; //Fecha y hora de la última modificación del inodo: ctime

	/* comprobar el tamaño del tipo time_t para vuestra plataforma/compilador: printf ("sizeof time_t is: %d\n", sizeof(time_t)); */
	unsigned int cardinal;//posicion dentro del array de inodos
	unsigned int nentradas; //numero de entradas en inodo
	unsigned int nlinks; //Cantidad de enlaces de entradas en directorio
	unsigned int tamEnBytesLog; //Tamaño en bytes lógicos
	unsigned int numBloquesOcupados; //Cantidad de bloques ocupados en la zona de datos

	unsigned int punterosDirectos[12]; //12 punteros a bloques directos
	unsigned int punterosIndirectos[3]; /*3 punteros a bloques indirectos:1 puntero indirecto simple, 1 puntero indirecto doble, 1 puntero indirecto triple */
	/* Utilizar una variable de alineación si es necesario para vuestra plataforma/compilador; 
	*/
	char padding[T_INODO-2*sizeof(unsigned int)-8*sizeof(unsigned char)-3*sizeof(time_t)-18*sizeof(unsigned int)]; //-6 para alineacion de palabra de 64 bits
	// Hay que restar también lo que ocupen las variables de alineación utilizadas!!!
};

struct STAT{
	unsigned char tipo; //Tipo (libre, directorio o fichero)
	unsigned char permisos; //Permisos (lectura y/o escritura y/o ejecución)

	time_t atime; //Fecha y hora del último acceso a datos: atime
	time_t mtime; //Fecha y hora de la última modificación de datos: mtime
	time_t ctime; //Fecha y hora de la última modificación del inodo: ctime

	unsigned int cardinal;//posicion dentro del array de inodos
	unsigned int nlinks; //Cantidad de enlaces de entradas en directorio
	unsigned int tamEnBytesLog; //Tamaño en bytes lógicos
	unsigned int numBloquesOcupados; //Cantidad de bloques ocupados en la zona de datos
};

/*
 * Funciones para calcular el tamaño en bloques.
 */
int tamMB(unsigned int nbloques);

int tamAI(unsigned int ninodos);


/*
 * Funciones para inicializar el sistema de ficheros.
 */
int initSB(unsigned int nbloques, unsigned int ninodos);

int initMB();

int initAI();

int leerSB();

int escribir_bit(unsigned int nbloque, unsigned int bit);

unsigned char leer_bit(unsigned int nbloque);

int leerMB();

//funciones básicas para reservar y liberar bloques

int reservar_bloque();

int liberar_bloque(unsigned int nbloque);

//funciones basicas para editar inodos

int escribir_inodo(struct inodo inodo, unsigned int ninodo);

struct inodo leer_inodo(unsigned int ninodo);

int reservar_inodo(unsigned char tipo, unsigned char permisos);

int leerAI();

//funcones basicas para trbajar a nivel de bloque logico

int obtener_nrangoBL(struct inodo inodo, int nblogico, int *ptr);

int obtener_indice(int nblogico, int nivel_punteros);

int traducir_bloque_inodo(unsigned int ninodo, unsigned int nblogico, char reservar);

int imprimir_bloque(int bloque);

int liberar_bloques_inodo(unsigned int ninodo, unsigned int nblogico);

int liberar_inodo(unsigned int ninodo);