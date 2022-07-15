/*  AUTORES:

- Eloy Rafael
- Miquel Robles
- Soufyane Youbi
*/

#include "simulacion.h"
struct INFORMACION
{
   int pid;
   unsigned int nEscrituras; 
   struct REGISTRO PrimeraEscritura;
   struct REGISTRO UltimaEscritura;
   struct REGISTRO MenorPosicion;
   struct REGISTRO MayorPosicion;
};
void writeInformation(struct INFORMACION info, char *buffer);
