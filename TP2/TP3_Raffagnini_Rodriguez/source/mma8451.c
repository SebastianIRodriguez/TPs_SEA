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
#include "mma8451.h"
#include "fsl_i2c.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "fsl_debug_console.h"

/*==================[macros and definitions]=================================*/
#define MMA8451_I2C_ADDRESS (0x1d)

#define INT1_PORT PORTC
#define INT1_GPIO GPIOC
#define INT1_PIN 5

#define INT2_PORT PORTD
#define INT2_GPIO GPIOD
#define INT2_PIN 1

// ***** Direcciones
#define STATUS_ADDRESS 0X00
#define INT_SOURCE_ADDRESS 0X0C
#define FF_MT_CONFIG_ADDRESS 0x15
#define FF_MT_SRC_ADDRESS 0x16
#define FF_MT_THRESHOLD_ADDRESS 0x17
#define FF_MT_COUNT_ADDRESS 0x18
#define CTRL_REG1_ADDRESS 0X2A
#define CTRL_REG4_ADDRESS 0X2D
#define CTRL_REG5_ADDRESS 0X2E

// 1. Register 0x15 FF/MT Config - Motion/Freefall Configuration
// 2. Register 0x17 FF_MT_THS - Setting the Threshold
// 3. Register 0x18 FF_MT_COUNT - Setting the Debounce Counter
// 4. Register 0x16 FF_MT_SRC - Motion/Freefall Source Detection
typedef union
{
    struct
    {
        unsigned SRC_DRDY : 1;
        unsigned : 1;
        unsigned SRC_FF_MT : 1;
        unsigned SRC_PULSE : 1;
        unsigned SRC_LNDPRT : 1;
        unsigned SRC_TRANS : 1;
        unsigned SRC_FIFO : 1;
        unsigned SRC_ASLP : 1;
    };
    uint8_t data;
} INT_SOURCE_t;

typedef union
{
    struct
    {
        unsigned XDR : 1;
        unsigned YDR : 1;
        unsigned ZDR : 1;
        unsigned ZYXDR : 1;
        unsigned XOW : 1;
        unsigned YOW : 1;
        unsigned ZOW : 1;
        unsigned ZYXOW : 1;
    };
    uint8_t data;
} STATUS_t;

typedef union
{
    struct
    {
        unsigned ACTIVE : 1;
        unsigned F_READ : 1;
        unsigned LNOISE : 1;
        unsigned DR : 3;
        unsigned ASLP_RATE : 2;
    };
    uint8_t data;
} CTRL_REG1_t;

typedef union
{
    struct
    {
        unsigned INT_EN_DRDY : 1;
        unsigned : 1;
        unsigned INT_EN_FF_MT : 1;
        unsigned INT_EN_PULSE : 1;
        unsigned INT_EN_LNDPRT : 1;
        unsigned INT_EN_TRANS : 1;
        unsigned INT_EN_FIFO : 1;
        unsigned INT_EN_ASLP : 1;
    };
    uint8_t data;
} CTRL_REG4_t;

typedef union
{
    struct
    {
        unsigned INT_CFG_DRDY : 1;
        unsigned : 1;
        unsigned INT_CFG_FF_MT : 1;
        unsigned INT_CFG_PULSE : 1;
        unsigned INT_CFG_LNDPRT : 1;
        unsigned INT_CFG_TRANS : 1;
        unsigned INT_CFG_FIFO : 1;
        unsigned INT_CFG_ASLP : 1;
    };
    uint8_t data;
} CTRL_REG5_t;

// Registro para configurar si el modo es detección de movimiento o caída libre
typedef union
{
    struct
    {
        unsigned : 1;
        unsigned : 1;
        unsigned : 1;
        unsigned XEFE : 1;
        unsigned YEFE : 1;
        unsigned ZEFE : 1;
        unsigned OAE : 1;
        unsigned ELE : 1;
    };
    uint8_t data;
} FF_MT_CONFIG_t;

// Registro para configurar el umbral de caída libre, 127 cuentas con 1 cuenta =  0.063g/LSB
typedef union
{
    struct
    {
        unsigned threshold : 7;
        unsigned DBCNTM : 1;
    };
    uint8_t data;
} FF_MT_THRESHOLD_t;

// Configura un filtro pasa-bajos para eliminar falsos positivos de FF
typedef union
{
    uint8_t count;
    uint8_t data;
} FF_MT_COUNT_t;

// Indica si se detectó caída libre
typedef union
{
    // EA se pone el 1 si se detectó el evento, leer el registro limpia la bandera
    struct
    {
        unsigned XHP : 1;
        unsigned XHE : 1;
        unsigned YHP : 1;
        unsigned YHE : 1;
        unsigned ZHP : 1;
        unsigned ZHE : 1;
        unsigned : 1;
        unsigned EA : 1;
    };
    uint8_t data;
} FF_MT_SRC_t;

/*==================[internal data declaration]==============================*/
static int16_t readX, readY, readZ;

/*==================[internal functions declaration]=========================*/
static uint8_t mma8451_read_reg(uint8_t addr)
{
    i2c_master_transfer_t masterXfer;
    uint8_t ret;

    memset(&masterXfer, 0, sizeof(masterXfer)); // pone todo en cero (sizeof())
    masterXfer.slaveAddress = MMA8451_I2C_ADDRESS;
    masterXfer.direction = kI2C_Read;
    masterXfer.subaddress = addr;
    masterXfer.subaddressSize = 1;
    masterXfer.data = &ret;
    masterXfer.dataSize = 1;
    masterXfer.flags = kI2C_TransferDefaultFlag;

    I2C_MasterTransferBlocking(I2C0, &masterXfer);

    return ret;
}

static void mma8451_read_multi_reg(uint8_t addr, unsigned int size, uint8_t *answer)
{
    i2c_master_transfer_t masterXfer;
    // uint8_t ret;

    memset(&masterXfer, 0, sizeof(masterXfer)); // pone todo en cero (sizeof())
    masterXfer.slaveAddress = MMA8451_I2C_ADDRESS;
    masterXfer.direction = kI2C_Read;
    masterXfer.subaddress = addr;
    masterXfer.subaddressSize = 1;
    masterXfer.data = answer;
    masterXfer.dataSize = size;
    masterXfer.flags = kI2C_TransferDefaultFlag;

    I2C_MasterTransferBlocking(I2C0, &masterXfer);

    // return ret;
}

static void mma8451_write_reg(uint8_t addr, uint8_t data)
{
    i2c_master_transfer_t masterXfer;

    memset(&masterXfer, 0, sizeof(masterXfer));

    masterXfer.slaveAddress = MMA8451_I2C_ADDRESS;
    masterXfer.direction = kI2C_Write;
    masterXfer.subaddress = addr;
    masterXfer.subaddressSize = 1;
    masterXfer.data = &data;
    masterXfer.dataSize = 1;
    masterXfer.flags = kI2C_TransferDefaultFlag;

    I2C_MasterTransferBlocking(I2C0, &masterXfer);
}

/*==================[internal data definition]===============================*/
static bool inited = false;
static bool is_falling = false;

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void config_port_int1(void)
{
    const port_pin_config_t port_int1_config = {
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
    const gpio_pin_config_t gpio_int1_config = {
        .pinDirection = kGPIO_DigitalInput,
        .outputLogic = 0U};

    PORT_SetPinConfig(INT1_PORT, INT1_PIN, &port_int1_config);
    GPIO_PinInit(INT1_GPIO, INT1_PIN, &gpio_int1_config);

    /* Interrupt polarity active high, or active low. Default value: 0.
       0: Active low; 1: Active high. VER REGISTRO CTRL_REG3 */
    PORT_SetPinInterruptConfig(INT1_PORT, INT1_PIN, kPORT_InterruptLogicZero);

    NVIC_EnableIRQ(PORTC_PORTD_IRQn);
    NVIC_SetPriority(PORTC_PORTD_IRQn, 0);
}

/*==================[external functions definition]==========================*/
void mma8451_init(void)
{
    if (!inited)
    {
        CTRL_REG1_t ctrl_reg1;
        CTRL_REG4_t ctrl_reg4;
        CTRL_REG5_t ctrl_reg5;
        FF_MT_CONFIG_t ff_mt_config;
        FF_MT_THRESHOLD_t ff_mt_threshold;
        FF_MT_COUNT_t ff_mt_count;

        /* Primero desactivo el acelerómetro, luego escribo otros registros*/

        ctrl_reg1.ACTIVE = 0;
        ctrl_reg1.F_READ = 0;
        ctrl_reg1.LNOISE = 1;
        ctrl_reg1.DR = 0B011; // ODR (Output Data Rate)  (011) - 100 Hz
        ctrl_reg1.ASLP_RATE = 0B00;
        // ctrl_reg1.data = 0x20;
        mma8451_write_reg(CTRL_REG1_ADDRESS, ctrl_reg1.data);

        // Configuro la detección de caída libre
        ff_mt_config.ELE = 1;
        ff_mt_config.OAE = 0;
        ff_mt_config.XEFE = 1;
        ff_mt_config.YEFE = 1;
        ff_mt_config.ZEFE = 1;
        mma8451_write_reg(FF_MT_CONFIG_ADDRESS, ff_mt_config.data);

        // Configuro el umbral de detección de caída libre a < 0.2G
        ff_mt_threshold.threshold = 3; // (3 cuentas * /0.063g)
        ff_mt_threshold.DBCNTM = 0;
        mma8451_write_reg(FF_MT_THRESHOLD_ADDRESS, ff_mt_threshold.data);

        // Configuro el debouncer/ filtro PB
        ff_mt_count.count = 12; //  12 cuentas o 120 ms a 100Hz ODR
        mma8451_write_reg(FF_MT_COUNT_ADDRESS, ff_mt_count.data);

        // Habilito interrupciones
        ctrl_reg4.INT_EN_DRDY = 1;  // Data ready
        ctrl_reg4.INT_EN_FF_MT = 0; // Caída libre
        ctrl_reg4.INT_EN_PULSE = 0;
        ctrl_reg4.INT_EN_LNDPRT = 0;
        ctrl_reg4.INT_EN_TRANS = 0;
        ctrl_reg4.INT_EN_FIFO = 0;
        ctrl_reg4.INT_EN_ASLP = 0;
        mma8451_write_reg(CTRL_REG4_ADDRESS, ctrl_reg4.data);

        // Ruteo las interrupciones por data ready y por caída libre al pin INT1
        ctrl_reg5.INT_CFG_DRDY = 1;
        ctrl_reg5.INT_CFG_FF_MT = 1;
        ctrl_reg5.INT_CFG_PULSE = 0;
        ctrl_reg5.INT_CFG_LNDPRT = 0;
        ctrl_reg5.INT_CFG_TRANS = 0;
        ctrl_reg5.INT_CFG_FIFO = 0;
        ctrl_reg5.INT_CFG_ASLP = 0;
        mma8451_write_reg(CTRL_REG5_ADDRESS, ctrl_reg5.data);

        // Pongo en active el dispositivo
        ctrl_reg1.data = mma8451_read_reg(CTRL_REG1_ADDRESS);
        ctrl_reg1.data |= 0x01;
        mma8451_write_reg(CTRL_REG1_ADDRESS, ctrl_reg1.data);

        config_port_int1();

    	// Se setea el DR
    	mma8451_setDataRate(DR_12p5hz);
    }
    inited = true;
}

void mma8451_setDataRate(DR_enum rate)
{
    CTRL_REG1_t ctr_reg1;
    bool estAct;

    /* antes de modificar data rate es necesario poner ACTIVE = 0 */
    ctr_reg1.data = mma8451_read_reg(CTRL_REG1_ADDRESS);

    /* guarda valor que tiene ACTIVE y luego pone a cero */
    estAct = ctr_reg1.ACTIVE;
    ctr_reg1.ACTIVE = 0;

    mma8451_write_reg(CTRL_REG1_ADDRESS, ctr_reg1.data);

    /* actualiza DR y en la misma escritura va a restaurar ACTIVE */
    ctr_reg1.DR = rate;
    ctr_reg1.ACTIVE = estAct;

    mma8451_write_reg(CTRL_REG1_ADDRESS, ctr_reg1.data);
}

void mma8451_enable_DR_IRQ()
{
    // dr_irq_enable = true;
    CTRL_REG1_t ctr_reg1;
    CTRL_REG4_t ctr_reg4;
    bool estAct;

    // antes de modificar es necesario poner ACTIVE = 0
    ctr_reg1.data = mma8451_read_reg(CTRL_REG1_ADDRESS);

    // guarda valor que tiene ACTIVE y luego pone a cero
    estAct = ctr_reg1.ACTIVE;
    ctr_reg1.ACTIVE = 0;
    mma8451_write_reg(CTRL_REG1_ADDRESS, ctr_reg1.data);

    // Leo en contenido del registro 4 y habilito la IRQ por DR
    ctr_reg4.data = mma8451_read_reg(CTRL_REG4_ADDRESS);
    ctr_reg4.INT_EN_DRDY = 1;
    mma8451_write_reg(CTRL_REG4_ADDRESS, ctr_reg4.data);

    // Vuelvo a poner active como estaba
    ctr_reg1.ACTIVE = estAct;
    mma8451_write_reg(CTRL_REG1_ADDRESS, ctr_reg1.data);
}

void mma8451_disable_DR_IRQ()
{
    CTRL_REG1_t ctr_reg1;
    CTRL_REG4_t ctr_reg4;
    bool estAct;

    // antes de modificar es necesario poner ACTIVE = 0
    ctr_reg1.data = mma8451_read_reg(CTRL_REG1_ADDRESS);

    // guarda valor que tiene ACTIVE y luego pone a cero
    estAct = ctr_reg1.ACTIVE;
    ctr_reg1.ACTIVE = 0;
    mma8451_write_reg(CTRL_REG1_ADDRESS, ctr_reg1.data);

    // Leo en contenido del registro 4 y habilito la IRQ por DR
    ctr_reg4.data = mma8451_read_reg(CTRL_REG4_ADDRESS);
    ctr_reg4.INT_EN_DRDY = 0;
    mma8451_write_reg(CTRL_REG4_ADDRESS, ctr_reg4.data);

    // Vuelvo a poner active como estaba
    ctr_reg1.ACTIVE = estAct;
    mma8451_write_reg(CTRL_REG1_ADDRESS, ctr_reg1.data);

    // dr_irq_enable = false;
}

int16_t mma8451_getAcX(void)
{
    return (int16_t)(((int32_t)readX * 100) / (int32_t)4096);
}

int16_t mma8451_getAcY(void)
{
    return (int16_t)(((int32_t)readY * 100) / (int32_t)4096);
}

int16_t mma8451_getAcZ(void)
{
    return (int16_t)(((int32_t)readZ * 100) / (int32_t)4096);
}

bool mma8451_getIsFalling()
{
    bool temp = is_falling;
    if (is_falling)
        is_falling = false;
    return temp;
}

void PORTC_PORTD_IRQHandler(void)
{
    // static unsigned int contador_interrupciones = 0;
    int16_t readG;
    INT_SOURCE_t intSource;
    STATUS_t status;
    uint8_t lecturas[6];

    intSource.data = mma8451_read_reg(INT_SOURCE_ADDRESS);

    if (intSource.SRC_DRDY)
    {

        status.data = mma8451_read_reg(STATUS_ADDRESS);
        mma8451_read_multi_reg(0x01, 6, lecturas);

        if (status.XDR)
        {
            readG = (int16_t)lecturas[0] << 8;
            readG |= lecturas[1];
            readX = readG >> 2;
        }

        if (status.YDR)
        {
            readG = (int16_t)lecturas[2] << 8;
            readG |= lecturas[3];
            readY = readG >> 2;
        }

        if (status.ZDR)
        {
            readG = (int16_t)lecturas[4] << 8;
            readG |= lecturas[5];
            readZ = readG >> 2;
        }
    }

    if (intSource.SRC_FF_MT)
    {
        mma8451_read_reg(FF_MT_SRC_ADDRESS); // Limpio la bandera
        is_falling = true;
    }

    PORT_ClearPinsInterruptFlags(INT1_PORT, 1 << INT1_PIN);
}

/*==================[end of file]============================================*/
