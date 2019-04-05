#! /bin/bash 
# $1 --> disco.imagen

make
clear
#Montamos el disco 
echo --------------------- Montamos el disco ---------------------
./mi_mkfs $1 100000
echo
echo --------------------- Mostramos los datos del sistema ---------------------
./leer_sf $1
echo
#Crear un directorio
echo --------------------- Creamos un directorio: /directorio/ ---------------------
./mi_mkdir $1 6 /directorio/
echo

echo Pulsa ENTER para continuar
read
#Listamos la carpeta raiz
echo --------------------- Mostramos los archivos/carpetas de la raiz ---------------------
./mi_ls $1 /
echo

echo Pulsa ENTER para continuar
read
#Crear un fichero
echo --------------------- Creamos un fichero dentro del directorio: /directorio/fichero.txt ---------------------
./mi_mkdir $1 6 /directorio/fichero.txt
echo

echo Pulsa ENTER para continuar
read
#Listamos la carpeta directorio
echo --------------------- Mostramos los archivos/carpetas dentro de la directorio: /directorio/ ---------------------
./mi_ls $1 /directorio/
echo

echo Pulsa ENTER para continuar
read
#Ecribimos en el fichero
echo --------------------- Escribimos en el fichero.txt ---------------------
./mi_escribir $1 /directorio/fichero.txt 0 holasoyuntexto
echo

echo Pulsa ENTER para continuar
read
#Leemos del fichero y direccionamos
echo --------------------- Leemos desde el fichero direccionando la salida a ficheroext.txt ---------------------
./mi_cat $1 /directorio/fichero.txt > ficheroext.txt
echo

echo Pulsa ENTER para continuar
read
#Creamos un directorio para llenarlo con 17 directorios
echo --------------------- Creamos un directorio para que contenga 17 directorios ---------------------
./mi_mkdir $1 6 /directorio17/
echo

echo Pulsa ENTER para continuar
read
echo --------------------- Creamos los 17 directorios ---------------------
limite=16
i=0;
while [[ $limite -ge $i ]]
do
	./mi_mkdir $1 6 /directorio17/D$i/
     let i=$i+1
done
echo

echo Pulsa ENTER para continuar
read
#Listamos la carpeta directorio
echo --------------------- Mostramos los archivos/directorios de directorio17 ---------------------
./mi_ls $1 /directorio17/
echo

echo Pulsa ENTER para continuar
read
#Mi_stat de directorio17
echo --------------------- Mostramos el STAT del directorio directorio17 ---------------------
./mi_stat $1 /directorio17/
echo
echo Vemos que ocupa 2 Bloques
echo

echo Pulsa ENTER para continuar
read
#Mi_rm de D0
echo --------------------- Borramos el directorio D0 ---------------------
./mi_rm $1 /directorio17/D0
echo

echo Pulsa ENTER para continuar
read
#Mi_stat de directorio17
echo --------------------- Mostramos el STAT del directorio directorio17 ---------------------
./mi_stat $1 /directorio17/
echo

echo Ahora ocupa solo 1 bloque y tiene permisos = 6
echo
echo --------------------- Mostramos la lista de directorios/archivos del directorio directorio17 ---------------------
./mi_ls $1 /directorio17/
echo

echo Ahora ocupa solo 1 bloque y tiene permisos = 6
echo

echo Pulsa ENTER para continuar
read
#Mi_chmod de Directorio17
echo --------------------- Cambiamos permisos del directorio7 de 6 a 2 ---------------------
./mi_chmod $1 2 /directorio17/
echo

echo Pulsa ENTER para continuar
read
#Listamos la carpeta raiz
echo --------------------- Mostramos el contenido de la raiz ---------------------
./mi_ls $1 /
echo
echo Ahora vemos como tiene permisos 2
echo

echo Pulsa ENTER para continuar
read
echo --------------------- Enlazamos a fichero.txt en directorio ficheroln.txt en raiz ---------------------
./mi_ln $1 /directorio/fichero.txt /ficheroln.txt

echo Pulsa ENTER para continuar
read
echo --------------------- Mostramos el STAT de fichero.txt ---------------------
./mi_stat $1 /directorio/fichero.txt
echo
echo Vemos como ahora tiene 2 links
echo

echo Pulsa ENTER para continuar
read
echo --------------------- Mostramos el STAT de ficheroln.txt ---------------------
./mi_stat $1 /ficheroln.txt
echo 

echo Vemos que es igual que el anterior
echo
echo Pulsa ENTER para continuar
read
echo --------------------- Borramos fichero.txt ---------------------
./mi_rm $1 /directorio/fichero.txt
echo

echo Pulsa ENTER para continuar
read
echo --------------------- Mostramos el STAT ficheroln.txt ---------------------
./mi_stat $1 /ficheroln.txt
echo
echo Vemos como ahora el número de links es 1
