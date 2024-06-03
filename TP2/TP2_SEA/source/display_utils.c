/* Copyright  DSI FCEIA UNR - Sistemas Digitales 2
 *    DSI: http://www.dsi.fceia.unr.edu.ar/
 *  Diego Alegrechi
 *  Gustavo Muro
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*==================[inclusions]=============================================*/
#include "display_utils.h"
#include "oled.h"
#include <stdio.h>
#include "mma8451.h"
#include "pato_lucas.h"
#include "SD2_board.h"

/*==================[macros and definitions]=================================*/

#define IMAGE_X 20
#define IMAGE_Y 0

/*==================[internal data declaration]==============================*/

static const uint32_t periodMs = 500;
static uint32_t tim_imgUpdate = 1;

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/


void displayUtils_print_image(uint8_t x0, uint8_t y0, unsigned char* image, uint8_t width_in_px, uint8_t height_in_px)
{
    int width_in_bytes = width_in_px / 8;
    int array_length = width_in_px * height_in_px / 8;
    
    int row_pos = 0;

    //oled_clearScreen(OLED_COLOR_BLACK);

    for (int i = 0; i < array_length; i++) {
        
        //Avanzar a la fila inferior del display
        if ((i % width_in_bytes) == 0 && i > 0) {
            row_pos++;
        }

        //Imprimir 8 pixeles
        //unsigned char img_segment = image[i];
        for (int k = 7; k >= 0; k--)
        {
            char pixel_value = (image[i] >> k) & 0x01;
            oled_color_t pixel_color = (pixel_value) ? OLED_COLOR_WHITE : OLED_COLOR_BLACK;
            
            uint8_t bit_offset = (7 - k);
            uint8_t segment_offset = (i % width_in_bytes) * 8;
            uint8_t x = x0 + segment_offset + bit_offset;

            uint8_t y = y0 + row_pos;
            
            oled_putPixel(x, y, pixel_color);
        }
    }
}

void displayUtils_print_accel(int accX, int accY, int accZ) {
    char completo[30];

    //sprintf(completo, "X: %d Y: %d Z: %d", accX, accY, accZ);
    sprintf(completo, "X: % 4d", accX);
    oled_putString(83, 8, (uint8_t*) completo, OLED_COLOR_WHITE, OLED_COLOR_BLACK);

    sprintf(completo, "Y: % 4d", accY);
    oled_putString(83, 18, (uint8_t*) completo, OLED_COLOR_WHITE, OLED_COLOR_BLACK);

    sprintf(completo, "Z: % 4d", accZ);
    oled_putString(83, 28, (uint8_t*) completo, OLED_COLOR_WHITE, OLED_COLOR_BLACK);

    if (accX > 35) {
        displayUtils_print_image(IMAGE_X + 16, 15, ojos_derecha, 24, 24);
    }
    else if(accX < -35) {
        displayUtils_print_image(IMAGE_X + 16, 15, ojos_izquierda, 24, 24);
    }
    else if(accY > 35) {
        displayUtils_print_image(IMAGE_X + 16, 15, ojos_arriba, 24, 24);
    }
    else if(accY < -35) {
        displayUtils_print_image(IMAGE_X + 16, 15, ojos_abajo, 24, 24);
    }
    else {
        displayUtils_print_image(IMAGE_X + 16, 15, ojos_centro, 24, 24);
    }
    
}

void displayUtils_init() {
	// Se inicializa comunicacion a traves del modulo SPI1
	board_configSPI1();

	// Se inicializa el modulo OLED
	oled_init();
	oled_setContrast(16);
	oled_clearScreen(OLED_COLOR_BLACK);
}



void displayUtils_show_base_image() {
    displayUtils_print_image(IMAGE_X, IMAGE_Y, pato_lucas, 64, 56);
}


void displayUtils_periodicTask1ms() {

    tim_imgUpdate--;

    if(tim_imgUpdate <= 0) {
        tim_imgUpdate = periodMs;

        int16_t accX = mma8451_getAcX();
        int16_t accY = mma8451_getAcY();
        int16_t accZ = mma8451_getAcZ();

        displayUtils_print_accel(accX, accY, accZ);
    }
}
/*==================[end of file]============================================*/
