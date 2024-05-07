#include <stdlib.h>

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
        i++;
    }

    *(destiny + i) = '\0';

    return i;
}

/**
 * @brief Reemplaza todas las coincidencias de un substring determinado dentro de un string
 *
 * @param orig arreglo de origen
 * @param rep substring buscado
 * @param with substring con el que se reemplazarán las coincidencias de <rep>
 * @return longitud del arreglo resultante, negativo si hay error
 *
 */
int str_replace(char *orig, char *rep, char *with)
{
    char result[2000];    // arreglo auxiliar que acumulará el arreglo modificado
    char *ins;            // the next insert point
    char *tmp;            // varies
    char *inicial = orig; // respaldo del arreglo de origen
    int len_orig;
    int len_rep;          // Longitud del substring a remover
    int len_with;         // Longitud del string a agregar
    int len_front;        // distance between rep and end of last rep
    int count;            // number of replacements

    // Checkeos de seguridad
    if (!orig || !rep)
        return -1;

    len_rep = strlen(rep);
    
    if (len_rep == 0)
        return -2; // empty rep causes infinite loop during count
    
    if (!with)
        with = "";
    
    len_with = strlen(with);
    len_orig = strlen(orig);

    // Cuenta la cantidad de reemplazos requeridos
    ins = orig;
    for (count = 0; (tmp = strstr(ins, rep)); ++count)
    {
        ins = tmp + len_rep;
    }

    int final_len = strlen(orig) + (len_with - len_rep) * count + 1;

    tmp = result;

    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"

    while (count--)
    {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }

    if(orig != (inicial + len_orig))
        strcpy(tmp, orig);
    

    // Copiamos el arreglo auxiliar en el original
    strcpy(inicial, result);

    return final_len;
}