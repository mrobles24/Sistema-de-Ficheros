/*  AUTORES:

- Eloy Rafael
- Miquel Robles
- Soufyane Youbi
*/

#include "simulacion.h"

int acabados = 0;

void reaper() {
    pid_t ended;
    signal(SIGCHLD, reaper);
    while ((ended = waitpid(-1, NULL, WNOHANG)) > 0) {
        acabados++;
    }
}

int main(int argc, char **argv) {
    char nombreSimul[MAXTAMRUTA], nombreProceso[MAXTAMRUTA], nombreFichero[MAXTAMRUTA], fechaStr[15];
    int proceso, escritura, error;
    time_t fecha;
    pid_t pid;
    struct REGISTRO registro;

    //Comprobamos sintaxis
    if (argc != 2) {
        fprintf(stderr, "Formato válido: ./simulacion <disco>\n");
        return -1;
    }

    //Montamos dispositivo
    if (bmount(argv[1]) == -1) {
        perror("Error");
        return -1;
    }

    //Creamos el directorio de simulación /simul_aaaammddhhmmss
    fecha = time(NULL);
    strftime(fechaStr, sizeof(fechaStr), "%Y%m%d%H%M%S", localtime(&fecha));
    sprintf(nombreSimul, "/simul_%s/", fechaStr);
    if ((error = mi_creat(nombreSimul, 7)) < 0) {
        fprintf(stderr, "Error al crear directorio %s:\n", nombreSimul);
        mostrar_error_buscar_entrada(error);
        bumount();
        return -1;
    }

    signal(SIGCHLD, reaper);
    printf("*** SIMULACIÓN DE %d PROCESOS REALIZANDO CADA UNO %d ESCRITURAS *** \n\n", NUMPROCESOS, NUMESCRITURAS);

    for (proceso = 1; proceso <= NUMPROCESOS; proceso++) {
        //Si es el hijo
        if (fork() == 0) { 
            pid = getpid();
            //Montamos el dispositivo
            bmount(argv[1]);
            //Creamos el directorio
            sprintf(nombreProceso, "%sproceso_%d/", nombreSimul, pid);
            if ((error = mi_creat(nombreProceso, 7)) < 0) {
                mostrar_error_buscar_entrada(error);
                bumount();
                exit(1);
            }
            //Creamos el fichero
            sprintf(nombreFichero, "%sprueba.dat", nombreProceso);
            if ((error = mi_creat(nombreFichero, 7)) < 0) {
                mostrar_error_buscar_entrada(error);
                bumount();
                exit(1);
            }
            //Inicializar la semilla de números aleatorios
            srand(time(NULL) + getpid());
            for (escritura = 0; escritura < NUMESCRITURAS; escritura++) {
                //Inicializar registro
                registro.fecha = time(NULL);
                registro.pid = getpid();
                registro.nEscritura = escritura + 1;
                registro.nRegistro =  rand() % REGMAX;
                //Escribimos el registro en el struct
                if ((error = mi_write(nombreFichero, &registro, registro.nRegistro * sizeof(struct REGISTRO), sizeof(struct REGISTRO))) < 0) {
                    mostrar_error_buscar_entrada(error);
                    bumount();
                    exit(1);
                }

                if (DEBUGN12)
                {
                    fprintf(stderr, "[simulacion.c -> Escritura %d en %s]\n",escritura,nombreFichero);
                }
                
                //Esperamos para realizar la siguiente escritura
                usleep(50000);
            }
            //Desmontamos dispositivo
            bumount();
            printf("[Proceso %d: Completadas %d escrituras en %s]\n", proceso, NUMESCRITURAS, nombreFichero);
            //printf("\033[1mProceso %d: Completadas %d escrituras en %s\033[0m\n", proceso, NUMESCRITURAS, nombreFichero);
            exit(0);
        }
        //Esperamos para lanzar el siguiente proceso
        usleep(200000);
    }
    //Permitir que el padre espere por todos los hijos
    while (acabados < NUMPROCESOS) {
        pause();
    }

    //Desmontar dispositivo
    if (bumount() == -1) {
        perror("Error");
        return -1;
    }

    return 0;
}