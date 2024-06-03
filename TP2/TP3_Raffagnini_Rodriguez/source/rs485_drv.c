/* Copyright 2022, DSI FCEIA UNR - Sistemas Digitales 2
 *    DSI: http://www.dsi.fceia.unr.edu.ar/
 * Copyright 2023, 2022, 2019, 2018, Gustavo Muro - Daniel Márquez - Guido Cicconi
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

// Standard C Included Files
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

// Project Included Files
#include "SD2_board.h"
#include "fsl_lpuart.h"
#include "fsl_port.h"
#include "fsl_lpuart_dma.h"
#include "fsl_dmamux.h"
#include "board.h"
#include "MKL43Z4.h"
#include "pin_mux.h"
#include "rs485_drv.h"
#include "ringBuffer.h"

/*==================[macros and definitions]=================================*/
#define LPUART_TX_DMA_CHANNEL 1U

#define RS485_UART                  LPUART1
#define RS485_UART_IRQn             LPUART1_IRQn

#define TX1_BUFFER_DMA_SIZE  32
#define RX1_RINGBUFFER_SIZE  32

/*==================[internal data declaration]==============================*/
static uint8_t txBuffer_dma[TX1_BUFFER_DMA_SIZE];
static lpuart_dma_handle_t LPUARTDmaHandle;
static dma_handle_t LPUARTTxDmaHandle;
volatile bool tx485OnGoing = false;
static void* pRingBufferRx;

/*==================[internal functions declaration]=========================*/

/*==================[internal data declaration]==============================*/
static const board_gpioInfo_type board_gpioContLine[] =
{
    {PORTE, GPIOE, 29},    /* RE */
    {PORTE, GPIOE, 30},    /* DE */
};

/*==================[internal functions definition]==========================*/

/* UART user callback */
static void LPUART_UserCallback(LPUART_Type *base, lpuart_dma_handle_t *handle, status_t status, void *userData)
{
    if (kStatus_LPUART_TxIdle == status)
    {
        tx485OnGoing = false;
    }
}

static void rs485_RE(bool est)
{
    if (est)
    	GPIO_PortSet(board_gpioContLine[0].gpio, 1<<board_gpioContLine[0].pin);
    else
    	GPIO_PortClear(board_gpioContLine[0].gpio, 1<<board_gpioContLine[0].pin);
}

static void rs485_DE(bool est)
{
    if (est)
    	GPIO_PortSet(board_gpioContLine[1].gpio, 1<<board_gpioContLine[1].pin);
    else
    	GPIO_PortClear(board_gpioContLine[1].gpio, 1<<board_gpioContLine[1].pin);
}

/*==================[external functions definition]==========================*/

void rs485_drv_init(void)
{

	// ****  Buffer circular de recepción
    pRingBufferRx = ringBuffer_init(RX1_RINGBUFFER_SIZE);

    // ****  Pines RE/DE
	int32_t i;
	gpio_pin_config_t gpio_config =
	{
		.outputLogic = 1,
		.pinDirection = kGPIO_DigitalOutput,
	};

	const port_pin_config_t port_config = {
			/* Internal pull-up/down resistor is disabled */
		.pullSelect = kPORT_PullDisable,
		/* Slow slew rate is configured */
		.slewRate = kPORT_SlowSlewRate,
		/* Passive filter is disabled */
		.passiveFilterEnable = kPORT_PassiveFilterDisable,
		/* Low drive strength is configured */
		.driveStrength = kPORT_LowDriveStrength,
		/* Pin is configured as PTC3 */
		.mux = kPORT_MuxAsGpio,
	};

	lpuart_config_t config;

    /* inicialización de pines de control */
    for (i = 0 ; i < 2 ; i++)
	{
		PORT_SetPinConfig(board_gpioContLine[i].port, board_gpioContLine[i].pin, &port_config);
		GPIO_PinInit(board_gpioContLine[i].gpio, board_gpioContLine[i].pin, &gpio_config);
	}

    rs485_RE(false);
    rs485_DE(false);

    /*
	 * config.baudRate_Bps = 115200U;
	 * config.parityMode = kUART_ParityDisabled;
	 * config.stopBitCount = kUART_OneStopBit;
	 * config.txFifoWatermark = 0;
	 * config.rxFifoWatermark = 1;
	 * config.enableTx = false;
	 * config.enableRx = false;
	 */
	LPUART_GetDefaultConfig(&config);
	config.baudRate_Bps = 115200;
	config.enableTx = true;
	config.enableRx = true;

	LPUART_Init(RS485_UART, &config, CLOCK_GetFreq(SYS_CLK));

	/* Configura los pines RX y TX de la UART1 */
	PORT_SetPinMux(PORTE, 0U, kPORT_MuxAlt3);
	PORT_SetPinMux(PORTE, 1U, kPORT_MuxAlt3);

    LPUART_EnableInterrupts(RS485_UART, kLPUART_RxDataRegFullInterruptEnable);
    LPUART_EnableInterrupts(RS485_UART, kLPUART_TransmissionCompleteInterruptEnable);
    LPUART_EnableInterrupts(RS485_UART, kLPUART_RxOverrunFlag);
    NVIC_EnableIRQ(LPUART1_IRQn);

    /***************** CONFIGURACIÓN DMA (sólo para TX) *******************/
    /* Init DMAMUX */
    DMAMUX_Init(DMAMUX0);

    /* Set channel for LPUART  */
    DMAMUX_SetSource(DMAMUX0, LPUART_TX_DMA_CHANNEL, kDmaRequestMux0LPUART1Tx);
    DMAMUX_EnableChannel(DMAMUX0, LPUART_TX_DMA_CHANNEL);

    /* Init the DMA module */
    DMA_Init(DMA0);
    DMA_CreateHandle(&LPUARTTxDmaHandle, DMA0, LPUART_TX_DMA_CHANNEL);

    /* Create LPUART DMA handle. */
    LPUART_TransferCreateHandleDMA(
            RS485_UART,
            &LPUARTDmaHandle,
            LPUART_UserCallback,
            NULL,
            &LPUARTTxDmaHandle,
            NULL);
}

int32_t rs485_drv_available_bytes_to_read() {
	return ringBuffer_getCount(pRingBufferRx);
}

/** \brief recibe datos por puerto serie accediendo al RB
 **
 ** \param[inout] pBuf buffer a donde guardar los datos
 ** \param[in] size tamaño del buffer
 ** \param[in] buffer_offset posicion en el buffer a partir de la cual coloca los datos recibidos
 ** \return cantidad de bytes recibidos
 **/
int32_t rs485_drv_recDatos(uint8_t *pBuf, int32_t size, int32_t buffer_offset)
{
    int32_t ret = 0;

    /* entra sección de código crítico */
    NVIC_DisableIRQ(LPUART1_IRQn);

    while (!ringBuffer_isEmpty(pRingBufferRx) && ret < size)
    {
        ringBuffer_getData(pRingBufferRx, &pBuf[ret + buffer_offset]);
        ret++;
    }

    /* sale de sección de código crítico */
    NVIC_EnableIRQ(LPUART1_IRQn);

    return ret;
}


/** \brief envía datos por puerto serie accediendo a memoria RAM
 **
 ** \param[inout] pBuf buffer a donde estan los datos a enviar
 ** \param[in] size tamaño del buffer
 ** \return cantidad de bytes enviados
 **/

int32_t rs485_drv_envDatos(uint8_t *pBuf, int32_t size)
{
    lpuart_transfer_t xfer;

    if (tx485OnGoing)
    {
        size = 0;
    }
    else
    {
        /* limita size */
        if (size > TX1_BUFFER_DMA_SIZE)
            size = TX1_BUFFER_DMA_SIZE;

        // Hace copia del buffer a transmitir en otro arreglo
        memcpy(txBuffer_dma, pBuf, size);

        rs485_RE(true);
        rs485_DE(true);

        xfer.data = txBuffer_dma;
        xfer.dataSize = size;

        tx485OnGoing = true;

        LPUART_TransferSendDMA(RS485_UART, &LPUARTDmaHandle, &xfer);
        LPUART_EnableInterrupts(RS485_UART, kLPUART_TransmissionCompleteInterruptEnable);
    }

    return size;
}


void LPUART1_IRQHandler(void)
{
	uint8_t data;

	// **** Recepción
    if ( (kLPUART_RxDataRegFullFlag)            & LPUART_GetStatusFlags(RS485_UART) &&
         (kLPUART_RxDataRegFullInterruptEnable) & LPUART_GetEnabledInterrupts(RS485_UART) )
	{
        /* obtiene dato recibido por puerto serie */
	    data = LPUART_ReadByte(RS485_UART);

		/* pone dato en ring buffer */
		ringBuffer_putData(pRingBufferRx, data);
	}

    // **** Transmisión
    if ( (kLPUART_TransmissionCompleteFlag)            & LPUART_GetStatusFlags(RS485_UART) &&
         (kLPUART_TransmissionCompleteInterruptEnable) & LPUART_GetEnabledInterrupts(RS485_UART) )
    {
        LPUART_DisableInterrupts(RS485_UART, kLPUART_TransmissionCompleteInterruptEnable);
        LPUART_ClearStatusFlags(RS485_UART, kLPUART_TransmissionCompleteFlag);
        rs485_RE(false);
        rs485_DE(false);
    }

    // **** Overrun
    if(kLPUART_RxOverrunFlag & LPUART_GetStatusFlags(LPUART1))
    	LPUART_ClearStatusFlags(LPUART1, kLPUART_RxOverrunFlag);

}

/*==================[end of file]============================================*/

