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

typedef struct {
    char local_ip[16];
    char gateway[16];
    char broadcast[16];
    char mask[16];
    char mac[20];
    char ipv6[60];
    char rx_packets[200];
    char tx_packets[200];
    char rx_bytes[100];
    char tx_bytes[100];
    char interface[20];
} Ethernet_info;

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

/**
 * 
*/
int get_cpu_data(char *info_cpu)
{
    char ruta_cpuinfo[] = "/proc/cpuinfo";          // Contiene la información de la CPU
    int bytes_leidos = 0;

    // Leemos la información de cpuinfo
    bytes_leidos = read_file(ruta_cpuinfo, info_cpu);
    
    if(0 < bytes_leidos)
    {
        printf("Se pudo leer el archivo %s.\n Longitud: %d\n Contenido: \n%s\n", ruta_cpuinfo, bytes_leidos, info_cpu);
    }
    else
    {
        printf("La lectura del archivo %s fallo con error: %d\n", ruta_cpuinfo, bytes_leidos);
    }

    return bytes_leidos;
}

int main(void)
{
    char info_cpu[30] = "a\ta\ta\ta\t";            // Tendrá el contenido de "cpuinfo"
    char info_cpu_json[30];       // Tendrá el contenido de "cpuinfo" para la página (distinto formato)
    char info_sensor[10];           // Tendrá el contenido de "sensor"
    Ethernet_info eth0;             // Estructura con los datos de la placa de red

    // Obtenemos la info de la CPU
    //get_cpu_data(info_cpu);

    // La copiamos y ponemos en formato adecuado para el json
    strcpy(info_cpu_json, info_cpu);
    str_replace(info_cpu_json, "\t", "\\t");

    char prueba[200] = "CPU revision	: 4\nCPU revision	: 4\nCPU revision	: 4\n\n";
    char *p = prueba;
    char *q = info_cpu_json;
    printf("Direccion de info_cpu: %u\n",q);
    printf("Puntero: %u, arreglo original: %s\n", p, prueba);
    str_replace(prueba, "\n", "\\t");
    p = prueba;
    printf("Puntero: %u, arreglo editado: %s\n", p, prueba);
    str_replace(prueba, "\t", "\\n");
    p = prueba;
    printf("Puntero: %u, arreglo editado: %s\n", p, prueba);


    return 0;
}
