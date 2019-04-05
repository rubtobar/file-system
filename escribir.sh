#script
#!/bin/bash
# $1 --> Disco virtual

#creamos un fichero en el directorio raiz
echo
echo MI_MKDIR -------------------------------------------
echo creamos un fichero en la raiz
echo
./mi_mkdir $1 7 /fichero.txt
#intentamos crear un fichero dentro de otro
echo
echo MI_MKDIR -------------------------------------------
echo intentamos crear un fichero dentro de otro
echo
./mi_mkdir $1 7 /fichero.txt/fichero2.txt
#listamos la raiz para ver el fichero
echo
echo MI_LS -------------------------------------------
echo listamos la raiz para ver el fichero creado
echo
./mi_ls $1 /
#escribimos en el fichero creado
echo
echo MI_ESCRIBIR -------------------------------------------
echo escribimos en el fichero creado
echo
./mi_escribir $1 /fichero.txt 0 hoasoyuntexto
#hacemos un mi_cat para ver el contenido del fichero
echo
echo MI_CAT -------------------------------------------
echo mostramos el contenido del fichero
echo
./mi_cat $1 /fichero.txt
