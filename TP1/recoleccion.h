/**
 * @file recoleeccion.h
 * @author Raffagnini - Rodriguez
 * @brief Aquí se encuentran declaradas las funciones que se definen en "recoleccion.c"
 * @version 0.1
 * @date 2024-05-05
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef RECOLECCION_H
#define RECOLECCION_H

typedef struct
{
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

int read_file(const char *filename, char *content);
int get_cpu_data(char *info_cpu);
Ethernet_info get_ethernet_data();
int get_sensor_data(char *info_sensor);
int update_json(char *info_cpu_json, char *info_sensor, Ethernet_info eth0);

#include "recoleccion.c"
#endif