/*  AUTORES:

- Eloy Rafael
- Miquel Robles
- Soufyane Youbi
*/

#include "directorios.h"

int main(int argc, char **argv) {

    int mi_link_error;

    //Comprobamos formato
    if (argc != 4) {
        fprintf(stderr, "Formato válido: ./mi_link <disco> </ruta_fichero_original> </ruta_enlace>\n");
        return 1;
    }

    //Comprobamos rutas
    if (argv[2][strlen(argv[2]) - 1] == '/') {
        fprintf(stderr, "Error: %s no es un fichero.\n", argv[2]);
        return 1;
    }
    if (argv[3][strlen(argv[3]) - 1] == '/') {
        fprintf(stderr, "Error: %s no es un fichero.\n", argv[3]);
        return 1;
    }

    //Montamos dispositivo
    if (bmount(argv[1]) == -1) {
        perror("Error");
        return 1;
    }

    //Creamos el enlace
    if ((mi_link_error = mi_link(argv[2], argv[3])) < 0) {
        mostrar_error_buscar_entrada(mi_link_error);
    }

    //Desmontamos dispositivo
    if (bumount() == -1) {
        perror("Error");
        return 1;
    }

    return 0;
}