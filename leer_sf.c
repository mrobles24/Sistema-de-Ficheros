/*  AUTORES:

- Eloy Rafael
- Miquel Robles
- Soufyane Youbi
*/

#include "directorios.h"

void mostrar_buscar_entrada(char *camino, char reservar)
{
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;
    printf("\ncamino: %s, reservar: %d\n", camino, reservar);
    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, reservar, 6)) < 0)
    {
        mostrar_error_buscar_entrada(error);
    }
    printf("**********************************************************************\n");
    return;
}

int main(int argc, char **argv)
{
    struct superbloque SB;
    bmount(argv[1]);

    if (bread(0, &SB) == -1)
    {
        perror("Error");
        return -1;
    }

    // Imprimimos datos del superbloque
    fprintf(stderr, "\nDATOS DEL SUPERBLOQUE:\n");
    fprintf(stderr, "posPrimerBloqueMB = %d\n", SB.posPrimerBloqueMB);
    fprintf(stderr, "posUltimoBloqueMB = %d\n", SB.posUltimoBloqueMB);
    fprintf(stderr, "posPrimerBloqueAI = %d\n", SB.posPrimerBloqueAI);
    fprintf(stderr, "posUltimoBloqueAI = %d\n", SB.posUltimoBloqueAI);
    fprintf(stderr, "posPrimerBloqueDatos = %d\n", SB.posPrimerBloqueDatos);
    fprintf(stderr, "posUltimoBloqueDatos = %d\n", SB.posUltimoBloqueDatos);
    fprintf(stderr, "posInodoRaiz = %d\n", SB.posInodoRaiz);
    fprintf(stderr, "posPrimerInodoLibre = %d\n", SB.posPrimerInodoLibre);
    fprintf(stderr, "cantBloquesLibres = %d\n", SB.cantBloquesLibres);
    fprintf(stderr, "cantInodosLibres = %d\n", SB.cantInodosLibres);
    fprintf(stderr, "totBloques = %d\n", SB.totBloques);
    fprintf(stderr, "totInodos = %d\n\n", SB.totInodos);

    bumount();

    return 0;
}