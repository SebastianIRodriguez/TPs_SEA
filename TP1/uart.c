#include <stdio.h>

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
}
