################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/fsl_clock.c \
../drivers/fsl_common.c \
../drivers/fsl_dma.c \
../drivers/fsl_dmamux.c \
../drivers/fsl_gpio.c \
../drivers/fsl_i2c.c \
../drivers/fsl_lpuart.c \
../drivers/fsl_lpuart_dma.c \
../drivers/fsl_smc.c \
../drivers/fsl_spi.c \
../drivers/fsl_spi_dma.c 

OBJS += \
./drivers/fsl_clock.o \
./drivers/fsl_common.o \
./drivers/fsl_dma.o \
./drivers/fsl_dmamux.o \
./drivers/fsl_gpio.o \
./drivers/fsl_i2c.o \
./drivers/fsl_lpuart.o \
./drivers/fsl_lpuart_dma.o \
./drivers/fsl_smc.o \
./drivers/fsl_spi.o \
./drivers/fsl_spi_dma.o 

C_DEPS += \
./drivers/fsl_clock.d \
./drivers/fsl_common.d \
./drivers/fsl_dma.d \
./drivers/fsl_dmamux.d \
./drivers/fsl_gpio.d \
./drivers/fsl_i2c.d \
./drivers/fsl_lpuart.d \
./drivers/fsl_lpuart_dma.d \
./drivers/fsl_smc.d \
./drivers/fsl_spi.d \
./drivers/fsl_spi_dma.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/%.o: ../drivers/%.c drivers/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MKL43Z256VLH4 -DCPU_MKL43Z256VLH4_cm0plus -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__REDLIB__ -I"C:\Users\lraff\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TP3_SD2_Raffagnini_Rodriguez\board" -I"C:\Users\lraff\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TP3_SD2_Raffagnini_Rodriguez\source" -I"C:\Users\lraff\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TP3_SD2_Raffagnini_Rodriguez" -I"C:\Users\lraff\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TP3_SD2_Raffagnini_Rodriguez\drivers" -I"C:\Users\lraff\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TP3_SD2_Raffagnini_Rodriguez\component\uart" -I"C:\Users\lraff\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TP3_SD2_Raffagnini_Rodriguez\component\serial_manager" -I"C:\Users\lraff\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TP3_SD2_Raffagnini_Rodriguez\utilities" -I"C:\Users\lraff\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TP3_SD2_Raffagnini_Rodriguez\component\lists" -I"C:\Users\lraff\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TP3_SD2_Raffagnini_Rodriguez\device" -I"C:\Users\lraff\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TP3_SD2_Raffagnini_Rodriguez\CMSIS" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


