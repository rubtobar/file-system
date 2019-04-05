#include "directorios.h"
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

struct registro {
    int fecha;
    int pid;
    int escritura;
    int posicion;
};

struct registro p_escritura;
struct registro u_escritura;

struct registro p_posicion;
struct registro u_posicion;

int optimizacion(char* ruta, struct registro reg, int j, int pid){

    if(mi_read(ruta, &reg, j*sizeof(struct registro), sizeof(struct registro)) > 0){

        if (reg.pid == pid){

            if (reg.escritura < p_escritura.escritura) {
                memcpy(&p_escritura, &reg, sizeof(struct registro));
            }
            if (reg.escritura > u_escritura.escritura) {
                memcpy(&u_escritura, &reg, sizeof(struct registro));                    
            }
            if (reg.posicion < p_posicion.posicion) {
                memcpy(&p_posicion, &reg, sizeof(struct registro));
            }
            if (reg.posicion > u_posicion.posicion) {
                memcpy(&u_posicion, &reg, sizeof(struct registro));
            }

            return(1);
        }
    }
    return(0);
}

int main(int argc, char *argv[]){
    struct STAT stat;
    unsigned int nentradas;
    struct entrada entr;
    char *ruta = malloc(100);
    char *ruta_informe = malloc(100);
    char *ruta_directorio = malloc(100);
    int pid;
    char *pidc = malloc(50);
    int reg_encontrados = 0;
    char *escritura = malloc(512);
    char *straux = malloc(100);
    time_t time;
    struct registro reg[64];

    if (argv[1] == '\0') {
        printf("./simulacion disco /directorio_simulacion \n");
        return -1;
    }
    
    bmount(argv[1]);

    //comprovamos estado del directorio
    mi_stat(argv[2], &stat);
    
    nentradas = stat.tamEnBytesLog/sizeof(struct entrada);

    printf("Numero de entradas en el directorio: %u\n", nentradas);
    
    strcpy(ruta_informe, argv[2]);
    strcpy(ruta_directorio, argv[2]);
    
    strcat(ruta_informe, "/informe.txt");

    printf("Ruta dir simulacion: %s\n", ruta_directorio);
    
    if(mi_create(ruta_informe, 7) < 0){
        printf("el archivo informe.txt ya existe\n");
        return -1;
    }
    
    //a continuación empezamos a leer las escrituras producidas por el primer proceso
    //primero accedemos a la primera entrada:

    //strcpy(ruta, argv[2]);

    //inicializamos el valor de los registros
        p_escritura.escritura = INT_MAX;
        u_escritura.escritura = 0;
        p_posicion.posicion = INT_MAX;
        u_posicion.posicion = 0;

    //leemos entrada para acceder a sus datos i acabar la ruta
        int j = 0;
        int l = 0;
        int var = 0;
        int recorte = 0;

    for (int i = 0; i < nentradas; ++i){

        //printf("verificaciones terminadas: %u/%d", i, nentradas);
        /*
        sprintf(straux, "%d", i);
        strcpy(escritura,"progreso: ");
        strcat(escritura,straux);
        sprintf(straux, "%d", nentradas);
        strcat(escritura,"/");
        strcat(escritura,straux);
        puts(escritura);
        */
        printf("procesos verificados: %d/%d\n", i+1,nentradas);
        
        mi_read(ruta_directorio, &entr, i*sizeof(struct entrada), sizeof(struct entrada));
        //vamos poniendo las diferentes rutas por cada proceso a vericficar
        memset(ruta,0,100);
        strcpy(ruta, argv[2]);
        strcat(ruta,"/");
        strcat(ruta, entr.nombre);
        strcat(ruta, "/prueba.dat");
        //a continuacion leemos el archivo al que corresponde la ruta para extraer los registros
        //este archivo lo leeremos en bloques de 64 en 64 para aumentar la eficiencia
        //primero obtenemos el tamaño del archivo prueba
        strcpy(pidc, entr.nombre);

        pid = atoi(pidc + 8);
        mi_stat(ruta, &stat);

        j = 0;
        for (; j < (stat.tamEnBytesLog/sizeof(struct registro));){
            
            if ((stat.tamEnBytesLog/sizeof(struct registro)) >= j+64){

                memset(&reg,0,64*sizeof(struct registro));

                mi_read(ruta, &reg, j*sizeof(struct registro), 64*sizeof(struct registro));

                for (l = 0; l < 64; ++l){

                    if (reg[l].pid == pid){

                        if (reg[l].escritura < p_escritura.escritura) {
                            memcpy(&p_escritura, &reg[l], sizeof(struct registro));
                        }
                        if (reg[l].escritura > u_escritura.escritura) {
                            memcpy(&u_escritura, &reg[l], sizeof(struct registro));                    
                        }
                        if (reg[l].posicion < p_posicion.posicion) {
                            memcpy(&p_posicion, &reg[l], sizeof(struct registro));
                        }
                        if (reg[l].posicion > u_posicion.posicion) {
                            memcpy(&u_posicion, &reg[l], sizeof(struct registro));
                        }
                        reg_encontrados++;
                        //printf("reg_encontrados %d\n", reg_encontrados);
                    }
                }
                
                j = j + 64;

            }else{
                
                var = (stat.tamEnBytesLog/sizeof(struct registro)) - j;
                //printf("var: %d\n", var);

                for (l = 0; l < var; ++l){

                    //printf("ruta %d\n", j);
                    //printf("tam max%lu\n", (stat.tamEnBytesLog/sizeof(struct registro)));
                    memset(&reg,0,64*sizeof(struct registro));

                    mi_read(ruta, &reg[0], j*sizeof(struct registro), sizeof(struct registro));

                    j++;

                    if (reg[0].pid == pid){

                        if (reg[0].escritura < p_escritura.escritura) {
                            memcpy(&p_escritura, &reg[0], sizeof(struct registro));
                        }
                        if (reg[0].escritura > u_escritura.escritura) {
                            memcpy(&u_escritura, &reg[0], sizeof(struct registro));                    
                        }
                        if (reg[0].posicion < p_posicion.posicion) {
                            memcpy(&p_posicion, &reg[0], sizeof(struct registro));
                        }
                        if (reg[0].posicion > u_posicion.posicion) {
                            memcpy(&u_posicion, &reg[0], sizeof(struct registro));
                        }
                        reg_encontrados++;
                        //printf("reg_encontrados %d\n", reg_encontrados);
                    }              
                }
            }
        }

        //printf("pid: %d\n", pid);
        //usleep(2000000);
        memset(escritura,0,512);
        memset(straux,0,100);
        strcpy(escritura,"\n#############\n");

        strcat(escritura, entr.nombre);
        //strcat(escritura,straux);
        strcat(escritura,"\nnºregistros: ");
        sprintf(straux, "%d", reg_encontrados);
        strcat(escritura,straux);
        //primera escritura
        memset(straux,0,100);
        strcat(escritura,"\nregistro primera escritura:");
        strcat(escritura,"\n\tescritura: ");
        sprintf(straux, "%d", p_escritura.escritura);
        strcat(escritura,straux);
        strcat(escritura,"\n\tposicion: ");
        sprintf(straux, "%d", p_escritura.posicion);
        strcat(escritura,straux);
        strcat(escritura,"\n\tfecha: ");
        time = (time_t) p_escritura.fecha;
        memset(straux,0,100);
        strcpy(straux, asctime(localtime(&time)));
        strcat(escritura,straux);
        strcat(escritura,"-------------");
        //ultima escritura
        memset(straux,0,100);
        strcat(escritura,"\nregistro ultima escritura:");
        strcat(escritura,"\n\tescritura: ");
        sprintf(straux, "%d", u_escritura.escritura);
        strcat(escritura,straux);
        strcat(escritura,"\n\tposicion: ");
        sprintf(straux, "%d", u_escritura.posicion);
        strcat(escritura,straux);
        strcat(escritura,"\n\tfecha: ");
        time = (time_t) u_escritura.fecha;
        memset(straux,0,100);
        strcpy(straux, asctime(localtime(&time)));
        strcat(escritura,straux);
        strcat(escritura,"-------------");
        //primera posicion
        memset(straux,0,100);
        strcat(escritura,"\nregistro primera posicion:");
        strcat(escritura,"\n\tescritura: ");
        sprintf(straux, "%d", p_posicion.escritura);
        strcat(escritura,straux);
        strcat(escritura,"\n\tposicion: ");
        sprintf(straux, "%d", p_posicion.posicion);
        strcat(escritura,straux);
        strcat(escritura,"\n\tfecha: ");
        time = (time_t) p_posicion.fecha;
        memset(straux,0,100);
        strcpy(straux, asctime(localtime(&time)));
        strcat(escritura,straux);
        strcat(escritura,"-------------");
        //ultima posicion
        memset(straux,0,100);
        strcat(escritura,"\nregistro ultima posicion:");
        strcat(escritura,"\n\tescritura: ");
        sprintf(straux, "%d", u_posicion.escritura);
        strcat(escritura,straux);
        strcat(escritura,"\n\tposicion: ");
        sprintf(straux, "%d", u_posicion.posicion);
        strcat(escritura,straux);
        strcat(escritura,"\n\tfecha: ");
        time = (time_t) u_posicion.fecha;
        memset(straux,0,100);
        strcpy(straux, asctime(localtime(&time)));
        strcat(escritura,straux);
        strcat(escritura,"-------------");

        //quitando basura del archivo
        while(escritura[511 - recorte] == 0){
            escritura[511 - recorte] = ' ';
            recorte++;
        }
        recorte = 0;

        mi_write(ruta_informe, escritura, i*512, 512);

        //printf("%s", escritura);

        //return 0;


        memset(&p_escritura,0,sizeof(p_escritura));
        memset(&u_escritura,0,sizeof(p_escritura));
        memset(&p_posicion,0,sizeof(p_escritura));
        memset(&u_posicion,0,sizeof(p_escritura));
        p_escritura.escritura = INT_MAX;
        u_escritura.escritura = 0;
        p_posicion.posicion = INT_MAX;
        u_posicion.posicion = 0;

        reg_encontrados = 0;

        //strcpy(ruta, argv[2]);      //reiniciamos el vamor de la ruta para buscar la sigiente carpeta
    }

    free(ruta);
    free(ruta_informe);
    free(ruta_directorio);
    free(pidc);
    free(escritura);
    free(straux);
    
    
    bumount();
    
    return 0;
}