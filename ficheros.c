#include "ficheros.h"

int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes){

	struct inodo nodo = leer_inodo(ninodo);
	int priBlogico = offset/BLOCKSIZE;
	int ultBlogico = (offset+nbytes-1)/BLOCKSIZE;
	unsigned char buffer[BLOCKSIZE];
	int desp = offset%BLOCKSIZE;//desplazamiento escritura en el primer bloque
	int blFisico;
	int bloques_intermedios = ultBlogico - priBlogico - 1; //le quitamos el primero y el ultimo

	if ((nodo.permisos & 2) != 2){
		printf("ERROR: el nodo no tiene permisos de escritura (nodo: %d)\n",ninodo);
		return(-1);//el inodo no tiene permiso de escritura
	}
/*
	Primeramente trataremos el caso en que el primer y último bloque coincidan, 
	y por tanto el buffer que vayamos a escribir cabe en un solo bloque.
*/

	memset(buffer,0,BLOCKSIZE);

	if (priBlogico == ultBlogico){
		
		blFisico = traducir_bloque_inodo(ninodo, priBlogico, 1);
		//printf("priBlogico %d\n", priBlogico);
		//printf("bloque fisico: %d\n", blFisico);
		bread(blFisico,buffer);
		memcpy(buffer + desp,buf_original,nbytes);
		bwrite(blFisico,buffer);

		//leer_bloque(blFisico);
		nodo = leer_inodo(ninodo); 
		if (nodo.tamEnBytesLog < (nbytes + offset)){
			nodo.tamEnBytesLog = (nbytes + offset);
			escribir_inodo(nodo,ninodo);
		}

		return(nbytes);
	}
/*
	A continuación trataremos el caso en que la operación afecte a más de un bloque.
*/
	else if (priBlogico < ultBlogico){
		//escribimos primer bloque
		blFisico = traducir_bloque_inodo(ninodo, priBlogico, 1);
		bread(blFisico,buffer);
		memcpy(buffer+desp,buf_original,BLOCKSIZE-desp);
		bwrite(blFisico,buffer);

		//escribimos bloques intermedios
		//printf("bo intermedios:%d\n", bloques_intermedios);
		for (int i = 0; i < bloques_intermedios; ++i){
			blFisico = traducir_bloque_inodo(ninodo, priBlogico + i +1, 1);
			bwrite (blFisico, buf_original + (BLOCKSIZE - desp) + (i * BLOCKSIZE));			
		}
		//escribit ultimo bloque
		//printf("var: %d\n", ultBlogico);
		
		blFisico = traducir_bloque_inodo(ninodo, ultBlogico, 1);
		bread(blFisico,buffer);
		memcpy(buffer, buf_original + (BLOCKSIZE - desp) + (BLOCKSIZE * bloques_intermedios), nbytes - (BLOCKSIZE-desp) - (BLOCKSIZE * bloques_intermedios) );
		//printf("bl fisico: %d\n", blFisico);
		bwrite(blFisico, buffer);
		
	}
	nodo = leer_inodo(ninodo);
	if (nodo.tamEnBytesLog < (nbytes + offset)){
		nodo.tamEnBytesLog = (nbytes + offset);
		escribir_inodo(nodo,ninodo);
	}


	return(nbytes);//retornar bytes escritos
}

int leer_bloque(int nbloque){

	unsigned char buffer[BLOCKSIZE];
	bread(nbloque,&buffer);
	printf("\n..................................................BLOCK[%d]....................................................\n", nbloque);
	for (int i = 0; i <= BLOCKSIZE; ++i){
		printf("%c", buffer[i]);
	}
	printf("\n...............................................................................................................\n");
	return(0);

}

int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes){

	struct inodo nodo = leer_inodo(ninodo);
	int priBlogico = offset/BLOCKSIZE;
	int ultBlogico = (offset+nbytes-1)/BLOCKSIZE;
	unsigned char buffer[BLOCKSIZE];
	int desp = offset%BLOCKSIZE;//desplazamiento escritura en el primer bloque
	int blFisico;
	int bloques_intermedios = ultBlogico - priBlogico - 1; //le quitamos el primero y el ultimo

	if ((nodo.permisos & 4) != 4){
		printf("ERROR: el nodo no tiene permisos de escritura\n");
		return(-1);//el inodo no tiene permiso de lectura
	}

	if ((offset+nbytes) > nodo.tamEnBytesLog){
		printf("ERROR: se intenta leer mas alla de final de fichero\n");
		return(0); //se pretende leer mas alla de final de fichero
	}


/*
	Primeramente trataremos el caso en que el primer y último bloque coincidan, 
	y por tanto el buffer que vayamos a escribir cabe en un solo bloque.
*/
	if (priBlogico == ultBlogico){
		//printf("blogico: %d\n", priBlogico);
		blFisico = traducir_bloque_inodo(ninodo, priBlogico, 0);
		//printf("bloquefisico: %d\n", blFisico);
		if (blFisico <= 0){
			return(-1);
		}
		bread(blFisico,buffer);
		memcpy(buf_original,buffer + desp,nbytes);
		return(nbytes);
	}
/*
	A continuación trataremos el caso en que la operación afecte a más de un bloque.
*/
	else if (priBlogico < ultBlogico){
		//escribimos primer bloque
		//printf("blogico: %d\n", priBlogico);
		blFisico = traducir_bloque_inodo(ninodo, priBlogico, 0);
		//printf("bloquefisico: %d\n", blFisico);
		if (blFisico > 0){
			bread(blFisico,buffer);
			memcpy(buf_original,buffer+desp,BLOCKSIZE-desp); //leemos primer bloque
		}
		
		//escribimos bloques intermedios
		//printf("bo intermedios:%d\n", bloques_intermedios);
		for (int i = 0; i < bloques_intermedios; ++i){
			blFisico = traducir_bloque_inodo(ninodo, priBlogico + i+1, 0);
			if (blFisico > 0){
				bread(blFisico, buf_original + (BLOCKSIZE - desp) + (i * BLOCKSIZE));
			}			
		}
		//escribit ultimo bloque		
		blFisico = traducir_bloque_inodo(ninodo, ultBlogico, 0);
		if (blFisico > 0){
			bread(blFisico,buffer);
			memcpy(buf_original + (BLOCKSIZE - desp) + (BLOCKSIZE * bloques_intermedios),buffer,nbytes - (BLOCKSIZE-desp) - (BLOCKSIZE * bloques_intermedios) );		
		}
	}

	return(nbytes);//retornar bytes escritos
}

int mi_chmod_f(unsigned int ninodo, unsigned char permisos){
	struct inodo nodo = leer_inodo(ninodo);
	nodo.permisos = permisos;
	nodo.ctime = time(NULL);
	escribir_inodo(nodo, ninodo);
	return(0);
}

int mi_truncar_f(unsigned int ninodo, unsigned int nbytes){

	int bloque = (nbytes / BLOCKSIZE);
	if ((nbytes % BLOCKSIZE) != 0){
		bloque++;
	}
	int salida = liberar_bloques_inodo(ninodo, bloque);
	struct inodo nodo = leer_inodo(ninodo);
	nodo.tamEnBytesLog = nbytes;
	escribir_inodo(nodo,nodo.cardinal);
	return(salida);
}

int mi_stat_f(unsigned int ninodo, struct STAT *p_stat){

	struct inodo nodo = leer_inodo(ninodo);
	struct STAT stat;
	stat.tipo = nodo.tipo;
	stat.permisos = nodo.permisos;
	stat.atime = nodo.atime;
	stat.mtime = nodo.mtime;
	stat.ctime = nodo.ctime;
	stat.cardinal = nodo.cardinal;
	stat.nlinks = nodo.nlinks;
	stat.tamEnBytesLog = nodo.tamEnBytesLog;
	stat.numBloquesOcupados = nodo.numBloquesOcupados;

	*p_stat = stat;

	return(0);
}

int leer_bloque_entradas(int nbloque){

	struct entrada{
		char nombre[60]; //En el sistema de ficheros ext2 la longitud del nombre es 255
		unsigned int inodo;
	};

	struct entrada buffer[16];
	bread(nbloque,&buffer);
	printf("\n..................................................BLOCK[%d]....................................................\n", nbloque);
	for (int i = 0; i < 16; i++){
		printf("(%s: ", buffer[i].nombre);
		printf("%d)   ", buffer[i].inodo);
	}
	printf("\n...............................................................................................................\n");
	return(0);
}