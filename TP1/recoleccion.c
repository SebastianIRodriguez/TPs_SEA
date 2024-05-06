/**
 * @file recoleccion.c
 * @author your name (you@domain.com)
 * @brief Aca se implementan cosas para leer los datos de interés, estos son:
 * - Info de la CPU
 * - Info de la placa de red
 * - Humedad
 * @version 0.1
 * @date 2024-05-05
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Lee el contenido de un archivo (en formato texto)
 *
 * @param filename ruta al archivo en cuestión
 * @param content arreglo donde se guarda el contenido del archivo
 * @return int bytes leídos, negativo si hay error
 */
int read_file(const char *filename, char *content)
{
    FILE *fichero;
    int i = 0;
    int resultado;
    char c;

    // Intentamos abrir el archivo
    fichero = fopen(filename, "r");
    if (fichero == NULL)
    {
        printf("El archivo no se ha podido abrir para lectura.\n");
        return -1;
    }

    // Se lee la info del archivo
    while (!feof(fichero))
    {
        resultado = fread(&c, sizeof(char), 1, fichero);
        if (resultado != 1)
        {
            break;
        }
        content[i++] = c;
    }
    content[i] = 0;

    if (ferror(fichero) != 0)
    {
        printf("Ha ocurrido algún error en la lectura del archivo %s\n", filename);
    }
    else
    {
        //printf("La informacion del sistema es:\n%s\n", content);
    }

    // Intentamos cerrar el archivo
    if (fclose(fichero) != 0)
    {
        printf("No se ha podido cerrar el archivo %s\n", filename);
        return -2;
    }
    return i;

}

/**
 * @brief Extrae la información de un arreglo
 *
 * @param origin arreglo de origen
 * @param match_exp palabra clave de la busqueda
 * @param end_c caracter que delimita la obtención de información
 * @param destiny arreglo de destino para la información
 * @return int bytes leídos, negativo si hay error
*/
int read_match(const char *origin, const char *match_exp, const char end_c, char *destiny)
{
    char *match;    // Puntero a la primera posición del match
    char *aux;      // Puntero auxiliar
    int match_len;  // Longitud del string de match
    int len = 0;    // Longitud de la información leída
    int i = 0;      // La mejor variable del mundo!

    match_len = strlen(match_exp);
    
    // Buscamos a match_exp en origin
    match =  strstr(origin, match_exp);
    if(match == NULL)
    {
        printf("Fallo al encontrar la expresion: %s\n", match_exp);
        return -1;
    }

    // Encontramos match de la palabra clave!
    aux = match + match_len;

    // Leemos la expresión deseada hasta el caracter terminador
    while (*(aux + i) != end_c)
    {
        *(destiny + i) = *(aux + i);
        i ++;
    }
    
    *(destiny + i) = 0;

    return i; 
}

int main(void)
{
    char nombre_fichero[] = "/proc/cpuinfo";
    char ruta_ethernet[] = "/tp1/eth_info.txt";
    char bash_code[] = "#!/bin/bash \n echo \"Hola, Luchin! Voy a tomar tu info de red, sopermi\" \n";
    char bc_read_net[] = "#!/bin/bash \n ifconfig eth0 > /tp1/eth_info.txt";
    char info_cpu[500];
    char info_ethernet[600];
    char local_ip[20];
    int bytes_leidos = 0;

    bytes_leidos = read_file(nombre_fichero,info_cpu);

    if(0 < bytes_leidos)
    {
        printf("Se pudo leer el archivo %s.\n Longitud: %d\n Contenido: \n%s\n", nombre_fichero, bytes_leidos, info_cpu);
    }
    else
    {
        printf("La lectura del archivo %s fallo con error: %d\n", nombre_fichero, bytes_leidos);
    }

    // Leemos la info de la placa de red (ethernet)
    system(bash_code);
    system(bc_read_net);

    // Guardamos la información en un arreglo
    bytes_leidos = read_file(ruta_ethernet, info_ethernet);

    if(0 < bytes_leidos)
    {
        printf("Se pudo leer el archivo %s.\n Longitud: %d\n Contenido: \n%s\n", ruta_ethernet, bytes_leidos, info_ethernet);
    }
    else
    {
        printf("La lectura del archivo %s fallo con error: %d\n", ruta_ethernet, bytes_leidos);
    }

    // Intento guardar la ip local
    bytes_leidos = read_match(info_ethernet, "inet addr:", ' ', local_ip);

    if(0 < bytes_leidos)
    {
        printf("Se pudo leer la ip local.\n Longitud: %d\n Contenido: \n%s\n", bytes_leidos, local_ip);
    }
    else
    {
        printf("Fallo con error: %d\n", bytes_leidos);
    }

    return 0;
}
  
  //printf( "Se leyo de NUMERO_PRUEBA: %d\n", numero);
  //printf( "Se leyo de STRING_PRUEBA: %s\n\n", getenv( "STRING_PRUEBA" ) );
