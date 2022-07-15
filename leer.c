/*  AUTORES:

- Eloy Rafael
- Miquel Robles
- Soufyane Youbi
*/

#include "ficheros.h"
#define tambuffer 1500 

int main(int argc, char **argv)
{   
    int offset = 0;
    int bytes_leidos = 0;
    int leidos = 0;
    struct STAT stat;

    // Si el numero de argumentos no es leidosido, lo anunciamos
    if (argc != 3)
    {
        fprintf(stderr, "Formato válido: leer <nombre_dispositivo> <ninodo>\n");
        return -1;
    }
    // control de bmount
    if (bmount(argv[1]) == -1)
    {
        perror("Error");
        return -1;
    }

    // Recuperamos el numero de inodo
    int ninodo = atoi(argv[2]);

    // Creamos un espacio para el buffer y lo llenamos de 0's
    unsigned char buffer[tambuffer];
    memset(buffer, 0, tambuffer);

    // Mientras hay cosas a leer
    while ((leidos = mi_read_f(ninodo, buffer, offset, tambuffer)) > 0)
    {
        // Aumentamos numero de bytes leidos y lo imprimimos por pantalla
        bytes_leidos += leidos;
        write(1, buffer, leidos);

        // Aumentamos el offset y volvemos a limpiar el buffer
        offset += tambuffer;
        memset(buffer, 0, tambuffer);

        // Leemos otra vez
        leidos = mi_read_f(ninodo, buffer, offset, tambuffer);
    }

    // Imprimimos por pantalla bytes leidos y su tamaño en bytes logicos
    fprintf(stderr,"\nBytes leídos: %d\n", bytes_leidos);
    mi_stat_f(ninodo, &stat);
    fprintf(stderr,"Tamaño en bytes lógicos: %d\n", stat.tamEnBytesLog);

    // Desmontamos el dispositivo
    if (bumount() == -1)
    {
        perror("Error");
    }
}