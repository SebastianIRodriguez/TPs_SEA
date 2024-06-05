// Hola, MCU, estoy cambiando el archivo!!!
// HOooooooooolaaaaa
/* Copyright 2022, DSI FCEIA UNR - Sistemas Digitales 2
 *    DSI: http://www.dsi.fceia.unr.edu.ar/
 * Copyright 2017-2019, Gustavo Muro - Daniel Márquez
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
// Project Included Files
#include <stdio.h>
#include "SD2_board.h"
#include "fsl_lpuart.h"
#include "fsl_port.h"
#include "board.h"
#include "MKL43Z4.h"
#include "pin_mux.h"
#include "mma8451.h"
#include "SD2_I2C.h"
#include "rs485_drv.h"
#include "request_manager.h"
#include "ADC0.h"
#include "clock_config.h"
#include "fsl_debug_console.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

int main(void)
{
	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();

	#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
		/* Init FSL debug console. */
		BOARD_InitDebugConsole();
	#endif

	// Se inicializan funciones de la placa
	board_init();

	// Se inicializa el I2C y se inicia al sensor de aceleración
	SD2_I2C_init();
	mma8451_init();
	mma8451_setDataRate(DR_50hz);

	// Arrancamos el ADC0, conversión simple
	ADC0_begin();

	// Se inicializa driver de UART1
	requestManager_init();

	// Se inicializa interrupción de systick cada 1 ms
	SysTick_Config(SystemCoreClock / 1000U);

	while (1){}
}

void SysTick_Handler(void)
{
	static unsigned int i = 0;

	requestManager_detect_request();

	if (i == 1000)
	{
		i = 0;
		unsigned char buffer[21];

		// Obtiene el resultado en cuentas
		int light_value = ADC0_get(LIGHT_SENSOR_CHANNEL);

		// requestManager_detect_request();
		int16_t acc_x = mma8451_getAcX();
		int16_t acc_y = mma8451_getAcY();
		int16_t acc_z = mma8451_getAcZ();

		bool sw1 = board_getSw(BOARD_SW_ID_1);
		bool sw3 = board_getSw(BOARD_SW_ID_3);

		sprintf((char *)buffer, ":%04d%+04d%+04d%+04d%d%d\n", light_value, acc_x, acc_y, acc_z, sw1, sw3);
		rs485_drv_envDatos(buffer, 20);

		PRINTF("Luz: %d   Aceleraciones: %d,%d,%d   Sw1: %d   Sw3: %d\r\n", light_value, acc_x, acc_y, acc_z, sw1, sw3);
		//: LLLLAAABBBCCCS1S2'LF'
	}

	i++;
}

/*==================[end of file]============================================*/
