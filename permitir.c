/*  AUTORES:

- Eloy Rafael
- Miquel Robles
- Soufyane Youbi
*/

#include "ficheros.h"

int main(int argc, char **argv)
{
    // Si el numero de argumentos no es valido, lo anunciamos
    if (argc < 4)
    {
        fprintf(stderr, "Error, número de argumentos no válidos\n");
        fprintf(stderr, "Formato válido: permitir <nombre_dispositivo> <ninodo> <permisos>\n");
        return -1;
    }

    // Montamos el dispositivo
    if (bmount(argv[1]) == -1)
    {
        perror("Error:");
        return -1;
    }

    // Recuperamos numero de inodo y sus nuevos permisos
    int ninodo = atoi(argv[2]);
    int permiso = atoi(argv[3]);

    // Cambiamos sus permisos
    if (mi_chmod_f(ninodo, permiso) == -1)
    {
        perror("Error:");
        return -1;
    }

    // Desmontamos el dispositivo
    if (bumount() == -1)
    {
        perror("Error:");
        return -1;
    }
}