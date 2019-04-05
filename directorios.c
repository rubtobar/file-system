#include "directorios.h"

int extraer_camino(const char *camino, char *inicial, char *final){

	int longitud = strlen(camino);
		
	if (longitud <= 0){
		return(-1);
	}
	
	int i = 1; /* Saltamos el carácter '/' */
	while ((camino[i] != '\0') && (camino[i] != '/') && (i < longitud)) {
		inicial[i-1] = camino [i];
		i++;
	}
	
	inicial[i-1] = '\0';
	
	int j = 0;
	while (camino[i] != '\0') {
		final[j] = camino[i];
		i++;
		j++;
	}
	
	final[j] = '\0';

	if (*final == '\0'){
		return(1);//es un fichero
	}
	return (0); // es un directorio

}

int buscar_entrada (const char *camino_parcial,unsigned int *p_inodo_dir,unsigned int *p_inodo,unsigned int *p_entrada,char reservar,unsigned char permisos) {
	
	struct inodo nodo;
    struct inodo nodoaux;
    int numentradas;
    struct entrada entr;
    char *inicial = malloc(60);
    char *final = malloc(60);
    char tipo;
    int nentrada = 0;  //nº de entrada inicial
    int estado;
    int entradalibre = 0;

    if (strcmp(camino_parcial,"/") == 0) {
        *p_inodo = 0;  //la raiz siempre estara asociada al inodo 0
        *p_entrada = 0;
        free(inicial);
        free(final);
        return(0);
    }

    if (strcmp(camino_parcial,"") == 0) {
        free(final);
        free(inicial);
        return -1;
    }

    if (extraer_camino (camino_parcial, inicial, final) == 0) {  
    	tipo = 'd';
    }else{
    	tipo = 'f';
    }

    //buscamos la entrada cuyo nombre se encuentra en inicial
    nodo = leer_inodo(*p_inodo_dir);   //leer el inodo indicado por *p_inodo_dir%s
    //inicializar el nombre de la entrada con '\0'
    numentradas = nodo.nentradas;    //calcular el nº de entradas del inodo
    if (numentradas > 0) {
        if ((nodo.permisos & 4) != 4) { 
        	free(inicial);
        	free(final);
        	return -2;
        }
        while (nentrada < numentradas) { 

            mi_read_f(*p_inodo_dir, &entr, nentrada * sizeof(entr), sizeof(entr));  //leer siguiente entrada;


            if (strcmp(entr.nombre, inicial) == 0){
                break;
            }

            nentrada++;
        }
    }

    if (nentrada == numentradas) { //la entrada no existe
        switch (reservar){
            case 0:  //modo consulta. Como no existe retornamos error
            	free(inicial);
        		free(final);
	            return -3;
            case 1:  //modo escritura. 
                //Creamos la entrada en el directorio referenciado por *p_inodo_dir
        	    if (tipo == 'd'){
        	        if (strcmp(final, "/") == 0) {        
                        //reservar un inodo como directorio y asignarlo a la entrada
                        //printf("reservando directorio\n");
                        nodoaux = leer_inodo(*p_inodo_dir);
                        if (nodoaux.tipo != 'd'){
                            printf("ERROR: la ruta no corresponde a un directorio\n");
                            return(-10);
                        }
                        memcpy(entr.nombre,inicial,60); //copiar inicial en el nombre de la entrada
                        entr.inodo = reservar_inodo('d',permisos);
        	        } else {
        	        	free(inicial);
        				free(final);
                        return -4;
        	        }
        	    } else { 
                    //reservar un inodo como fichero y asignarlo a la entrada
                    //printf("reservando fichero\n");
                    nodoaux = leer_inodo(*p_inodo_dir);
                    if (nodoaux.tipo != 'd'){
                        printf("ERROR: la ruta no corresponde a un directorio\n");
                        return(-10);
                    }
                    memcpy(entr.nombre,inicial,60);
                    entr.inodo = reservar_inodo('f',permisos);
                }
        	    //escribir la entrada

                entradalibre = nodo.nentradas;
                nodo.nentradas++;
                escribir_inodo(nodo,*p_inodo_dir);

        	    if (mi_write_f(*p_inodo_dir, &entr, entradalibre * sizeof(struct entrada), sizeof(struct entrada)) < 0) {
                    printf("error al escribir\n");
        	        if (entr.inodo != -1) {  //entrada.inodo != -1        
                        liberar_inodo(entr.inodo);
                    }
        	        free(inicial);
        			free(final);
                    return -5;
        	    }                
        }
    }
	
    //si hemos llegado al final del camino
    if (strcmp(final,"/") == 0 || strcmp(final,"") == 0) { 
        if ((nentrada < numentradas) && (reservar==1)) { //modo escritura y la entrada ya existe
        	free(inicial);
        	free(final);
	    	return -6;
        }
        // cortamos la recursividad
        //asignar a *p_inodo el numero de inodo del directorio/fichero creado/leido
        *p_inodo = entr.inodo;
        //asignar a *p_entrada el número de su entrada dentro del último directorio que lo contiene
        *p_entrada = nentrada;
        free(inicial);
        free(final);
        return 0;
    } else {
        //asignamos a *p_inodo_dir el inodo de la entrada;
        *p_inodo_dir = entr.inodo;

        estado = buscar_entrada(final, p_inodo_dir, p_inodo, p_entrada, reservar, permisos);
        free(inicial);
        free(final);
        return estado;
    }
    return 0;
}

int mi_create(const char *camino, unsigned char permisos){

    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    char reservar = 1;

    //seccion critica
    mi_waitSem();
    int salida = buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,reservar,permisos);
    mi_signalSem();

    return salida;
}

int mi_dir(const char *camino, char *buffer){
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    struct inodo nodo;
    struct entrada entr;
    unsigned int numentradas;
    int nentrada = 0;
    
    memset(buffer, 0, 1); //quitamos basura del buffer

    if(buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,0,0) < 0){
        return -1;
    }
    nodo = leer_inodo(p_inodo);
    numentradas = nodo.nentradas;    //calcular el nº de entradas del inodo

    if (numentradas > 0) {
        if ((nodo.permisos & 4) != 4) { 
            return -2;
        }
        if (nodo.tipo != 'd'){
            return -3;
        }
        while (nentrada < numentradas) {  
            mi_read_f(p_inodo, &entr, nentrada * sizeof(entr), sizeof(entr));  //leer siguiente entrada;
            strcat(buffer,entr.nombre);
            strcat(buffer,"\t");
            nentrada++;
        }
    }
    return 0;
}


int mi_link(const char *camino1, const char *camino2){

	unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    struct inodo nodo1;
    int estado;
    struct entrada entr1;
    struct entrada entr2;

	if(buscar_entrada(camino1,&p_inodo_dir,&p_inodo,&p_entrada,0,0) < 0){
		printf("%s no existe\n", camino1);
        return -1;
    }
    mi_read_f(p_inodo_dir, &entr1, p_entrada * sizeof(entr1), sizeof(entr1)); //guardamos la entrada para ponerla en el otro directorio
    nodo1 = leer_inodo(p_inodo); //nodo al que tenemos que referenciar
    nodo1.atime = time(NULL);
    nodo1.nlinks++;

    //seccion critica
    mi_waitSem();
    escribir_inodo(nodo1,p_inodo);
    mi_signalSem();

    p_inodo_dir = 0;
    p_inodo = 0;
    p_entrada = 0;

    //seccion critica
    mi_waitSem();
    estado = buscar_entrada(camino2,&p_inodo_dir,&p_inodo,&p_entrada,1,7);
    mi_signalSem();

    if(estado < 0){
    	printf("el archivo %s ya existe\n", camino2);
    	nodo1.atime = time(NULL);
    	nodo1.nlinks++;

        //seccion critica
        mi_waitSem();
    	escribir_inodo(nodo1,nodo1.cardinal);
        mi_signalSem();

        return -1; //caso de error
    }

    mi_read_f(p_inodo_dir, &entr2, p_entrada * sizeof(entr2), sizeof(entr2));

    //seccion critica
    mi_waitSem();
    liberar_inodo(p_inodo);  //liberamos el nodo archivo creado
    entr2.inodo = entr1.inodo;
    mi_write_f(p_inodo_dir, &entr2, p_entrada * sizeof(entr2), sizeof(entr2));
    mi_signalSem();

    return 0;
}

int mi_unlink(const char *camino){

	unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    struct inodo nodo;
    struct entrada entr;
    struct entrada entraux;

	if(buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,0,0) < 0){
		printf("%s no existe\n", camino);
        return -1;
    }
    mi_read_f(p_inodo_dir, &entr, p_entrada * sizeof(entr), sizeof(entr));
    nodo = leer_inodo(p_inodo_dir);

    //caso en que la entrada a borrar sea la ultima del directorio
    if (p_entrada == nodo.nentradas - 1){

        //seccion critica
        mi_waitSem();
    	mi_truncar_f(nodo.cardinal, nodo.tamEnBytesLog - sizeof(entr));
    	nodo = leer_inodo(p_inodo_dir);
    	nodo.nentradas--;
    	escribir_inodo(nodo,nodo.cardinal);
        mi_signalSem();

    	//a continuacion quitamos el link del nodo y si es 0 lo liberamos
    	nodo = leer_inodo(entr.inodo);
    	if (nodo.nlinks == 1){

            //seccion critica
            mi_waitSem();
    		liberar_inodo(nodo.cardinal);
            mi_signalSem();

    	}else{
    		nodo.nlinks--;

            //seccion critica
            mi_waitSem();
    		escribir_inodo(nodo,nodo.cardinal);
            mi_signalSem();

    	}
    //caso en que la entrada este por enmedio del directorio
    }else{
    	//substituimos la entrada de enmedio por la ultima, luego trunkamos el directorio
    	mi_read_f(p_inodo_dir, &entraux, (nodo.nentradas-1) * sizeof(entr), sizeof(entr));

        //seccion critica
        mi_waitSem();
    	mi_write_f(p_inodo_dir, &entraux, p_entrada * sizeof(entr), sizeof(entr));
    	mi_truncar_f(nodo.cardinal, nodo.tamEnBytesLog - sizeof(entr));
    	nodo = leer_inodo(p_inodo_dir);
    	nodo.nentradas--;
    	escribir_inodo(nodo,nodo.cardinal);
        mi_signalSem();

    	//a continuacion quitamos el link del nodo y si es 0 lo liberamos
    	nodo = leer_inodo(entr.inodo);
    	if (nodo.nlinks == 1){
            
            //seccion critica
            mi_waitSem();
    		liberar_inodo(nodo.cardinal);
            mi_signalSem();
    	}else{
    		nodo.nlinks--;

            //seccion critica
            mi_waitSem();
    		escribir_inodo(nodo,nodo.cardinal);
            mi_signalSem();

    	}
    }

    return 0;
    
}

int mi_chmod(const char *camino, unsigned char permisos){

	unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int salida;

    if(buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,0,0) < 0){
        return -1;
    }

    //seccion critica
    mi_waitSem();
    salida = mi_chmod_f(p_inodo, permisos);
    mi_signalSem();

    return(salida);
}

int mi_stat(const char *camino, struct STAT *p_stat){

	unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
	int salida;

    //seccion critica
    mi_waitSem();
	salida = buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,0,0);

	if((salida < 0) && (salida =! -6)){
        printf("error\n");
		return -1;
	}
    salida = mi_stat_f(p_inodo, p_stat);
    mi_signalSem();

	return(salida);
}

int mi_read(const char *camino, void *buf, unsigned int offset, unsigned int nbytes){

	unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
	int ret;

	if(buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,0,0) < 0){
		return -1;
	}

	ret = mi_read_f(p_inodo, buf,offset,nbytes);
	return(ret);
}

int mi_write(const char *camino, const void *buf, unsigned int offset, unsigned int nbytes){

	unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
	int ret;

    //seccion critica
    mi_waitSem();
	if(buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,0,0) < 0){
		return -1;
	}
    mi_signalSem();
    
    //seccion critica
    mi_waitSem();
	ret = mi_write_f(p_inodo, buf,offset,nbytes);
    mi_signalSem();

	return(ret);
}
