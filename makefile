DEBUG = -g
CFLAGS = -Wall -std=gnu99 -pthread
PROGRAMS = mi_mkfs mi_mkdir mi_chmod mi_ls mi_stat mi_cat mi_ln mi_rm mi_escribir leer_sf simulacion verificacion

BLOQUES = bloques.c
FB = ficheros_basico.c
F = ficheros.c
D = directorios.c
MUT = semaforo_mutex_posix.c

all: $(PROGRAMS)	$(LIBS)

%: %.c
	$(CC) $(DEBUG) $(CFLAGS) $(BLOQUES) $(FB) $(F) $(D) $(MUT) -o $@ $<

.PHONY: clean

clean:
	$(RM) -rf *.o $(PROGRAMS)
	
