/*  AUTORES:

- Eloy Rafael
- Miquel Robles
- Soufyane Youbi
*/

#include "directorios.h"

int main(int argc, char **argv) {

    char buffer[2000];
    int totEnt;

    //En caso de que no haya 3 argumentos mostramos el error por pantalla
    if (argc != 3) {
        fprintf(stderr, "Formato válido: ./mi_ls <disco> </ruta_directorio>\n");
        return -1;
    }

    //Montamos el dispositivo
    if (bmount(argv[1]) == -1) {
        perror("Error");
        return -1;
    }
    
    //Imprimimos total de entradas y los campos si hay entradas
    if ((totEnt = mi_dir(argv[2], buffer)) < 0) {

        mostrar_error_buscar_entrada(totEnt);

    } else { 

        fprintf(stderr,"Total: %d\n", totEnt);

        if (totEnt > 0) {
            fprintf(stderr,"\033[1;34mTipo\t\tModo\tmTime\t\t\t\tTamaño\t\tNombre\033[0m\n");
            fprintf(stderr,"%s", buffer);
        }
    }

    //Desmontamos el dispositivo
    if (bumount() == -1) {
        perror("Error");
        return -1;
    }

    return 0;
}