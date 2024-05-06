#include <fcntl.h>
#include <unistd.h>

int open_tty_file(char path[])
{
    int fd = open(path, O_RDWR);

    // Config file as Non-Blocking
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    return fd;
}

int serial_write(int fd, char data[])
{
    int data_size = strlen(data);
    return write(fd, data, data_size);
}

int serial_write_ln(int fd, char data[])
{
    return serial_write(fd, data) + serial_write(fd, "\n");
}

/*
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
            int byte_count = fread(&c, sizeof(char), 1, fichero);
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