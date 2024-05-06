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

int main(void)
{
    // Rutas a los archivos de interes
    char ruta_cpuinfo[] = "/proc/cpuinfo";          // Contiene la información de la CPU
    char ruta_sensor[] = "/tp1_files/htu21_humidity";             // Contiene la información del sensor de humedad
    char ruta_ethernet[] = "/tp1_files/eth_info.txt";     // Contendrá la información extraída del ifconfig
    char ruta_gateway[] = "/tp1_files/route.txt";         // Contrendrá la información extraída de route
    char ruta_html[] = "/tp1_files/data.json";         // Ruta al archivo json
  
    // Comandos bash
    char bc_read_net[] = "#!/bin/bash \n ifconfig eth0 > /tp1_files/eth_info.txt";    // Solicitamos y guardamos la info de la placa de red
    char bc_read_gateway[] = "#!/bin/bash \n route > /tp1_files/route.txt";           // Solicitamos y guardamos la info del comando route
    
    // Otras variables
    char info_cpu[5000];         // Tendrá el contenido de "cpuinfo"
    char info_sensor[10];       // Tendrá el contenido de "sensor"
    char info_ethernet[6000];    // Tendrá el contenido de "eth_info.txt"
    char info_gateway[3000];     // Tendrá el contenido de "route.txt"
    int bytes_leidos = 0;       // Variable auxiliar
    Ethernet_info eth0;         // Estructura con los datos de la placa de red
    FILE *datos_html;           // Puntero al archivo json

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

    // Leemos la info de la placa de red (ethernet)
    system(bc_read_net);
    system(bc_read_gateway);

    // Guardamos la información en arreglos
    bytes_leidos = read_file(ruta_ethernet, info_ethernet);

    if(0 < bytes_leidos)
    {
        printf("Se pudo leer el archivo %s.\n Longitud: %d\n Contenido: \n%s\n", ruta_ethernet, bytes_leidos, info_ethernet);
    }
    else
    {
        printf("La lectura del archivo %s fallo con error: %d\n", ruta_ethernet, bytes_leidos);
    }

    bytes_leidos = read_file(ruta_gateway, info_gateway);

    if(0 < bytes_leidos)
    {
        printf("Se pudo leer el archivo %s.\n Longitud: %d\n Contenido: \n%s\n", ruta_gateway, bytes_leidos, info_gateway);
    } 
    else
    {
        printf("La lectura del archivo %s fallo con error: %d\n", ruta_gateway, bytes_leidos);
    }
    
    // Guardamos los datos
    read_match(info_ethernet, "Link encap:", ' ', eth0.interface);      // Tecnología de la interfaz
    read_match(info_ethernet, "inet addr:", ' ', eth0.local_ip);        // IP local
    read_match(info_gateway, "default         ", ' ', eth0.gateway);    // Gateway
    read_match(info_ethernet, "Bcast:", ' ', eth0.broadcast);           // Broadcast
    read_match(info_ethernet, "Mask:", '\n', eth0.mask);                // Mask
    read_match(info_ethernet, "inet6 addr: ", ' ', eth0.ipv6);          // IPv6
    read_match(info_ethernet, "HWaddr ", '\n', eth0.mac);               // MAC
    read_match(info_ethernet, "RX packets:", '\n', eth0.rx_packets);    // rx packets
    read_match(info_ethernet, "TX packets:", '\n', eth0.tx_packets);    // tx packets
    read_match(info_ethernet, "RX bytes:", 'T', eth0.rx_bytes);         // rx bytes
    read_match(info_ethernet, "TX bytes:", '\n', eth0.tx_bytes);        // tx bytes

    printf("Info de la placa de red %s: \n"
    "ip local: %s  gateway: %s  broadcast: %s  mascara: %s  MAC: %s\n"
    "ipv6: %s\n"
    "Rx packets: %s\n"
    "Tx packets: %s\n"
    "Rx bytes: %s\n"
    "Tx bytes: %s\n", eth0.interface, eth0.local_ip, eth0.gateway, eth0.broadcast, eth0.mask, eth0.mac, eth0.ipv6, eth0.rx_packets, eth0.tx_packets, eth0.rx_bytes, eth0.tx_bytes);

    // Recolectamos la info del sensor
    read_file(ruta_sensor, info_sensor);

    printf("El sensor dice que hay %s porciento de humedad\n", info_sensor);

    // Pasamos los datos al servidor web:
    datos_html = fopen(ruta_html, "w");
    if (datos_html == NULL)
    {
        printf("El archivo no se ha podido abrir para escritura.\n");
        return -1;
    }

    fprintf(
        datos_html,
        "{\n"
        "\"humidity\":  \"%s\",\n"
        "\"cpu_info\":  \"%s\",\n"
        "\"interface\": \"%s\",\n"
        "\"ip local\":  \"%s\",\n"
        "\"gateway\":   \"%s\",\n"
        "\"broadcast\": \"%s\",\n"
        "\"mascara\":   \"%s\",\n"
        "\"mac\":       \"%s\",\n"
        "\"ipv6\":      \"%s\",\n"
        "\"rx packets\":\"%s\",\n"
        "\"tx packets\":\"%s\",\n"
        "\"rx bytes\":  \"%s\",\n"
        "\"tx bytes\":  \"%s\"\n"
        "}",
        info_sensor, info_cpu, eth0.interface, eth0.local_ip, eth0.gateway, eth0.broadcast, eth0.mask, eth0.mac, eth0.ipv6, eth0.rx_packets, eth0.tx_packets, eth0.rx_bytes, eth0.tx_bytes
    );

    // Intentamos cerrar el archivo
    if (fclose(datos_html) != 0)
    {
        printf("No se ha podido cerrar el archivo %s\n", ruta_html);
        return -2;
    }

    return 0;
}
  /*{
    "humidity": "85 %",
    "cpu_info": "processor:0\nBogoMIPS: 38.40\nFeatures:fp asimd evtstrm crc32 cpuid\nCPU implementer: 0x41\nCPU architecture: 8\nCPU variant: 0x0\nCPU part: 0xd03CPU revision : 4",
    "network_info": "eth0\nLink encap:Ethernet  HWaddr B8:27:EB:00:88:83\ninet addr:192.168.56.3  Bcast:192.168.56.255  Mask:255.255.255.0\ninet6 addr: fe80::ba27:ebff:fe00:8883/64 Scope:Link\nUP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1\nRX packets:5208 errors:0 dropped:0 overruns:0 frame:0\nTX packets:1291 errors:0 dropped:0 overruns:0 carrier:0\ncollisions:0 txqueuelen:1000\nRX bytes:2274174 (2.1 MiB)  TX bytes:231025 (225.6 KiB)"
}*/
  //printf( "Se leyo de NUMERO_PRUEBA: %d\n", numero);
  //printf( "Se leyo de STRING_PRUEBA: %s\n\n", getenv( "STRING_PRUEBA" ) );
  /**
   *     // Intento guardar la ip local
    bytes_leidos = read_match(info_ethernet, "inet addr:", ' ', eth0.local_ip);

    if(0 < bytes_leidos)
    {
        printf("Se pudo leer la ip local.\n Longitud: %d\n Contenido: \n%s\n", bytes_leidos, eth0.local_ip);
    }
    else
    {
        printf("Fallo con error: %d\n", bytes_leidos);
    }
  */
