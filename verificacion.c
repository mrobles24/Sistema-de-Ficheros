/*  AUTORES:

- Eloy Rafael
- Miquel Robles
- Soufyane Youbi
*/

#include "verificacion.h"

/* main:
   Se recorre el fichero "prueba.dat" de cada proceso, generando:

   ● Proceso: escribir el PID
   ● Número de escrituras: escribir el contador de los registros validados dentro del
   fichero "prueba.dat" (se validan verificando que el campo PID coincida, ya que
   podríamos haber leído basura)
   ● Primera escritura: escribir la de no de escritura menor
   ● Última escritura: escribir la de no de escritura mayor
   ● Menor posición: escribir la de posición (no registro) más baja
   ● Mayor posición: escribir la de posición (no registro) más alta

   El objetivo es imprimir por pantalla los cuatro registros más significativos que cada proceso ha escrito en su fichero. 
   Para cada escritura se mostrará el número de escritura, el no de registro y la fecha. 

*/
int main(int argc, char const *argv[])
{
   char nombre_disco[1024], camino[1024], camino_fichero[1024], prueba[1024], buffer_info[3000], *ret;
   int numentradas, offset;
   int cant_registros_buffer_escrituras = 256;

   struct STAT stat;
   struct INFORMACION info;
   struct entrada buffer_entradas[NUMPROCESOS];
   struct REGISTRO buffer_escrituras[cant_registros_buffer_escrituras];

   // Comprobamos la sintaxis
   if (argc != 3)
   {
      fprintf(stderr, "Formato válido: verificacion <nombre_dispositivo> <directorio_simulación>\n");
      exit(-1);
   }

   strcpy(nombre_disco, argv[1]);
   if (access(nombre_disco, F_OK) == -1)
   {
      perror("Error:");
      exit(-1);
   }

   // Montamos el dispositivo
   bmount(nombre_disco);
   strcpy(camino, argv[2]);

   // Imprimimos ruta de directorio de simulacion 
   fprintf(stderr,"dir_sim:%s\n", camino);

   // Calculamos el nº de entradas del directorio de simulación a partir del stat de su inodo
   mi_stat(camino, &stat);
   if (camino[strlen(camino) - 1] != '/')
   {
      perror("Error:");
      exit(-1);
   }
   numentradas = stat.tamEnBytesLog / sizeof(struct entrada);

   // Imprimimos numero de entradas
   fprintf(stderr,"numentradas: %d NUMPROCESOS: %d\n", numentradas, NUMPROCESOS);
   if (numentradas != NUMPROCESOS)
      return -1;

   // Creamos el fichero "informe.txt" dentro del directorio de simulación
   sprintf(camino_fichero, "%sinforme.txt", camino);
   mi_creat(camino_fichero, 6);

   // Leemos los directorios correspondientes a los procesos
   mi_read(camino, buffer_entradas, 0, NUMPROCESOS * sizeof(struct entrada));
   for (int i = 0; i < numentradas; i++)
   {
      info.nEscrituras = 0;
      ret = strchr(buffer_entradas[i].nombre, '_');
      if (!ret)
      {
         perror("Error");
         exit(-1);
      }
      memset(&info, 0, sizeof(struct INFORMACION));
      info.pid = atoi(ret + 1);

      // Recorrido secuencial del fichero prueba.dat
      offset = 0;
      memset(prueba, 0, sizeof(prueba));
      sprintf(prueba, "%s%s/prueba.dat", camino, buffer_entradas[i].nombre);
      memset(buffer_escrituras, 0, NUMPROCESOS * sizeof(struct REGISTRO));

      while (mi_read(prueba, buffer_escrituras, offset, sizeof(buffer_escrituras)) > 0 && info.nEscrituras < NUMESCRITURAS)
      {
         // Para cada registro
         for (int j = 0; j < cant_registros_buffer_escrituras; j++)
         {
            // Si la escritura es válida entonces
            if (buffer_escrituras[j].pid == info.pid) 
            {
               // Primera escritura
               if (info.nEscrituras == 0)
               {
                  info.PrimeraEscritura = buffer_escrituras[j];
                  info.UltimaEscritura = buffer_escrituras[j];
                  info.MenorPosicion = buffer_escrituras[j];
                  info.MayorPosicion = buffer_escrituras[j];
               }
               // No es la primera escritura
               else
               {
                  // Es escritura anterior
                  if (difftime(buffer_escrituras[j].fecha, info.PrimeraEscritura.fecha) < 0)
                  {
                     info.PrimeraEscritura = buffer_escrituras[j];
                  }
                  else if (buffer_escrituras[j].nEscritura < info.PrimeraEscritura.nEscritura)
                  {
                     info.PrimeraEscritura = buffer_escrituras[j];
                  }

                  // Es escritura posterior
                  if (difftime(buffer_escrituras[j].fecha, info.UltimaEscritura.fecha) > 0)
                  {
                     info.UltimaEscritura = buffer_escrituras[j];
                  }
                  else if (buffer_escrituras[j].nEscritura > info.UltimaEscritura.nEscritura)
                  {
                     info.UltimaEscritura = buffer_escrituras[j];
                  }
               }

               // Incrementamos contador de escrituras
               info.nEscrituras++;
            }
         }

         // Vaciamos el buffer y incrementamos el offset
         offset += sizeof(buffer_escrituras);
         memset(buffer_escrituras, 0, sizeof(buffer_escrituras));
      }

      // Obtener la escritura de la última posición
      mi_stat(prueba, &stat);
      mi_read(prueba, &info.MayorPosicion, stat.tamEnBytesLog - sizeof(struct REGISTRO), sizeof(struct REGISTRO));

      // Añadir la información al fichero informe.txt por el final con funcion auxiliar
      mi_stat(camino_fichero, &stat);
      writeInformation(info, buffer_info);
      mi_write(camino_fichero, buffer_info, stat.tamEnBytesLog, strlen(buffer_info));

      if (DEBUGN13)
      {
         fprintf(stderr,"[%d)\t%d escrituras validadas en %s]\n", i + 1, info.nEscrituras, prueba);
      }
      
   }
   // Desmontamos dispositivo
   bumount();
}

/* writeInformation:
   Se encarga de escribir toda la información en el fichero
*/
void writeInformation(struct INFORMACION info, char *buffer)
{
   char buffer_secundario[250];

   // Mostramos PID
   sprintf(buffer, "PID:%d\n", info.pid);
   sprintf(buffer_secundario, "Numero de escrituras:%d\n", info.nEscrituras);
   strcat(buffer, buffer_secundario);
   memset(buffer_secundario, 0, strlen(buffer_secundario));

   // Mostramos primera escritura
   sprintf(buffer_secundario, "Primera Escritura\t%d\t%d\t%s", info.PrimeraEscritura.nEscritura, info.PrimeraEscritura.nRegistro,
           asctime(localtime(&info.PrimeraEscritura.fecha)));
   strcat(buffer, buffer_secundario);
   memset(buffer_secundario, 0, strlen(buffer_secundario));

   // Mostramos ultima escritura
   sprintf(buffer_secundario, "Ultima Escritura\t%d\t%d\t%s", info.UltimaEscritura.nEscritura, info.UltimaEscritura.nRegistro,
           asctime(localtime(&info.UltimaEscritura.fecha)));
   strcat(buffer, buffer_secundario);
   memset(buffer_secundario, 0, strlen(buffer_secundario));

   // Mostramos menor posición
   sprintf(buffer_secundario, "Menor Posición\t\t%d\t%d\t%s", info.MenorPosicion.nEscritura, info.MenorPosicion.nRegistro,
           asctime(localtime(&info.MenorPosicion.fecha)));
   strcat(buffer, buffer_secundario);
   memset(buffer_secundario, 0, strlen(buffer_secundario));

   // Mostramos mayor posición
   sprintf(buffer_secundario, "Mayor Posición\t\t%d\t%d\t%s\n", info.MayorPosicion.nEscritura, info.MayorPosicion.nRegistro,
           asctime(localtime(&info.MayorPosicion.fecha)));
   strcat(buffer, buffer_secundario);
}
