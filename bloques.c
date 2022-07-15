/*  AUTORES:

- Eloy Rafael
- Miquel Robles
- Soufyane Youbi
*/

#include "bloques.h"
#include "semaforo_mutex_posix.h"

static int descriptor = 0;
static sem_t *mutex;
static unsigned int inside_sc = 0;

// ***** FUNCIONES NIVEL 1 ***** //

/* bmount:
    Función para montar el dispositivo virtual, y dado que
    se trata de un fichero, esa acción consistirá en abrirlo.
*/
int bmount(const char *camino)
{
    umask(000);

    // Abrimos el fichero con permisos de lectura y escritura
    descriptor = open(camino, O_RDWR | O_CREAT, 0666);

    if (descriptor == -1)
    {
        perror("Error");
    }

    if (!mutex)
    { // el semáforo es único en el sistema y sólo se ha de inicializar 1 vez (padre)
        mutex = initSem();
        if (mutex == SEM_FAILED)
        {
            return -1;
        }
    }
    return descriptor;
}

/* bumount:
    Desmonta el dispositivo virtual. Básicamente llama a la función
    close() para liberar el descriptor de fichero.
*/
int bumount()
{
    int x = close(descriptor);
    deleteSem();
    if (x == -1)
    {
        perror("Error");
    }
    return x;
}

/* bwrite:
    Escribe 1 bloque en el dispositivo virtual, en el bloque físico especificado por nbloque.
*/
int bwrite(unsigned int nbloque, const void *buf)
{
    off_t sitio = nbloque * BLOCKSIZE;

    // Nos colocamos en el sitio correspondiente
    off_t colocar = lseek(descriptor, sitio, SEEK_SET);

    // Escribimos en ese lugar
    size_t escribir = write(descriptor, buf, BLOCKSIZE);

    if ((colocar | escribir) == -1)
    {
        perror("Error");
        return -1;
    }
    return BLOCKSIZE;
}

/* bread:
    Lee 1 bloque del dispositivo virtual, que se corresponde con el bloque físico especificado
    por nbloque.
*/
int bread(unsigned int nbloque, void *buf)
{
    off_t sitio = nbloque * BLOCKSIZE;

    // Nos colocamos en el sitio correspondiente
    off_t colocar = lseek(descriptor, sitio, SEEK_SET);

    // Leemos en ese lugar
    size_t leer = read(descriptor, buf, BLOCKSIZE);

    if ((colocar | leer) == -1)
    {
        perror("Error");
        return -1;
    }
    return BLOCKSIZE;
}

/* mi_waitSem:
    Función que llama a waitSem.
*/
void mi_waitSem()
{
    if (!inside_sc)
    { // inside_sc==0
        waitSem(mutex);
    }
    inside_sc++;
}

/* mi_signalSem:
    Función que llama a signalSem.
*/
void mi_signalSem()
{
    inside_sc--;
    if (!inside_sc)
    {
        signalSem(mutex);
    }
}