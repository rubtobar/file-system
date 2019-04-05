#include "directorios.h"
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define NUM_PROCESOS 100
#define NUM_ESCRITURAS 50
#define MAX_OFFSET 500000

#define tam_camino 128

struct registro {
    int fecha;
    int pid;
    int escritura;
    int posicion;
};

int hijosterminados = 0;
struct tm *ts;
char buf[128];
pid_t pid;
time_t tiempo;
char *ruta;
struct registro reg;



void reaper(){
    while(wait3(NULL, WNOHANG, NULL) > 0){
        hijosterminados++;
    }  
}

int proceso(char *sf){

    bmount(sf);    //montamos archivo

    memset(buf,0,tam_camino);

    strftime(buf, tam_camino, "/simul_%Y%m%d%H%M%S/", ts);
    //ponemos en nombre junto al pid al nombre de el directorio
    strcat(buf, "proceso_");
    sprintf(ruta, "%d/", getpid());
    strcat(buf, ruta);
    //cada proceso crea su directorio
    
    if (mi_create(buf, 7) < 0){
        printf("problema al crear el directorio: %s\n", buf);
        exit(0);
    }

    //ponemos el nombre para el archivo con las pruebas
    strcat(buf, "prueba.dat");
    //creamos archivo de prueba

    if (mi_create(buf, 7) < 0){
        printf("problema al crear el archivo: %s\n", buf);
        exit(0);
    }

    //inicializar semillla para random
    srand(time(NULL)+getpid());

    //empezamos a hacer escrituras
    for (int j = 0; j < NUM_ESCRITURAS; j++) {
        reg.fecha = (int) time(NULL);
        reg.pid = getpid();
        reg.escritura = j+1;
        reg.posicion = rand() % MAX_OFFSET;
        if(mi_write(buf, &reg, reg.posicion*sizeof(struct registro), sizeof(struct registro)) < 0){
            printf("%d fallo escritura: %d\n", getpid(), j);
            exit(0);
        }

        usleep(50000);
    }

    printf("proceso %d terminado\n", getpid());
    bumount();

    exit(0);
}

int main(int argc, char *argv[]){

    ruta = malloc(128);
    
    if (argv[1] == '\0') {
        printf("./simulacion disco\n");
        return -1;
    }
    
    bmount(argv[1]);
    
    //creamos directorio de simulación
    time(&tiempo);
    ts = localtime(&tiempo);
    strftime(buf, 50, "/simul_%Y%m%d%H%M%S/", ts);

    //creamos directorio de simulacion
    mi_create(buf, 7);

    hijosterminados = 0;
    signal(SIGCHLD, reaper);
    
    for (int i = 0; i < NUM_PROCESOS; ++i){
        if(fork() == 0) proceso(argv[1]); //Creamos proceso
        usleep(200000); //Esperamos paraa crear sig proceso
    }
    
    if(hijosterminados < NUM_PROCESOS) pause(); //Esperamos a que todos los procesos acaben 

    //desmontamos archivo
    bumount();
    
    return 0;
}