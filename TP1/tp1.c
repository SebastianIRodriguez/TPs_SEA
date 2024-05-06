#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#include "recoleccion.c"
#include "uart.c"

#define TTY_PATH "/dev/ttyS0"
const int MAX_COMMAND_LENGTH = 50;

enum COMMAND_TYPE
{
    HUMIDITY,
    CPU,
    NETWORK,
    ALL,
    EXIT,
    ERROR
} typedef COMMAND_TYPE;

COMMAND_TYPE interpret_command(char command[])
{
    if (strcmp(command, "humidity") == 0)
    {
        return HUMIDITY;
    }
    else if (strcmp(command, "cpu") == 0)
    {
        return CPU;
    }
    else if (strcmp(command, "network") == 0)
    {
        return NETWORK;
    }
    else if (strcmp(command, "all") == 0)
    {
        return ALL;
    }
    else if (strcmp(command, "q") == 0)
    {
        return EXIT;
    }
    return ERROR;
}

void process_command(int fd, COMMAND_TYPE command, char info_sensor[], char info_cpu[], Ethernet_info eth0) {

    char buf[2000];           // Buffer de usos múltiples

    switch (command)
    {
    case HUMIDITY:
        serial_write_ln(fd, "\n--- HUMEDAD [%] ---");
        serial_write(fd, info_sensor);
        serial_write(fd, "\n\n");
        break;

    case CPU:
        serial_write_ln(fd, "\n--- DATOS DE LA CPU ---");
        serial_write(fd, info_cpu);
        break;

    case NETWORK:
        sprintf(
            buf,
            "\n--- DATOS DE LA PLACA DE RED ---\n"
            "Interfaz: %s\n"
            "IP local: %s  MAC: %s\n"
            "Broadcast: %s  Mascara subred: %s  Gateway: %s\n"
            "IPv6: %s\n"
            "Rx packets: %s\n"
            "Tx packets: %s\n"
            "Rx bytes: %s\n"
            "Tx bytes: %s\n\n",
            eth0.interface, eth0.local_ip, eth0.mac, eth0.broadcast, eth0.mask, eth0.gateway, eth0.ipv6, eth0.rx_packets, eth0.tx_packets, eth0.rx_bytes, eth0.tx_bytes);
        serial_write(fd, buf);
        break;

    case ALL:
        sprintf(
            buf,
            "\n--- DATOS DE LA CPU ---\n"
            "%s"
            "\n--- DATOS DE LA PLACA DE RED ---\n"
            "Interfaz: %s\n"
            "IP local: %s  MAC: %s\n"
            "Broadcast: %s  Mascara subred: %s  Gateway: %s\n"
            "IPv6: %s\n"
            "Rx packets: %s\n"
            "Tx packets: %s\n"
            "Rx bytes: %s\n"
            "Tx bytes: %s\n"
            "\n--- HUMEDAD [%%] --- \n%s\n\n",
            info_cpu, eth0.interface, eth0.local_ip, eth0.mac, eth0.broadcast, eth0.mask, eth0.gateway, eth0.ipv6, eth0.rx_packets, eth0.tx_packets, eth0.rx_bytes, eth0.tx_bytes, info_sensor);
        serial_write(fd, buf);
        break;

    case EXIT:
        serial_write_ln(fd, "Fin de la transmision...");
        break;

    default:
        break;
    }
}

int main()
{
    char info_cpu[2000];      // Tendrá el contenido de "cpuinfo"
    char info_cpu_json[2000]; // Tendrá el contenido de "cpuinfo" para la página (distinto formato)
    char info_sensor[10];     // Tendrá el contenido de "sensor"
    Ethernet_info eth0;       // Estructura con los datos de la placa de red
    time_t tiempo_actual;     // Contiene el tiempo actual en segundos
    time_t tiempo_auxiliar;   // Variable auxiliar para coordinar la temporización de tareas

    char buff = 0;
    char cmd_input[MAX_COMMAND_LENGTH];
    int cmd_index = 0;

    int fd = open_tty_file(TTY_PATH);

    tiempo_actual = time(NULL);
    tiempo_auxiliar = tiempo_actual;
    while (1)
    {
        tiempo_actual = time(NULL);
        if (tiempo_actual != tiempo_auxiliar)
        {
            tiempo_auxiliar = tiempo_actual;

            // Obtenemos la info de la CPU
            get_cpu_data(info_cpu);

            // La copiamos y ponemos en formato adecuado para el json
            strcpy(info_cpu_json, info_cpu);
            str_replace(info_cpu_json, "\n", "\\n");
            str_replace(info_cpu_json, "\t", "\\t");

            // Obtenemos la info de la placa de red
            eth0 = get_ethernet_data();

            // Obtenemos la info del sensor
            get_sensor_data(info_sensor);

            // Actualizamos el .json para el html
            update_json(info_cpu_json, info_sensor, eth0);
        }

        int byte_count = read(fd, &buff, 1);
        if (byte_count == 1)
        {
            cmd_input[cmd_index] = buff;

            if (cmd_index == MAX_COMMAND_LENGTH)
                cmd_index = 0;

            if (cmd_input[cmd_index] == '\n')
            {
                cmd_input[cmd_index] = '\0';
                cmd_index = 0;

                COMMAND_TYPE command = interpret_command(cmd_input);
                process_command(fd, command, info_sensor, info_cpu, eth0);
                if (command == EXIT)
                {
                    return 0;
                }
                
            }
            else
            {
                cmd_index++;
            }
        }
    }
}