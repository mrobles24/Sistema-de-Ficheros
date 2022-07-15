/*  AUTORES:

- Eloy Rafael
- Miquel Robles
- Soufyane Youbi
*/

#include "ficheros.h"

int main(int argc, char **argv)
{
    int ninodo1, ninodo2, ninodo3, ninodo4, ninodo5;
    int bytes1, bytes2, bytes3, bytes4, bytes5;

    // Si el numero de argumentos no es valido, lo anunciamos
    if (argc < 2)
    {
        fprintf(stderr, "Formato válido :escribir <nombre_dispositivo> <$(cat fichero)> <diferentes_inodos>\n");
        return -1;
    }

    // Montamos el dispositivo 
    if (bmount(argv[1]) == -1)
    {
        return -1;
    }

    // Recuperamos el numero de bytes y los diferentes inodos
    int nbytes = strlen(argv[2]);
    int diferentes_inodos = atoi(argv[3]);
    struct STAT stat;

    // Si hay diferentes inodos
    if (diferentes_inodos == 1)
    { 
        // ***** INODO 1 ***** //

        // Obtenemos el primer inodo
        ninodo1 = reservar_inodo('f', 6);
        if (ninodo1 == -1)
        {
            bumount();
            return -1;
        }

        // Lo imprimimos por pantalla
        fprintf(stderr,"\n\n");
        fprintf(stderr,"Nº inodo reservado: %d\n", ninodo1);
        fprintf(stderr,"offset: 9000\n");

        // Traducimos su bloque
        traducir_bloque_inodo(ninodo1, 8, 1);

        // Escribimos bytes
        bytes1 = mi_write_f(ninodo1, argv[2], 9000, nbytes * sizeof(char));
        fprintf(stderr,"Bytes escritos: %d\n", bytes1);

        // Recuperamos su estado
        mi_stat_f(ninodo1, &stat);
        fprintf(stderr,"stat.tamEnBytesLog= %d\n", stat.tamEnBytesLog);
        fprintf(stderr,"stat.numBloquesOcupados= %d\n", stat.numBloquesOcupados);

        // ***** INODO 2 ***** //

        // Obtenemos el segundo inodo
        ninodo2 = reservar_inodo('f', 6);
        if (ninodo2 == -1)
        {
            bumount();
            return -1;
        }

        // Lo imprimimos por pantalla
        fprintf(stderr,"\n\n");
        fprintf(stderr,"Nº inodo reservado: %d\n", ninodo2);
        fprintf(stderr,"offset: 209000\n");

        // Traducimos su bloque
        traducir_bloque_inodo(ninodo2, 204, 1);

        // Escribimos bytes
        bytes2 = mi_write_f(ninodo2, argv[2], 209000, nbytes * sizeof(char));
        fprintf(stderr,"Bytes escritos: %d\n", bytes2);

        // Recuperamos su estado
        mi_stat_f(ninodo2, &stat);
        fprintf(stderr,"stat.tamEnBytesLog= %d\n", stat.tamEnBytesLog);
        fprintf(stderr,"stat.numBloquesOcupados= %d\n", stat.numBloquesOcupados);

        // ***** INODO 3 ***** //

        // Obtenemos el tercer inodo
        ninodo3 = reservar_inodo('f', 6);
        if (ninodo3 == -1)
        {
            bumount();
            return -1;
        }

        // Lo imprimimos por pantalla
        fprintf(stderr,"\n\n");
        fprintf(stderr,"Nº inodo reservado: %d\n", ninodo3);
        fprintf(stderr,"offset: 30725000\n");

        // Traducimos su bloque
        traducir_bloque_inodo(ninodo3, 30004, 1);

        // Escribimos bytes
        bytes3 = mi_write_f(ninodo3, argv[2], 30725000, nbytes * sizeof(char));
        fprintf(stderr,"Bytes escritos: %d\n", bytes3);

        // Recuperamos su estado
        mi_stat_f(ninodo3, &stat);
        fprintf(stderr,"stat.tamEnBytesLog= %d\n", stat.tamEnBytesLog);
        fprintf(stderr,"stat.numBloquesOcupados= %d\n", stat.numBloquesOcupados);

        // ***** INODO 4 ***** //

        // Obtenemos el cuarto inodo
        ninodo4 = reservar_inodo('f', 6);
        if (ninodo4 == -1)
        {
            bumount();
            return -1;
        }

        // Lo imprimimos por pantalla
        fprintf(stderr,"\n\n");
        fprintf(stderr,"Nº inodo reservado: %d\n", ninodo4);
        fprintf(stderr,"offset: 409605000\n");

        // Traducimos su bloque
        traducir_bloque_inodo(ninodo4, 400004, 1);

        // Escribimos bytes
        bytes4 = mi_write_f(ninodo4, argv[2], 409605000, nbytes * sizeof(char));
        fprintf(stderr,"Bytes escritos: %d\n", bytes4);

        // Recuperamos su estado
        mi_stat_f(ninodo4, &stat);
        fprintf(stderr,"stat.tamEnBytesLog= %d\n", stat.tamEnBytesLog);
        fprintf(stderr,"stat.numBloquesOcupados= %d\n", stat.numBloquesOcupados);

        // ***** INODO 5 ***** //

        // Obtenemos el quinto inodo
        ninodo5 = reservar_inodo('f', 6);
        if (ninodo5 == -1)
        {
            bumount();
            return -1;
        }

        // Lo imprimimos por pantalla
        fprintf(stderr,"\n\n");
        fprintf(stderr,"Nº inodo reservado: %d\n", ninodo5);
        fprintf(stderr,"offset: 480000000\n");

        // Traducimos su bloque
        traducir_bloque_inodo(ninodo5, 468750, 1);

        // Escribimos bytes
        bytes5 = mi_write_f(ninodo5, argv[2], 480000000, nbytes * sizeof(char));
        fprintf(stderr,"Bytes escritos: %d\n", bytes5);

        // Recuperamos su estado
        mi_stat_f(ninodo5, &stat);
        fprintf(stderr,"stat.tamEnBytesLog= %d\n", stat.tamEnBytesLog);
        fprintf(stderr,"stat.numBloquesOcupados= %d\n", stat.numBloquesOcupados);

        // Desmontamos el dispositivo
        bumount();
    }
    // Si no hay diferentes inodos, reservamos uno para los offsets
    else
    { 
        // Obtención del inodo
        ninodo1 = reservar_inodo('f', 6);
        if (ninodo1 == -1)
        {
            bumount();
            return -1;
        }

        // Lo imprimimos por pantalla
        fprintf(stderr,"\n\n");
        fprintf(stderr,"Nº inodo reservado: %d\n", ninodo1);
        fprintf(stderr,"offset: 9000\n");

        // Traducimos su bloque
        traducir_bloque_inodo(ninodo1, 8, 1);

        // Escribimos bytes
        bytes1 = mi_write_f(ninodo1, argv[2], 9000, nbytes * sizeof(char));
        fprintf(stderr,"Bytes escritos: %d\n", bytes1);

        // Recuperamos su estado
        mi_stat_f(ninodo1, &stat);
        fprintf(stderr,"stat.tamEnBytesLog= %d\n", stat.tamEnBytesLog);
        fprintf(stderr,"stat.numBloquesOcupados= %d\n", stat.numBloquesOcupados);

        // Repetimos procedimiento para cada offset:
        // Offset 209000
        fprintf(stderr,"\n\n");
        fprintf(stderr,"Nº inodo reservado: %d\n", ninodo1);
        fprintf(stderr,"offset: 209000\n");
        traducir_bloque_inodo(ninodo1, 204, 1);
        bytes2 = mi_write_f(ninodo1, argv[2], 209000, nbytes * sizeof(char));
        fprintf(stderr,"Bytes escritos: %d\n", bytes2);
        mi_stat_f(ninodo1, &stat);
        fprintf(stderr,"stat.tamEnBytesLog= %d\n", stat.tamEnBytesLog);
        fprintf(stderr,"stat.numBloquesOcupados= %d\n", stat.numBloquesOcupados);

        // Offset 30725000
        fprintf(stderr,"\n\n");
        fprintf(stderr,"Nº inodo reservado: %d\n", ninodo1);
        fprintf(stderr,"offset: 30725000\n");
        traducir_bloque_inodo(ninodo1, 30004, 1);
        bytes3 = mi_write_f(ninodo1, argv[2], 30725000, nbytes * sizeof(char));
        fprintf(stderr,"Bytes escritos: %d\n", bytes3);
        mi_stat_f(ninodo1, &stat);
        fprintf(stderr,"stat.tamEnBytesLog= %d\n", stat.tamEnBytesLog);
        fprintf(stderr,"stat.numBloquesOcupados= %d\n", stat.numBloquesOcupados);

        // Offset 409605000
        fprintf(stderr,"\n\n");
        fprintf(stderr,"Nº inodo reservado: %d\n", ninodo1);
        fprintf(stderr,"offset: 409605000\n");
        traducir_bloque_inodo(ninodo1, 400004, 1);
        bytes4 = mi_write_f(ninodo1, argv[2], 409605000, nbytes * sizeof(char));
        fprintf(stderr,"Bytes escritos: %d\n", bytes4);
        mi_stat_f(ninodo1, &stat);
        fprintf(stderr,"stat.tamEnBytesLog= %d\n", stat.tamEnBytesLog);
        fprintf(stderr,"stat.numBloquesOcupados=%d\n", stat.numBloquesOcupados);

        // Offset 480000000
        fprintf(stderr,"\n\n");
        fprintf(stderr,"Nº inodo reservado: %d\n", ninodo1);
        fprintf(stderr,"offset: 480000000\n");
        traducir_bloque_inodo(ninodo1, 468750, 1);
        bytes5 = mi_write_f(ninodo1, argv[2], 480000000, nbytes * sizeof(char));
        fprintf(stderr,"Bytes escritos: %d\n", bytes5);
        mi_stat_f(ninodo1, &stat);
        fprintf(stderr,"stat.tamEnBytesLog= %d\n", stat.tamEnBytesLog);
        fprintf(stderr,"stat.numBloquesOcupados= %d\n", stat.numBloquesOcupados);

        // Desmontamos el dispositivo
        bumount();
    }
}