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
#include <stdio.h>

#include "mma8451.h"
#include "uart0_drv.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/
static unsigned int tim_imgUpdate = 110;
static unsigned int tim_animacion3D = 1;

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/
void animacion3D_init(int ms)
{
    // Se inicializa driver de UART0
    uart0_drv_init();
    mma8451_init();

    if (ms > 99)
        tim_imgUpdate = ms;
}

void animacion3D_setPeriod(int ms)
{
    if (ms > 99)
        tim_imgUpdate = ms;
}

void animacion3D_periodicTask1ms()
{
    tim_animacion3D--;
    if (tim_animacion3D == 0)
    {
        tim_animacion3D = tim_imgUpdate;
        int accX = mma8451_getAcX();
        int accY = mma8451_getAcY();
        int accZ = mma8451_getAcZ();
        char mensaje[15];
        sprintf(mensaje, "%d %d %d\n", accX, accY, accZ);
        uart0_drv_envDatos((uint8_t *)mensaje, strlen(mensaje));
    }
}
/*==================[end of file]============================================*/
