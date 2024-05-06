#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define TTY_PATH "/dev/ttyS0"
const int MAX_COMMAND_LENGTH = 50;


enum COMMAND_TYPE { HUMIDITY, CPU, NETWORK, ALL, EXIT, ERROR } typedef COMMAND_TYPE;

COMMAND_TYPE process_command(char command[])
{
    if (strcmp(command, "humidity") == 0)
    {
        printf("Me pidieron la humedad\n");
        return HUMIDITY;
    }
    else if (strcmp(command, "cpu") == 0)
    {
        printf("Me pidieron la cpu\n");
        return CPU;
    }
    else if (strcmp(command, "network") == 0)
    {
        printf("Me pidieron la red\n");
        return NETWORK;
    }
    else if (strcmp(command, "all") == 0)
    {
        printf("Me pidieron TODO\n");
        return ALL;
    }
    else if (strcmp(command, "q") == 0)
    {
        printf("Me tomo el buque\n");
        return EXIT;
    }
    
    printf("Comando no reconocido\n");
    return ERROR;
}

int open_tty_file(char path[]) {
    int fd = open(path, O_RDONLY);

    //Config file as Non-Blocking
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    return fd;
}

int main()
{
    char buff = 0;
    char cmd_input[MAX_COMMAND_LENGTH];
    int cmd_index = 0;
    
    int fd = open_tty_file(TTY_PATH);
    while (1)
    {
        int byte_count = read(fd, &buff, 1);
        if (byte_count == 1)
        {
            cmd_input[cmd_index] = buff;
            
            if(cmd_index == MAX_COMMAND_LENGTH)
                cmd_index = 0;

            if (cmd_input[cmd_index] == '\n')
            {
                cmd_input[cmd_index] = '\0';
                cmd_index = 0;

                printf("\nProcesando comando...\n");
                COMMAND_TYPE command = process_command(cmd_input);
                switch (command)
                {
                case EXIT:
                    return 0;
                
                default:
                    break;
                }
            }
            else {
                cmd_index++;
            }
        }
    }
}


/*int main()
{
    char buff = 0;
    char command[MAX_COMMAND_LENGTH];
    int cmd_index = 0;
    
    int fd = open_tty_file(TTY_PATH);
    while (1)
    {
        int byte_count = read(fd, &buff, 1);
        if (byte_count == 1)
        {
            command[cmd_index++] = (buff != '\n') ? buff : '\0';
            cmd_index = (cmd_index < MAX_COMMAND_LENGTH) ? cmd_index : 0;

            if (command[cmd_index-1] == '\0')
            {
                command[cmd_index - 1] = '\0';
                printf("\nProcesando comando...\n");

                process_command(command);
                cmd_index = 0;
            }

            printf("%c", buff);
        }
    }
}*/

// while (1) {
// La funcion iocontrol que tiene unas macros que te tiran todo

// Leer datos del sensor de humedad
// Leer datos del cpu
// Leer datos de la placa de red

// Actualizar archivo json con toda esta informacion (de un solo saque)

// Enviar informacion por el puerto serie

// Esperar x tiempo
//}

/*#include <stdio.h>
#include <unistd.h>

int main() {
    FILE* fichero = fopen("/dev/ttyS0", "r");
    if (fichero == NULL) {
        printf("El fichero no se ha podido abrir para lectura.\n");
        return -1;
    }


    printf("Archivo abierto");

    while(1) {
        if (feof(fichero))
        {
            printf("No hay nada para leer");
        }
        else {
            char c;
            printf("Leyendo un byte");
            int byte_count = read(&c, sizeof(char), 1);
            if(byte_count == 0) {
                printf("Fallo la lectura\n");
            }
            else {
                printf("%c", c);
            }
        }
    }

    if (fclose(fichero)!=0) {
        printf("No se ha podido cerrar el fichero.\n");
        return -1;
    }
}*/