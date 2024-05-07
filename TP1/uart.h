/**
 * @file uart.h
 * @author Raffagnini - Rodriguez
 * @brief Aquí se encuentran declaradas las funciones que se definen en "uart.c"
 * @version 0.1
 * @date 2024-05-05
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef UART_H
#define UART_H

int open_tty_file(char path[]);
int serial_write(int fd, char data[]);
int serial_write_ln(int fd, char data[]);
int serial_read(int fd, char *data, int ammount);

#include "uart.c"
#endif

