/*  AUTORES:

- Eloy Rafael
- Miquel Robles
- Soufyane Youbi
*/

#include "directorios.h"

int main(int argc, char **argv) {
    char *nombre_dispositivo, *ruta;
    int permisos, mi_chmod_err;

    //Comprobamos el número de argumentos
    if (argc != 4) {
        fprintf(stderr, "Formato válido: ./mi_chmod <disco> <permisos> </ruta>\n");
        return -1;
    }

    nombre_dispositivo = argv[1];
    permisos = atoi(argv[2]);

    ruta = argv[3];

    //Comprobamos los permisos sean válidos
    if (permisos < 0 || permisos > 7) {
        fprintf(stderr, "Error: Permisos ha de estar entre 0 y 7.\n");
        return -1;
    }

    //Montamos dispositivo
    if (bmount(nombre_dispositivo) == 1) {
        perror("Error");
        return -1;
    }

    //Llamamos a mi_chmod para modificar permisos
    if ((mi_chmod_err = mi_chmod(ruta, permisos)) < 0) {
        mostrar_error_buscar_entrada(mi_chmod_err);
    }

    //Desmontamos dispositivo
    if (bumount() == 1) {
        perror("Error");
        return -1;
    }

    return EXIT_SUCCESS;
}