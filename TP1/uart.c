#include <fcntl.h>
#include <unistd.h>
#include "uart.h"

/**
 * @brief Permite abrir el archivo de lectura/escritura del puerto serie
 * en modo no bloqueante (el programa no se "clava" si no hay nada para leer)
 *
 * @param path ruta al archivo
 * @return int referencia al archivo
 */
int open_tty_file(char path[])
{
    int fd = open(path, O_RDWR);

    // Config file as Non-Blocking
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    return fd;
}

/**
 * @brief Imprime un arreglo de caracteres en el puerto serie 
 *
 * @param fd referencia al archivo de control del puerto serie
 * @param data arreglo a imprimir
 * @return int bytes escritos
 */
int serial_write(int fd, char data[])
{
    int data_size = strlen(data);
    return write(fd, data, data_size);
}

/**
 * @brief Imprime un arreglo de caracteres en el puerto serie y agrega un \n al final
 *
 * @param fd referencia al archivo de control del puerto serie
 * @param data arreglo a imprimir
 * @return int bytes escritos
 */
int serial_write_ln(int fd, char data[])
{
    return serial_write(fd, data) + serial_write(fd, "\n");
}

/**
 * @brief Intenta leer lo recibido por el puerto serie
 *
 * @param fd referencia al archivo de control del puerto serie
 * @param data arreglo que contendrá los bytes leídos
 * @param ammount cantidad de bytes que se pretende leer
 * @return int bytes leídos
 */
int serial_read(int fd, char *data, int ammount)
{
    return read(fd, data, ammount);
}
