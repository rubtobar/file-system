#include "ficheros_basico.h"

/*
 * Funciones para calcular el tamaño en bloques.
 */
int tamMB(unsigned int nbloques){

	int tamMB;

	if ((nbloques % (BLOCKSIZE*8)) == 0) {
		tamMB = nbloques/(BLOCKSIZE*8);
		if (tamMB == 0)	{
			tamMB++;
		}
	}else {
		tamMB = nbloques/(BLOCKSIZE*8);
		tamMB++;
	}
	return (tamMB);
}

int tamAI(unsigned int ninodos){

	int tamAI;
	if (ninodos % (BLOCKSIZE / T_INODO) == 0) {
		tamAI = ninodos/(BLOCKSIZE / T_INODO);
		if (tamAI == 0)
		{
			tamAI++;
		}
	}else {
		tamAI = ninodos / (BLOCKSIZE / T_INODO);
		tamAI++;
	}
	return(tamAI);
}

/*
 * Funciones para inicializar el sistema de ficheros.
 */
int initSB(unsigned int nbloques, unsigned int ninodos){
	
	int MBtam = tamMB(nbloques);
	int AItam = tamAI(ninodos);

	struct superbloque SB;
	// Rellenamos campos del SB
	SB.posPrimerBloqueMB = posSB + tamSB;
	SB.posUltimoBloqueMB = SB.posPrimerBloqueMB + MBtam-1;
	SB.posPrimerBloqueAI = SB.posUltimoBloqueMB + 1;
	SB.posUltimoBloqueAI = SB.posPrimerBloqueAI + AItam-1;
	SB.posPrimerBloqueDatos = SB.posUltimoBloqueAI + 1;
	SB.posUltimoBloqueDatos = nbloques -1;
	SB.posInodoRaiz = 0;
	SB.posPrimerInodoLibre = 0;
	SB.cantBloquesLibres = nbloques;
	SB.cantInodosLibres = ninodos;
	SB.totBloques = nbloques;
	SB.totInodos = ninodos;

	memset(&SB.padding,0,sizeof(SB.padding)); //llenamos a 0 los datos del padding
	if(DEBUG_FICHERO == 1){
		printf("SuperBloque inicializacion completada\n");//comprovamos tamaño SB
	}
	return(bwrite(posSB , &SB)); //escribimos SB en fichero
}

int initMB(){

	struct superbloque SB;
	unsigned char *buffer[BLOCKSIZE];

	bread(posSB, &SB);
	memset(buffer, 0, BLOCKSIZE);

	for (int i = SB.posPrimerBloqueMB; i <= SB.posUltimoBloqueMB; ++i)
	{
		bwrite(i, buffer);
	}
	
	//inicializamos como ocupados los bits de la cabecera
	//Superbloque
	escribir_bit(posSB, 1);
	SB.cantBloquesLibres--;
	//mapa de bits
	for (int i = SB.posPrimerBloqueMB; i <= SB.posUltimoBloqueMB; i++){
		escribir_bit(i, 1);
		SB.cantBloquesLibres--;
	}
	//array de inodos
	for (int i = SB.posPrimerBloqueAI; i <= SB.posUltimoBloqueAI; i++){
		escribir_bit(i, 1);
		SB.cantBloquesLibres--;
	}

	bwrite(posSB, &SB);

	return 0;
}

int initAI(){
	struct superbloque SB;
	struct inodo nodo;
	struct inodo buffer[8];
	int posAI;
	int posbuffer = 0;
	bread(0, &SB);
	posAI = SB.posPrimerBloqueAI;

	nodo.tipo = 'l';
	nodo.permisos = 0;
	memset(&nodo.reservado_alineacion1,0,sizeof(nodo.reservado_alineacion1));
	nodo.atime = time(NULL);
	nodo.mtime = time(NULL);
	nodo.ctime = time(NULL);
	nodo.nlinks = 0;
	nodo.tamEnBytesLog = 0;
	nodo.numBloquesOcupados = 0;
	memset(&nodo.punterosDirectos, 0,sizeof(nodo.punterosDirectos));
	memset(&nodo.punterosIndirectos, 0,sizeof(nodo.punterosIndirectos));
	memset(&nodo.padding,0,sizeof(nodo.padding));
	memset(&buffer,0,sizeof(buffer));
	
	for (int i = 0; i < SB.totInodos; i++){

		nodo.cardinal = i;
		nodo.punterosDirectos[0] = i+1;
		if (i == SB.totInodos-1){
			nodo.punterosDirectos[0] = UINT_MAX;
		}

		buffer[posbuffer] = nodo;
		posbuffer++;

		if(posbuffer == 8){
			posbuffer = 0;
			bwrite(posAI, &buffer);
			memset(&buffer,0,BLOCKSIZE);
			posAI++;
		}
		
	}

	if (SB.totInodos%8 != 0){
		bwrite(posAI, &buffer);
	}
	return (0);
}

int leerSB() {

	struct superbloque SB;
	
	bread(0, &SB);
	
	printf("\n/####################################################/\n");
	printf(" Datos del SuperBloque:\n\n");
	printf("	Pos. 1er bloque del Mapa de Bits:	%d\n", SB.posPrimerBloqueMB);
	printf("	Pos. Ult. bloque del Mapa deBits:	%d\n", SB.posUltimoBloqueMB);
	printf("\n");
	printf("	Pos. 1er bloque del Array de Inodos:	%d\n", SB.posPrimerBloqueAI);
	printf("	Pos. Ult. bloque del Array de Inodos:	%d\n", SB.posUltimoBloqueAI);
	printf("\n");
	printf("	Pos. 1er bloque de datos:	%d\n", SB.posPrimerBloqueDatos);
	printf("	Pos. Ult. bloque de datos:	%d\n\n", SB.posUltimoBloqueDatos);
	printf("\n");
	printf("	Pos. Inodo raiz: 	%d\n", SB.posInodoRaiz);
	printf("	Pos. Inodo libre: 	%d\n\n", SB.posPrimerInodoLibre);
	printf("\n");
	printf("	Num. de bloques libres:	%d\n", SB.cantBloquesLibres);
	printf("	Num. de inodos libres:	%d\n\n", SB.cantInodosLibres);
	printf("\n");
	printf("	Tot. bloques:	%d\n", SB.totBloques);
	printf("	Tot. inodos:	%d\n\n", SB.totInodos);
	printf("/####################################################/\n");
	
	return 0;
}

int escribir_bit(unsigned int nbloque, unsigned int bit){

	struct superbloque SB;
	bread(posSB, &SB);

	unsigned int posByte = nbloque / 8;
	unsigned int posBit = nbloque % 8;
	unsigned int posBlock = posByte / BLOCKSIZE + SB.posPrimerBloqueMB;

	posByte = posByte % BLOCKSIZE;

	unsigned char buffer[BLOCKSIZE];
	bread(posBlock, &buffer);

	unsigned char mascara = 128;	

	mascara >>= posBit; // ponemos el bit de la mascarara sobre en bit a modificar

	if (bit == 1){
		buffer[posByte] |= mascara;
	}else if (bit == 0){
		buffer[posByte] &= ~mascara;
	}else{
		printf("ERROR: valor de bit incorrecto %d\n", bit);
	}
	bwrite(posBlock, &buffer);

	return (0);
}

unsigned char leer_bit(unsigned int nbloque){

	struct superbloque SB;
	bread(posSB, &SB);
	unsigned char bitLeido;

	unsigned int posByte = nbloque / 8;
	unsigned int posBit = nbloque % 8;
	unsigned int posBlock = posByte / BLOCKSIZE + SB.posPrimerBloqueMB;

	unsigned char buffer[BLOCKSIZE];
	bread(posBlock, &buffer);

	posByte = posByte % BLOCKSIZE;

	unsigned char mascara = 128;	

	if(posBit > 0){
	mascara >>= posBit;
	}

	mascara &= buffer[posByte];

	if (mascara == 0){
		bitLeido = 0; 
	}
	else{
		bitLeido = 1;
	}

 return (bitLeido);

}

int leerMB(){

	struct superbloque SB;
	bread(posSB, &SB);

	int cont = 0;

	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Mapa de Bits~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	for (int i = 0; i < SB.totBloques; ++i)
	{
		//printf("%u", leer_bit(i));
		if (leer_bit(i) == 1){
			printf("%d,", i);
		}

		if (leer_bit(i))
		{
			cont++;
		}
	}
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Fin Mapa de Bits~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	return (0);
}


int reservar_bloque(){

	struct superbloque SB;
	bread(posSB, &SB);
	unsigned char buffer[BLOCKSIZE];
	unsigned char bufferAux[BLOCKSIZE];
	unsigned char byteAux = 255;
	memset(&bufferAux,255,BLOCKSIZE);
	memset(&buffer, 0, BLOCKSIZE);
	int bloque = 0; //bloque del MB en el que se encuentra el bit a 1;
	int byte = 0;
	int bit = 0;
	unsigned char mascara = 128;
	unsigned char byteRes;
	unsigned char bufferRellenar[BLOCKSIZE];
	memset(&bufferRellenar,0,BLOCKSIZE);

	if(SB.cantBloquesLibres == 0){
		printf("ERROR: no quedan bloques libres(SuperBloque)\n");
		return(-1);
	}
	for (int i = SB.posPrimerBloqueMB; i <= SB.posUltimoBloqueMB; ++i){
		bread(i,&buffer);
		if (memcmp(&buffer,&bufferAux,BLOCKSIZE) != 0){
			bloque = i;
			break;
		}
	}
	
	bloque = bloque - SB.posPrimerBloqueMB;
	//printf("bloque:%d\n", bloque);

	for (int i = 0; i <= BLOCKSIZE; i++){
		if (memcmp(&buffer[i],&byteAux,sizeof(unsigned char)) != 0){
			byte = i;
			break;
		}
	}
	//printf("byte%d\n", byte);
	if (buffer[byte] < 255) { // hay bits a 0 en el byte
		byteRes = buffer[byte] & mascara;
		while (byteRes != 0) { // operador AND para bits
			mascara >>= 1; // desplazamiento de bits a la derecha
			byteRes = buffer[byte] & mascara;
			bit++;
		}

	}else{
		printf("ERROR: reservar bloque\n");
	}
	//printf("bit%d\n", bit);

	escribir_bit(bloque*BLOCKSIZE*8+byte*8+bit,1);

	//actualizamos la cantidad de bloques libres en es SB
	SB.cantBloquesLibres--;

	bwrite(posSB,&SB);

	//bwrite(bloque*BLOCKSIZE+byte*8+bit,&bufferRellenar);

	return(bloque*BLOCKSIZE*8+byte*8+bit);

}

int liberar_bloque(unsigned int nbloque){
	struct superbloque SB;
	bread(posSB, &SB);
	escribir_bit(nbloque, 0);
	//rellenamos el bloque con 0s
	unsigned char bufferRellenar[BLOCKSIZE];
	memset(&bufferRellenar,0,BLOCKSIZE);
	bwrite(nbloque,&bufferRellenar);
	//actualizamos la cantidad de bloques libres en el SB
	SB.cantBloquesLibres++;
	bwrite(posSB,&SB);
	//printf("bloque liberado %d\n", nbloque);
	return(nbloque);
}

int escribir_inodo(struct inodo inodo, unsigned int ninodo){

	struct superbloque SB;
	bread(posSB, &SB);
	int blockInodo = ninodo / (BLOCKSIZE/T_INODO);
	int posInodo = ninodo % (BLOCKSIZE/T_INODO);
	struct inodo buffer[BLOCKSIZE/T_INODO];
	bread(SB.posPrimerBloqueAI + blockInodo, &buffer);
	buffer[posInodo] = inodo;
	bwrite(SB.posPrimerBloqueAI + blockInodo, &buffer);

	return(0);
}

struct inodo leer_inodo(unsigned int ninodo){

	struct superbloque SB;
	bread(posSB, &SB);
	int blockInodo = ninodo / (BLOCKSIZE/T_INODO);
	int posInodo = ninodo % (BLOCKSIZE/T_INODO);
	struct inodo buffer[BLOCKSIZE/T_INODO];
	bread(SB.posPrimerBloqueAI + blockInodo, &buffer);

	return(buffer[posInodo]);
}

int reservar_inodo(unsigned char tipo, unsigned char permisos){

	struct superbloque SB;
	bread(posSB, &SB);

	if (SB.cantInodosLibres <= 0){
		printf("ERROR: No quedan inodos libres\n");
		return(-1);
	}

	struct inodo nodoLeido;
	struct inodo nodoNuevo;
	nodoNuevo.tipo = tipo;
	nodoNuevo.permisos = permisos;
	memset(&nodoNuevo.reservado_alineacion1,0,sizeof(nodoNuevo.reservado_alineacion1));
	nodoNuevo.atime = time(NULL);
	nodoNuevo.mtime = time(NULL);
	nodoNuevo.ctime = time(NULL);
	nodoNuevo.nlinks = 1;
	nodoNuevo.tamEnBytesLog = 0;
	nodoNuevo.numBloquesOcupados = 0;
	memset(&nodoNuevo.punterosDirectos, 0,sizeof(nodoNuevo.punterosDirectos));
	memset(&nodoNuevo.punterosIndirectos, 0,sizeof(nodoNuevo.punterosIndirectos));
	memset(&nodoNuevo.padding,0,sizeof(nodoNuevo.padding));

	nodoLeido = leer_inodo(SB.posPrimerInodoLibre);
	int num_inodo = SB.posPrimerInodoLibre;
	nodoNuevo.cardinal = nodoLeido.cardinal;
	escribir_inodo(nodoNuevo, SB.posPrimerInodoLibre);
	SB.posPrimerInodoLibre = nodoLeido.punterosDirectos[0];
	SB.cantInodosLibres--;
	bwrite(posSB,&SB);

	return(num_inodo);
}

int leerAI(int numero){
	struct superbloque SB;
	bread(posSB, &SB);

	struct tm *ts;
	char atime[80];
	char mtime[80];
	char ctime[80];
	struct inodo inodo;
	//int ninodo = 0;
	int count = SB.totInodos;
/*
	if (numero != -1){
		count = numero+1;
		ninodo = numero;
	}
*/
	printf("\n***********************************************************Inodos**********************************************************\n");
	
	for (int i = 0;i<=count;i++) {
		inodo = leer_inodo(i);
		if (inodo.tipo == 'l')
		{
			break;
		}
		ts = localtime(&inodo.atime);
		strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
		ts = localtime(&inodo.mtime);
		strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
		ts = localtime(&inodo.ctime);
		strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);
		printf("ID: %d TYPE: %c ACCESSTIME: %s MODIFYTIME: %s CHANGETTIME: %s\n",inodo.cardinal,inodo.tipo,atime,mtime,ctime);
		printf("Bytes logicos: %u Bloques fisicos %u\n",inodo.tamEnBytesLog, inodo.numBloquesOcupados);
		printf("Directos:	");
		for (int i = 0; i < 12; ++i)
		{
			printf("[%d]", inodo.punterosDirectos[i]);
		}
		printf("\nIndirecto 0:	[%d]\n",inodo.punterosIndirectos[0]);
		printf("Indirecto 1:	[%d]\n",inodo.punterosIndirectos[1]);
		printf("Indirecto 2:	[%d]\n",inodo.punterosIndirectos[2]);
		printf("\n");
	}
	printf("******************************************************Fin array Inodos*****************************************************\n");
	return(0);
}

int obtener_nrangoBL(struct inodo inodo, int nblogico, int *ptr){

   int npunteros, directos, indirectos0, indirectos1, indirectos2;
   npunteros = BLOCKSIZE/sizeof(int);   //256
   directos = 12;
   indirectos0 = npunteros + directos;                          //268
   indirectos1 = npunteros * npunteros + indirectos0;             //65.804
   indirectos2 = npunteros * npunteros * npunteros + indirectos1;   //16.843.020

   if (nblogico < directos){
   	  
         *ptr = inodo.punterosDirectos[nblogico];
         return(0);
   }
   else{
      if (nblogico < indirectos0) {
         *ptr = inodo.punterosIndirectos[0];
         return(1);

      }
      else{
         if (nblogico < indirectos1){
            *ptr = inodo.punterosIndirectos[1]; 
            return(2);
         }
         else{
            if (nblogico < indirectos2){
               *ptr = inodo.punterosIndirectos[2]; 
               return(3);
            }
            else{
               *ptr = 0;
               printf("ERROR: No existe el bloque\n");
               return(-1);
            }
         }
      }
   }
}

int obtener_indice (int nblogico, int nivel_punteros){
	int npunteros, directos, indirectos0, indirectos1, indirectos2, ind;
	ind = -1;
	npunteros = BLOCKSIZE/sizeof(int);	//256
	directos=12;
	indirectos0 = npunteros + directos;	//268
	indirectos1 = npunteros * npunteros + indirectos0;	//65.804
	indirectos2 = npunteros * npunteros * npunteros + indirectos1;	//16.843.020
	//caso en que son directos
	if (nblogico<directos){
		ind = nblogico;
	}
	else{
		//caso en que son indirectos 1
		if (nblogico<indirectos0){
			ind=nblogico-directos;
		}
		else{
			//caso en que son indirectos 2
			if (nblogico<indirectos1){
				if (nivel_punteros==1){ind=(nblogico-indirectos0)/npunteros;}
				else{
					if (nivel_punteros==0){ind=(nblogico-indirectos0)%npunteros;}
				}
			//caso en que son indirectos 3
			}else{
				if (nblogico<indirectos2){
					if (nivel_punteros==2){ind=(nblogico-indirectos1)/(npunteros*npunteros);}
					else{
						if (nivel_punteros==1){ind=((nblogico-indirectos1)%(npunteros*npunteros))/npunteros;}
						else{
							if (nivel_punteros==0){ind=((nblogico-indirectos1)%(npunteros*npunteros))%npunteros;}
						}
					}
				}
			}
		}
	}
	return(ind);
}

int traducir_bloque_inodo(unsigned int ninodo, unsigned int nblogico, char reservar){

	struct superbloque SB;
	bread(posSB, &SB);
	struct inodo nodo = leer_inodo(ninodo);
	int *ptr, *ptr_ant, rango, indice = 0, bloque_fisico;
	ptr = malloc(sizeof(int));
	ptr_ant = malloc(sizeof(int));
	int buffer[256];
	unsigned char voidAux[BLOCKSIZE];
	memset(&voidAux,0,BLOCKSIZE);
	int nuevo_bloque;

	//obtenemos el nivel de punteros
	rango = obtener_nrangoBL(nodo, nblogico, ptr);
	//casos en que estamos entre el nblogico {0 - 12}
	if (rango == 0){
		if (reservar == 0){
			//si estamos en modo lectura unicamente retornamos error
			if (*ptr != 0){
				//retornamos el puntero directo que indica nblogico
				free(ptr);
				free(ptr_ant);
				return(nodo.punterosDirectos[nblogico]);
			}else{
				//retornamos error en caso que estamos leyendo y no exista
				//printf("ERROR: puntero a datos inexistente(f1)\n");
				free(ptr);
				free(ptr_ant);
				return(-1);
			}

		}else if (reservar == 1){
			
			if (*ptr != 0){
				//retornamos el puntero directo que indica nblogico
				free(ptr);
				free(ptr_ant);
				return(nodo.punterosDirectos[nblogico]);
			}
			//reservamos un nuevo bloque y se lo asignamos al puntero
			nuevo_bloque = reservar_bloque();
			//printf("bloque reservado: %d\n", nuevo_bloque);
			//consideramos que los 12 primeros bloques tienen un puntero cada uno
			nodo.punterosDirectos[nblogico] = nuevo_bloque;
			nodo.ctime = time(NULL);
			nodo.numBloquesOcupados++;
			//*****************************nodo.tamEnBytesLog = nodo.tamEnBytesLog + BLOCKSIZE;
			escribir_inodo(nodo, ninodo);
			free(ptr);
			free(ptr_ant);
			return(nuevo_bloque);
		}
	}

	//casos en que el rango es mayor que uno {<12}
	int nivelaux = rango-1;
	//int nuevo_bloque;
	if (rango > 0){
		while (rango >= 0){
			//comprovamos que existan los bloques intermedios
			if (*ptr == 0){
				if (reservar == 0){
					//printf("ERROR: puntero a datos inexistente(f2)\n");
					free(ptr);
					free(ptr_ant);
					return(-1);
				}else if (reservar == 1){
					//creamos la ruta hacia el nuevo bloque de datos
					nuevo_bloque = reservar_bloque();
					//printf("nuevo_bloque: %d\n", nuevo_bloque);
					//actualizamos los bloques ocupados(bloques de punt cuentan)
					nodo.numBloquesOcupados++;
					if (nodo.punterosIndirectos[nivelaux] == 0){
						nodo.punterosIndirectos[nivelaux] = nuevo_bloque;
						nodo.ctime = time(NULL);
						escribir_inodo(nodo, ninodo);
					}else{
						buffer[indice] = nuevo_bloque;
						bwrite(*ptr_ant, &buffer);
						//actualizamos el tamaño en bytes logicos del nodo (los de punteros no los contamos)
						if (rango == 0){
							//*****************************nodo.tamEnBytesLog = nodo.tamEnBytesLog + BLOCKSIZE;
							escribir_inodo(nodo, ninodo);
						}
						
					}
					*ptr = nuevo_bloque;
				}
			}
			//printf("nblogico: %d rango: %d\n",nblogico,rango-1);
			indice = obtener_indice(nblogico, rango-1);
			//printf("indice: %d\n", indice);
			*ptr_ant = *ptr;
			bread(*ptr, &buffer);
			*ptr = buffer[indice];
			rango--;
		}
		//liberamos los punteros
		bloque_fisico = *ptr_ant;
		free(ptr);
		free(ptr_ant);
		return(bloque_fisico);
	}
	//algo ha ido mal
	free(ptr);
	free(ptr_ant);
	return(-1);
}

int imprimir_bloque(int nbloque){
	int buffer[256];
	bread(nbloque,&buffer);
	printf("\n..................................................BLOCK[%d]....................................................\n", nbloque);
	for (int i = 0; i < 256; ++i){
		printf("%d,", buffer[i]);
	}
	printf("\n...............................................................................................................\n");
	return(0);
}

int liberar_bloques_inodo(unsigned int ninodo, unsigned int nblogico){

	struct inodo nodo = leer_inodo(ninodo);
	int npunteros = BLOCKSIZE/sizeof(int);	//256
	int buffer[npunteros];
	int buffer1[npunteros];
	int buffer2[npunteros];
	int bloques_liberados = 0;
	int bloques_datos_liberados = 0;

	//liberamos los bloques a los que apuntan los punteros directos del inodo
	for (int i = 0; i < 12; ++i){
		if (nodo.punterosDirectos[i] != 0){

			if (nblogico <= i){
				liberar_bloque(nodo.punterosDirectos[i]);
				bloques_liberados++;
				bloques_datos_liberados++;
				nodo.punterosDirectos[i] = 0;
			}
			
		}
	}
	//------------------------------------------------------------------------------------------------------------
	//liberamos los punteros indirectos 1;
	if (nodo.punterosIndirectos[0] != 0){
		bread(nodo.punterosIndirectos[0],&buffer);
		for (int i = 0; i < npunteros; ++i){
			if (buffer[i] != 0){

				if (nblogico <= i+12){
					liberar_bloque(buffer[i]);//liberamos cada bloqe apuntado por el bloque de punteros
					buffer[i] = 0;
					bwrite(nodo.punterosIndirectos[0],&buffer);
					bloques_liberados++;
					bloques_datos_liberados++;
				}
				
			}
		}
		//liberamos bloque de punteros
		if (nblogico < 12){
			liberar_bloque(nodo.punterosIndirectos[0]);
			bloques_liberados++;
			nodo.punterosIndirectos[0] = 0;
		}
		
	}
	//------------------------------------------------------------------------------------------------------------
	//liberamos los punteros indirectos 2
	if (nodo.punterosIndirectos[1] != 0){
		bread(nodo.punterosIndirectos[1],&buffer);
		for (int i = 0; i < npunteros; ++i){
			//primer bloque punteros
			if (buffer[i] != 0){
				bread(buffer[i],&buffer1);
				for (int j = 0; j < npunteros; ++j){
					if (buffer1[j] != 0){

						if (nblogico <= i*256+j+256+12){
							liberar_bloque(buffer1[j]);
							buffer1[j] = 0;
							bwrite(buffer[i], &buffer1);
							bloques_liberados++;
							bloques_datos_liberados++;
						}
						
					}
				}
			}
		}

		for (int i = 0; i < npunteros; ++i){
			//primer bloque punteros
			if (buffer[i] != 0){

				if (nblogico < 256*i+256+12){
					liberar_bloque(buffer[i]);
					buffer[i] = 0;
					bwrite(nodo.punterosIndirectos[1],&buffer);
					bloques_liberados++;
				}

			}
		}

		//liberamos bloque de punteros
		if (nblogico < 256+12){
			liberar_bloque(nodo.punterosIndirectos[1]);
			bloques_liberados++;
			bwrite(nodo.punterosIndirectos[1],&buffer);
			nodo.punterosIndirectos[1] = 0;
		}
		
	}
	//------------------------------------------------------------------------------------------------------------
	//liberamos los punteros indirectos 3
	//libramos todos los bloques de datos
	if (nodo.punterosIndirectos[2] != 0){
		bread(nodo.punterosIndirectos[2],&buffer);
		for (int i = 0; i < npunteros; ++i){
			//primer bloque punteros
			if (buffer[i] != 0){
				bread(buffer[i],&buffer1);
				//segundo bloque de punteros
				for (int j = 0; j < npunteros; ++j){
					if (buffer1[j] != 0){
						bread(buffer1[j],&buffer2);
						//tercer bloque de punteros
						for (int k = 0; k < npunteros; ++k){
							if (buffer2[k] != 0){
								//bloques de datos

								if (nblogico <= i*256+j*256+k+65804){
									liberar_bloque(buffer2[k]);
									buffer2[k] = 0;
									bwrite(buffer1[j], &buffer2);
									bloques_liberados++;
									bloques_datos_liberados++;
								}

							}
						}	
					}
				}		
			}
		}
		//liberamos los ultimos bloques de punteros
		for (int i = 0; i < npunteros; ++i){
			//primer bloque punteros
			if (buffer[i] != 0){
				bread(buffer[i],&buffer1);
				//segundo bloqued de punteros
				for (int j = 0; j < npunteros; ++j){
					if (buffer1[j] != 0){

						if (nblogico <= i*256+j*256+65804){
							liberar_bloque(buffer1[j]);
							buffer1[j] = 0;
							bwrite(buffer[i],&buffer1);
							bloques_liberados++;
						}
							
					}
				}		
			}
		}
		//liberamos los bloques de punteros de enmedio
		for (int i = 0; i < npunteros; ++i){
			//primer bloque punteros
			if (buffer[i] != 0){

				if (nblogico <= i*256+65804){
					liberar_bloque(buffer[i]);
					buffer[i] = 0;
					bwrite(nodo.punterosIndirectos[2],&buffer);
					bloques_liberados++;
				}

			}
		}
		if (nblogico <= 65804){
			liberar_bloque(nodo.punterosIndirectos[2]);
			bloques_liberados++;
			nodo.punterosIndirectos[2] = 0;
		}
	}
	nodo.tamEnBytesLog = nodo.tamEnBytesLog - bloques_datos_liberados*BLOCKSIZE;
	nodo.numBloquesOcupados = nodo.numBloquesOcupados - bloques_liberados;
	nodo.ctime = time(NULL);
	nodo.atime = time(NULL);
	escribir_inodo(nodo, ninodo);
	
	return(bloques_liberados);
}

int liberar_inodo(unsigned int ninodo){

	struct superbloque SB;
	bread(posSB, &SB);
	struct inodo nodo = leer_inodo(ninodo);

	//liberamos todos los bloques del inodo
	liberar_bloques_inodo(ninodo,0);
	//asignamos los punteros del SB y del inodo
	nodo.punterosDirectos[0] = SB.posPrimerInodoLibre;
	SB.posPrimerInodoLibre = ninodo;
	nodo.tipo = 'l';
	nodo.ctime = time(NULL);
	escribir_inodo(nodo, ninodo);
	bwrite(posSB,&SB);

	return(ninodo);
}