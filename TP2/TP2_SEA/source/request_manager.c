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
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "fsl_lpuart.h"
#include "fsl_port.h"
#include "SD2_board.h"
#include "rs485_drv.h"
#include "mma8451.h"
/*==================[macros and definitions]=================================*/

#define LED_ACTION_MESSAGE '0'
#define SW_STATE_MESSAGE '1'
#define ACCEL_STATE_MESSAGE '2'

#define LED_RED_MESSAGE_ID '1'
#define LED_GREEN_MESSAGE_ID '2'

#define SW1_MESSAGE_ID '1'
#define SW3_MESSAGE_ID '3'

#define LF 0x0A

#define BUFFER_SIZE 18

/**
 *  LED ROJO
 *  :XX01Y’LF’ :XX01Y’LF’   Y = E - A - T
 *  LED VERDE
 *  :XX02Y’LF’ :XX02Y’LF’   Y = E - A - T
 *  XX = '23'
 **/

void requestManager_init()
{
    rs485_drv_init();
    mma8451_init();
}

void requestManager_process_led_action_request(uint8_t *buffer, uint8_t received_message_id)
{

    if(received_message_id != LED_RED_MESSAGE_ID && received_message_id != LED_GREEN_MESSAGE_ID)
    	return;

    board_ledId_enum requested_led_id = (received_message_id == LED_RED_MESSAGE_ID) ? BOARD_LED_ID_ROJO : BOARD_LED_ID_VERDE;

    char requested_led_action = buffer[3]; //'E', 'A' o 'T'
    switch (requested_led_action)
    {
    case 'E':
        board_setLed(requested_led_id, BOARD_LED_MSG_ON);
        break;

    case 'A':
        board_setLed(requested_led_id, BOARD_LED_MSG_OFF);
        break;

    case 'T':
        board_setLed(requested_led_id, BOARD_LED_MSG_TOGGLE);
        break;

    default: // Si se envia otro caracter, no respondo el pedido
        return;
    }

    // Envio el mensaje tal cual lo recibi
    rs485_drv_envDatos(buffer, 5);
}

/**
 * :XX11’LF’ :XX11Y’LF’   Y = N - P
 * :XX13’LF’ :XX13Y’LF’   Y = N - P
 * XX = '23'
 */
void requestManager_process_switch_state_request(uint8_t *buffer, uint8_t received_message_id)
{
    uint8_t requested_sw_state = 0;

    switch (received_message_id)
    {
    case SW1_MESSAGE_ID:
        requested_sw_state = board_getSw(BOARD_SW_ID_1);
        break;

    case SW3_MESSAGE_ID:
        requested_sw_state = board_getSw(BOARD_SW_ID_3);
        break;

    default: // Si se envia otro numero de switch, no respondo el pedido
        return;
    }

    buffer[3] = (requested_sw_state) ? 'P' : 'N';
    buffer[4] = 0x0A;

    rs485_drv_envDatos(buffer, 5);
}

/**
 * :XX21’LF’ :XX21SXXXSYYYSZZZ’LF’
 * XX = '23'
 */
void requestManager_process_acceleration_request(uint8_t *buffer, uint8_t received_message_id)
{

    if (received_message_id != '1') // Si no se envia el codigo correcto, no respondo el pedido
        return;

    int accX = mma8451_getAcX();
    int accY = mma8451_getAcY();
    int accZ = mma8451_getAcZ();

    sprintf((char *)buffer, ":%+04d%+04d%+04d\n", accX, accY, accZ);

    rs485_drv_envDatos(buffer, 18);
}

void requestManager_process_request(uint8_t *buffer, int32_t request_length)
{
    // Verifico que al menos el mensaje leido tenga la longitud del header + el terminador osea :XXZZ'LF'
    // Verifico que el identificador sea el correcto
    if (request_length < 4 || buffer[0] != ':')
        return;

    uint8_t received_message_type = buffer[1];
    uint8_t received_message_id = buffer[2];

    switch (received_message_type)
    {
    case LED_ACTION_MESSAGE:

        if (request_length != 5)
            return;

        requestManager_process_led_action_request(buffer, received_message_id);

        break;

    case SW_STATE_MESSAGE:

        if (request_length != 4)
            return;

        requestManager_process_switch_state_request(buffer, received_message_id);

        break;

    case ACCEL_STATE_MESSAGE:

        if (request_length != 4)
            return;

        requestManager_process_acceleration_request(buffer, received_message_id);

        break;

    default:
        break;
    }
}

void requestManager_detect_request()
{
    int32_t ret = 0;
    static uint8_t buffer[BUFFER_SIZE];
    static uint8_t buffer_index = 0;

    if (rs485_drv_available_bytes_to_read() > 0)
    {
        if (buffer_index < BUFFER_SIZE)
        {
            ret = rs485_drv_recDatos(buffer, 1, buffer_index);
            if (ret == 1)
            {
                if (buffer[buffer_index] == ':')
                { // Detecto interrupcion de la trama actual y comienzo de una nueva
                    buffer_index = 0;
                    buffer[0] = ':';
                }
                else if (buffer[buffer_index] == LF)
                {
                    requestManager_process_request(buffer, buffer_index + 1); 
                    buffer_index = -1;
                }
                buffer_index++;
            }
        }
        else
        {
            buffer_index = 0;
        }
    }
}

/*==================[end of file]============================================*/
