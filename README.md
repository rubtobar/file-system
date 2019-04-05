Informe práctica SSOO2

Rubén Tobar Nicolau
Llorenç Borràs Socíes
Funcionamiento global

La práctica consiste en la construcción de un sistema de ficheros gestionado mediante las bibliotecas: directorios, ficheros, ficheros_basicos y bloques.

En primer lugar, el sistema de ficheros se dividirá en bloques todos del mismo tamaño, estos bloques serán referenciados mediante el uso de la biblioteca directorios.
Esta biblioteca se ocupará de las escrituras y lecturas en los diferentes bloques del sistema de ficheros, así como de montar y desmontar el sistema de ficheros.

Una vez el sistema de ficheros esté dividido en bloques, debemos inicializarlo de la siguiente manera:

El primer bloque será utilizado como super bloque, este contendrá la información sobre el sistema de ficheros.
Los siguientes bloques son el mapa de bits, donde veremos representados todos los bloques según su estado (libre/reservado) y el array de inodos.
Estas dos partes del sistema de ficheros tendrán un tamaño variable que dependerá del tamaño de nuestro disco.
Por último los bloques restantes serán los bloques de datos, donde guardaremos los datos de los diferentes ficheros y directorios(además de los bloques de punteros que estos utilicen).
Esta inicialización se hace mediante la librería ficheros_basicos.


De esta manera el funcionamiento general de nuestro sistema de ficheros sería el siguiente:

Cada inodo contendrá punteros a los bloques que este referencia, de esta manera cada inodo será un archivo o directorio y dará la posibilidad de acceder a su contenido a través de este.

En el momento de aumentar el tamaño de un archivo/directorio este reservará nuevos bloques libres utilizando el array de inodos para saber cuales son, en caso de truncar el archivo los bloques sobrantes se liberan.
Los inodos funcionan de la misma manera, con la diferencia que se encuentran en una lista enlazada, al reservar uno de estos, este sale de la lista y al liberarlo vuelve a entrar.

Los inodos utilizan un sistema de bloques de punteros gracias al cual puede referenciar a gran cantidad de bloques con únicamente 12 punteros directos y 3 indirectos(simples, dobles, triples), esto quiere decir que reservaran bloques extra para utilizarlos como punteros para referenciar más bloques a cada nivel.

Los inodos directorio guardan los nombres y cardinalidades de los demás inodos de tal forma que mediante un directorio podamos acceder a otros inodos utilizando su nombre en lugar de su posición en el array.
Utilizaremos un directorio raíz el cual será el directorio desde el que accederemos a todos los demás además de a los archivos que contengan.

Este funcionamiento vienen dadas mediante las librerías: ficheros_basico y directorios.

Además también implementan un sistema de semáforos para evitar las escrituras simultáneas que puedan producir errores en el sistema de ficheros mediante semáforos MUTEX.

Funciones y funcionamiento
Librería bloques

int bmount(const char *camino);
Función encargada de montar el sistema de ficheros, esta realiza un open() del archivo en el que se encuentra nuestro disco, en caso que no exista lo crea.

int bumount();
Se encarga de desmontar el SF cerrando el archivo que contiene el disco.

int bwrite(unsigned int nbloque, const void *buf);
Función encargada de escribir en un determinado bloque del sistema de ficheros, siempre escribe encima de todo el bloque.
Escribe el buffer pasado por parámetro, este debe tener el tamaño de un bloque.

int bread(unsigned int nbloque, void *buf);
Función encargada de leer un bloque, al igual que lectura siempre lee un bloque entero y lo escribe en la posición del buffer.
Estas dos funciones utilizan la función lseek() para posicionar el puntero de escritura en la posición referente al bloque a leer/escribir.

void mi_waitSem();
Entra en la sección crítica del semáforo MUTEX.

void mi_signalSem();
Sale de la sección crítica del semáforo MUTEX.


Librería ficheros_basico

Funciones de inicialización

int initSB(unsigned int nbloques, unsigned int ninodos);
Esta función escribe el Superbloque en el primer bloque del SF.
En este escribe el número de bloques, así como el número de inodos además de escribir en qué bloque empieza el mapa de bits y en qué bloque empieza el array de inodos.
y otros datos de importancia para el funcionamiento del SF.

int initMB();
Inicializa el mapa de bits poniendo a 1 los bits que representan los bloques utilizados por el SB el MB y el AI, los demás que dan a 0.

int initAI();
Inicializa el array de inodos como una lista enlazada de estos, utilizando el puntero directo 0 como puntero al siguiente inodo de la lista.

int leerSB();
Imprime los datos del SB por pantalla.

int escribir_bit(unsigned int nbloque, unsigned int bit);
Función encargada de poner a 1 o 0 el bit correspondiente del mapa de bits, esta la utilizaremos en el momento de reservar un bloque.

unsigned char leer_bit(unsigned int nbloque);
Devuelve el valor de un bit del mapa de bits, función utilizada para saber si un bloque está reservado o no.
Únicamente es utilizada para imprimir el mapa de bits por pantalla como 0’s y 1’s.

int leerMB();
Esta función es un bucle que recorre el mapa de bits con la función anterior para imprimir cada uno de los valores que este contiene por pantalla(imprime la cardinalidad de cada uno e los bits a 1, de esta manera sabremos que bloques están ocupados en el SF).

Funciones básicas para reservar y liberar bloques

int reservar_bloque();
Recorre el array de inodos en busca del primer bloque libre, una vez encontrado reserva el bloque y devuelve su posición.
El proceso de búsqueda se hace primero de bloque en bloque, después de byte en byte y por último de bit en bit.
De esta manera nos vamos saltando todos los bloques y bytes que contienen todo 1’s (bloques reservados).
Esto aumenta la eficiencia a medida que el el SF va llenando la sección de bloques de datos.

int liberar_bloque(unsigned int nbloque);
Esta función se encarga de liberar el bloque introducido por parámetro poniendo a 0 su correspondiente bit del MB.

Funciones básicas para tratamiento de inodos

int escribir_inodo(struct inodo inodo, unsigned int ninodo);
Función encargada de escribir la estructura inodo en el lugar correspondiente del array de inodos.

struct inodo leer_inodo(unsigned int ninodo);
Lee el inodo del array de inodos de la posición indicada por ninodo.

int reservar_inodo(unsigned char tipo, unsigned char permisos);
Al llamar a esta función sacamos el primer inodo de la lista enlazada de inodos y lo reservamos(lo sacamos de la lista).
Este queda inicializado con el tipo y los permisos introducidos, los demas parametros quedan a 0.

int leerAI();
Imprime los campos del inodo introducido por parámetro por pantalla, en caso de introducir -1, se imprimen todos.

Funciones basicas para trabajar a nivel de bloque lógico

int obtener_nrangoBL(struct inodo inodo, int nblogico, int *ptr);
Esta función, dado un número de bloque lógico, un inodo al cual queremos acceder y un puntero, retorna el nivel en el que se encuentra el bloque lógico y el puntero con la dirección específica del inodo en ese nivel.

int obtener_indice(int nblogico, int nivel_punteros);
Retorna el índice en el que se encuentra el siguiente puntero del bloque de punteros, este depende del nivel de punteros y del blogico.

int traducir_bloque_inodo(unsigned int ninodo, unsigned int nblogico, char reservar);
Esta función traduce el bloque lógico del inodo al bloque físico al que referenciar en el SF.
La función utiliza las dos anteriores funciones y en caso de que reservar este a 1 reservará el bloque físico, en caso de que sea 0 retorna error.

int imprimir_bloque(int bloque);
Función de prueba, imprime un bloque de punteros por pantalla.

int liberar_bloques_inodo(unsigned int ninodo, unsigned int nblogico);
Esta función trunca los bloques lógicos del inodo, liberandolos mediante bucles hasta dejar el tamaño del inodo como nblogico (en caso de poner a 0, el inodo queda sin bloques asociados).
Los bloques lógicos que no estaban reservados no se tienen en cuenta (archivos dispersos).

int liberar_inodo(unsigned int ninodo);
Realiza la misma operación que la anterior función, pero liberando todos los bloques (nblogico a 0).

Librería ficheros

Funciones de lectura, escritura y modificación por inodo

int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes);
Escribe el buffer en el inodo especificado por parámetro a partir del bloque lógico que viene dado por el offset (en bytes).
En el caso que la escritura ocupe más de un bloque, se realizarán las escrituras en cada uno de los bloques intermedios, en el primero y el último.
Los datos del bloque que no se deben sobrescribir se preservan durante la escritura utilizando un buffer auxiliar.

int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes);
Realiza la misma operación que la escritura, pero leyendo los datos y poniéndolos en el buffer.

int leer_bloque(int nbloque);
Imprime por pantalla el contenido en datos de un bloque del SF.

int mi_chmod_f(unsigned int ninodo, unsigned char permisos);
Esta función modifica los permisos de el nodo especificado sobreescribiendo.
No modifica los demás datos del inodo.

int mi_truncar_f(unsigned int ninodo, unsigned int nbytes);
Realiza la misma función que liberar bloques inodo pero utilizando el tamaño en bytes.

int mi_stat_f(unsigned int ninodo, struct STAT *p_stat);
Retorna la estructura STAT de un inodo (la estructura STAT contiene todos los campos de un inodo a excepción de los punteros a bloques).

int leer_bloque_entradas(int nbloque);
Imprime por pantalla un bloque con entradas de tal forma que sea legible para el usuario.


Librería directorios

Funciones de tratamiento de ficheros por ruta

int extraer_camino(const char *camino, char *inicial, char *final);
Esta función divide la ruta introducida en camino en la parte inicial y final.
La parte inicial sería el texto antes del primer “/”, el resto queda como final.
Esta función se llama varias veces para ir desglosando la ruta y recorrerla hasta llegar a el archivo/directorio.

int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos);
Esta función recursiva se ocupa de buscar la entrada en el inodo directorio en el que nos encontramos (empezando por la raíz).
En caso de que no esté creada la entrada en el directorio, y estemos reservando, se creará la entrada y reservara el inodo que le corresponda.

*Las siguientes funciones realizan la misma operación que algunas de las anteriores (únicamente llaman a las mencionadas traduciendolos la ruta a inodo).

int mi_create(const char *camino, unsigned char permisos);
Crea la nueva entrada y reserva el inodo para crear el camino especificado.

int mi_dir(const char *camino, char *buffer);
Lee todas las entradas de un inodo directorio y las almacena como string en el buffer.

int mi_link(const char *camino1, const char *camino2);
Duplica la entrada de un archivo para que este quede duplicado (el inodo pasará a tener +1link).
El inodo será el mismo para ambas entradas.

int mi_unlink(const char *camino);
Realiza el procedimiento contrario al anterior.
Elimina una de las entradas al inodo, en caso de que el nodo se quede sin entradas lo libera (liberando previamente todos sus bloques).

int mi_chmod(const char *camino, unsigned char permisos);
Modifica los permisos del inodo correspondiente (al de la ruta), sobreescribiendo con los permisos introducidos por parámetro.

int mi_stat(const char *camino, struct STAT *p_stat);
Llama a la función mi_stat_f traduciendo la ruta al inodo correspondiente.

int mi_read(const char *camino, void *buf, unsigned int offset, unsigned int nbytes);
Llama a la función mi_write_f traduciendo la ruta al inodo correspondiente.

int mi_write(const char *camino, const void *buf, unsigned int offset, unsigned int nbytes);
Llama a la función mi_read_f traduciendo la ruta al inodo correspondiente.


Primitivas

mi_mkfs, mi_mkdir, mi_chmod, mi_ls, mi_stat, mi_cat, mi_ln, mi_rm
Estas primitivas imitan el funcionamiento de las respectivas en los sistemas Linux.
Únicamente operan sobre nuestro sistema de ficheros el cual se debe introducir como parámetro al llamarlas.

mi_escribir
Est primitiva escribe un texto entrado por argumento en el sistema de ficheros (en la ruta[existente] especificada por el usuario).
Funciona para varios offsets de tal forma que el texto de entrada se escribe empezando en la posición indicada.

leerSF
Esta primitiva imprime por pantalla los datos del SB, los inodos ocupados y los bloques reservados en el SF.


