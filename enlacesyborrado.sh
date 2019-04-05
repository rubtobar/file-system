#script
#!/bin/bash
# $1 --> Disco virtual

#Crear carpetas y ficheros
echo ┌──────────────────────────────────────────────────┐
echo │./mi_mkdir $1 7 /dirA0/                         
echo │./mi_mkdir $1 7 /dirA0/A.dat                    
echo │./mi_mkdir $1 7 /dirB0/                         
echo │./mi_mkdir $1 7 /dirC0/                         
echo │./mi_mkdir $1 7 /dirD0/                         
echo └──────────────────────────────────────────────────┘

./mi_mkdir $1 7 /dirA0/
./mi_mkdir $1 7 /dirA0/A.dat
./mi_mkdir $1 7 /dirB0/
./mi_mkdir $1 7 /dirC0/
./mi_mkdir $1 7 /dirD0/

echo ┌───────────────────────────────────────────────────┐
echo │./mi_escribir $1 /dirA0/A.dat                    
echo └───────────────────────────────────────────────────┘
./mi_escribir $1 /dirA0/A.dat 0 holasoyuntexto

#Comprobar la informacion actual del fichero A.dat
echo ┌───────────────────────────────────────────────────┐
echo │Ejecutando ./mi_stat $1 /dirA0/A.dat             
echo └───────────────────────────────────────────────────┘
./mi_stat $1 /dirA0/A.dat

echo ┌───────────────────────────────────────────────────┐
echo │Ejecutando ./mi_cat $1 /dirA0/A.dat              
echo └───────────────────────────────────────────────────┘
./mi_cat $1 /dirA0/A.dat
echo -------------------------------------------


echo ┌───────────────────────────────────────────────────┐
echo │Ejecutando ./mi_ln $1 /dirA0/A.dat /dirB0/B.dat
echo └───────────────────────────────────────────────────┘
./mi_ln $1 /dirA0/A.dat /dirB0/B.dat

echo ┌───────────────────────────────────────────────────┐
echo │Ejecutando ./mi_ln $1 /dirA0/A.dat /dirC0/C.dat
echo └───────────────────────────────────────────────────┘
./mi_ln $1 /dirA0/A.dat /dirC0/C.dat

echo ┌───────────────────────────────────────────────────┐
echo │Ejecutando ./mi_ln $1 /dirA0/A.dat /dirD0/D.dat
echo └───────────────────────────────────────────────────┘
./mi_ln $1 /dirA0/A.dat /dirD0/D.dat
echo -------------------------------------------


echo ┌──────────────────────────────────────────────────┐
echo │Ejecutando ./mi_stat $1 /dirA0/A.dat            
echo └──────────────────────────────────────────────────┘
./mi_stat $1 /dirA0/A.dat



echo ┌──────────────────────────────────────────────────┐
echo │Ejecutando ./mi_cat $1  /dirB0/B.dat            
echo └──────────────────────────────────────────────────┘
./mi_cat $1 /dirB0/B.dat
echo ┌──────────────────────────────────────────────────┐
echo │Ejecutando ./mi_cat $1 /dirC0/C.dat             
echo └──────────────────────────────────────────────────┘
./mi_cat $1 /dirC0/C.dat
echo ┌──────────────────────────────────────────────────┐
echo │Ejecutando ./mi_cat $1 /dirD0/D.dat             
echo └──────────────────────────────────────────────────┘
./mi_cat $1 /dirD0/D.dat
echo -------------------------------------------


echo ┌──────────────────────────────────────────────────┐
echo │Ejecutando ./mi_rm $1  /dirB0/B.dat		
echo └──────────────────────────────────────────────────┘
./mi_rm $1 /dirB0/B.dat
echo ┌──────────────────────────────────────────────────┐
echo │Ejecutando ./mi_rm $1 /dirC0/C.dat		
echo └──────────────────────────────────────────────────┘
./mi_rm $1 /dirC0/C.dat
echo ┌──────────────────────────────────────────────────┐
echo │Ejecutando ./mi_rm $1 /dirD0/D.dat		
echo └──────────────────────────────────────────────────┘
./mi_rm $1 /dirD0/D.dat

echo ┌──────────────────────────────────────────────────┐
echo │Ejecutando ./mi_stat $1 /dirA0/A.dat		
echo └──────────────────────────────────────────────────┘
./mi_stat $1 /dirA0/A.dat
echo ┌──────────────────────────────────────────────────┐
echo │Ejecutando ./mi_rm $1 /dirA0/A.dat		
echo └──────────────────────────────────────────────────┘
./mi_rm $1 /dirA0/A.dat


