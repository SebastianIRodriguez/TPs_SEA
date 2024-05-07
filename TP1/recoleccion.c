/**
 * @file recoleccion.c
 * @author Raffagnini - Rodriguez
 * @brief Aca se implementan funciones para leer los datos de interés, estos son:
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
#include "recoleccion.h"
#include "string_utils.h"

/**
 * @brief Lee el contenido de un archivo (en formato texto)
 *
 * @param filename ruta al archivo en cuestión
 * @param content arreglo donde se guarda el contenido del archivo
 * @return int bytes leídos, negativo si hay error
 */
int read_file(const char *filename, char *content)
{
    int i = 0;
    int resultado;
    char c;
    FILE *fichero = fopen(filename, "r"); // Intentamos abrir el archivo

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
    content[i] = '\0';

    // Intentamos cerrar el archivo
    if (fclose(fichero) != 0)
    {
        printf("No se ha podido cerrar el archivo %s\n", filename);
        return -2;
    }
    return i;
}

/**
 * @brief Obtiene y copia en un arreglo la información de la cpu del dispositivo
 *
 * @param info_cpu arreglo donde se guardará la información
 * @return int bytes leídos, negativo si hay error
 */
int get_cpu_data(char *info_cpu)
{
    char ruta_cpuinfo[] = "/proc/cpuinfo"; // Contiene la información de la CPU
    int bytes_leidos = read_file(ruta_cpuinfo, info_cpu);

    return bytes_leidos;
}

/**
 * @brief Obtiene los datos de la placa de red del equipo y los devuelve en
 * una correspondiente estructura de datos
 *
 * @return Ethernet_info información de la placa
 */
Ethernet_info get_ethernet_data()
{
    Ethernet_info eth0;
    int bytes_leidos;

    // Arreglos
    char info_ethernet[2000]; // Tendrá el contenido de "eth_info.txt"
    char info_gateway[2000];  // Tendrá el contenido de "route.txt"

    // Rutas
    char ruta_ethernet[] = "/tp1_files/eth_info.txt"; // Contendrá la información extraída del ifconfig
    char ruta_gateway[] = "/tp1_files/route.txt";     // Contrendrá la información extraída de route

    // Comandos bash
    char bc_read_net[] = "#!/bin/bash \n ifconfig eth0 > /tp1_files/eth_info.txt"; // Solicitamos y guardamos la info de la placa de red
    char bc_read_gateway[] = "#!/bin/bash \n route > /tp1_files/route.txt";        // Solicitamos y guardamos la info del comando route

    // Leemos la info de la placa de red (ethernet)
    system(bc_read_net);
    system(bc_read_gateway);

    read_file(ruta_ethernet, info_ethernet);
    read_file(ruta_gateway, info_gateway);

    // Guardamos los datos
    read_match(info_ethernet, "Link encap:", ' ', eth0.interface);   // Tecnología de la interfaz
    read_match(info_ethernet, "inet addr:", ' ', eth0.local_ip);     // IP local
    read_match(info_gateway, "default         ", ' ', eth0.gateway); // Gateway
    read_match(info_ethernet, "Bcast:", ' ', eth0.broadcast);        // Broadcast
    read_match(info_ethernet, "Mask:", '\n', eth0.mask);             // Mask
    read_match(info_ethernet, "inet6 addr: ", ' ', eth0.ipv6);       // IPv6
    read_match(info_ethernet, "HWaddr ", '\n', eth0.mac);            // MAC
    read_match(info_ethernet, "RX packets:", '\n', eth0.rx_packets); // rx packets
    read_match(info_ethernet, "TX packets:", '\n', eth0.tx_packets); // tx packets
    read_match(info_ethernet, "RX bytes:", 'T', eth0.rx_bytes);      // rx bytes
    read_match(info_ethernet, "TX bytes:", '\n', eth0.tx_bytes);     // tx bytes

    return eth0;
}

/**
 * @brief Obtiene y copia en un arreglo la información del sensor de humedad
 *
 * @param info_sensor arreglo donde se guardará la información
 * @return int bytes leídos, negativo si hay error
 */
int get_sensor_data(char *info_sensor)
{
    char ruta_sensor[] = "/tp1_files/htu21_humidity"; // Contiene la información del sensor de humedad
    int bytes_leidos;

    info_sensor[0] = '\0';

    bytes_leidos = read_file(ruta_sensor, info_sensor);

    if (bytes_leidos > 0)
    {
        // El formato de respuesta es xxx.yyy\n\0, donde la cantidad de 'x' depende del número
        for (int i = bytes_leidos - 1; bytes_leidos - 4 < i; i--)
            info_sensor[i] = info_sensor[i - 1];

        info_sensor[bytes_leidos - 4] = '.';
    }

    return bytes_leidos;
}

/**
 * @brief Actualiza la información del archivo json del servidor
 *
 * @param info_cpu_json arreglo en formato adecuado que contiene la información del cpu
 * @param info_sensor arreglo que contiene la información del sensor de humedad
 * @param eth0 estructura que contiene la información de la placa de red
 * @return int  si hubo éxito, negativo si presentó problemas
 */
int update_json(char *info_cpu_json, char *info_sensor, Ethernet_info eth0)
{
    char ruta_json[] = "/var/www/data.json";    // Ruta al archivo json
    FILE *datos_html = fopen(ruta_json, "w");   // Puntero al archivo json

    if (datos_html == NULL)
    {
        printf("El archivo no se ha podido abrir para escritura.\n");
        return -1;
    }

    // Copiamos la informacion en el json:
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
        info_sensor, info_cpu_json, eth0.interface, eth0.local_ip, eth0.gateway, eth0.broadcast, eth0.mask, eth0.mac, eth0.ipv6, eth0.rx_packets, eth0.tx_packets, eth0.rx_bytes, eth0.tx_bytes);

    // Intentamos cerrar el archivo
    if (fclose(datos_html) != 0)
    {
        printf("No se ha podido cerrar el archivo %s\n", ruta_json);
        return -2;
    }

    return 0;
}