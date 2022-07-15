/*  AUTORES:

- Eloy Rafael
- Miquel Robles
- Soufyane Youbi
*/

#include "directorios.h"

int main(int argc, char *argv[]){
    
    int error;

    // Comprobamos formato
    if (argc != 3) {
        fprintf(stderr, "Formato válido: ./mi_rm disco /ruta\n");
        return -1;
    }
    
    // Montamos dispositivo
    if (bmount(argv[1]) == -1) {
        perror("Error");
        return -1;
    }

    // Eliminamos el enlace
    if((error = mi_unlink(argv[2])) < 0){
        mostrar_error_buscar_entrada(error);
        return -1;
    }

    // Desmontamos dispositivo
    if (bumount() == -1) {
        perror("Error");
        return -1;
    }
    
    return 0;
}