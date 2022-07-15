/*  AUTORES:

- Eloy Rafael
- Miquel Robles
- Soufyane Youbi
*/

#include "ficheros_basico.h"

int main(int argc, char **argv){
    char *s;
    bmount(argv[1]);

    for (size_t i = 0; i < atoi(argv[2]); i++)
    {   
        s = (char*)malloc(BLOCKSIZE*sizeof(char));
        bwrite(i,memset(s,'0',BLOCKSIZE));
    }

    initSB(atoi(argv[2]),atoi(argv[2])/4);
    initMB();
    initAI();
    
    reservar_inodo('d',7);
    
    /*
    traducir_bloque_inodo(i,8,1);
    traducir_bloque_inodo(i,204,1);
    traducir_bloque_inodo(i,30004,1);
    traducir_bloque_inodo(i,400004,1);
    traducir_bloque_inodo(i,468750,1);
    */
    bumount();
    
    return 1;
}