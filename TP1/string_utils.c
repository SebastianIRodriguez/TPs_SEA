/**
 * @file String_utils.c
 * @author Raffagnini - Rodriguez
 * @brief Aca se implementan funciones para el manejo de los strings, de manera tal de permitir:
 * - Extraer facilmente un substring delimitado por una palabra clave y un caracter en específico
 * - Reemplazar un substring por otro dentro de un arreglo
 * @version 0.1
 * @date 2024-05-05
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <stdlib.h>
#include "string_utils.h"

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
    char *match;                        // Puntero a la primera posición del match
    char *aux;                          // Puntero auxiliar
    int i = 0;                          // La mejor variable del mundo!
    int match_len = strlen(match_exp);  // Longitud del string de match

    // Buscamos a match_exp en origin
    match = strstr(origin, match_exp);
    if (match == NULL)
        return -1;
    
    // Encontramos match de la palabra clave!
    aux = match + match_len;

    // Leemos la expresión deseada hasta el caracter terminador
    while (*(aux + i) != end_c)
    {
        *(destiny + i) = *(aux + i);
        i++;
    }

    *(destiny + i) = '\0';

    return i;
}

/**
 * @brief Reemplaza todas las coincidencias de un substring determinado dentro de un string
 * Atención, esta función utiliza memoria dinámica! 
 *
 * @param orig arreglo de origen
 * @param rep substring buscado
 * @param with substring con el que se reemplazarán las coincidencias de <rep>
 * @return longitud del arreglo resultante, negativo si hay error
 *
 */
int str_replace(char *orig, char *rep, char *with)
{
    // Punteros
    char *result;         // Acumulará el arreglo modificado
    char *match;          // Apunta a la próxima ocurrecia del substring a reemplazar en el arreglo original
    char *aux;            // Puntero auxiliar para múltiples tareas
    char *inicial = orig; // Respalda la ubicación del primer caracter del arreglo de origen
    
    // Enteros
    int len_orig;   // Longitud del arreglo original
    int len_rep;    // Longitud del substring a remover
    int len_with;   // Longitud del string a agregar
    int len_front;  // Distancia entre rep y la última ocurrecia del substring en el arreglo de origen
    int count;      // Cantidad de reemplzos a ejecutar

    // Checkeos de seguridad
    if (!orig || !rep)
        return -1;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return -2; // empty rep causes infinite loop during count
    if (!with)
        with = "";

    // Tomamos las longitudes de los arreglos
    len_with = strlen(with);
    len_orig = strlen(orig);

    // Cuenta la cantidad de reemplazos requeridos
    match = orig;
    for (count = 0; (aux = strstr(match, rep)); ++count)
    {
        match = aux + len_rep;
    }

    // Reservamos la memoria necesaria
    int final_len = strlen(orig) + (len_with - len_rep) * count + 1;
    result = malloc(final_len);

    if(result == NULL)
        return -3;

    aux = result;

    // <aux> apunta al final del arreglo modificado (result)
    // <match> apunta a la próxima ocurrencia del substringa a reemplazar en <orig>
    // <orig> apunta al inicio del array de origen (primer iteración) o a la posición siguiente 
    // a la última ocurrencia de <match> dentro de del array original.

    while (count--)
    {
        match = strstr(orig, rep);
        len_front = match - orig;
        aux = strncpy(aux, orig, len_front) + len_front;
        aux = strcpy(aux, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }

    // Copiamos lo que falte de arreglo, de hacer falta
    if(orig != (inicial + len_orig))
        strcpy(aux, orig);
    
    // Copiamos el arreglo auxiliar en el original
    strcpy(inicial, result);

    // Liberamos la memoria reservada
    free(result);

    return final_len;
}