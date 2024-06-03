################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/SD2_I2C.c \
../source/SD2_board.c \
../source/TP3.c \
../source/animacion3D.c \
../source/display_utils.c \
../source/font5x7.c \
../source/mma8451.c \
../source/mtb.c \
../source/oled.c \
../source/pato_lucas.c \
../source/request_manager.c \
../source/ringBuffer.c \
../source/rs485_drv.c \
../source/semihost_hardfault.c \
../source/uart0_drv.c 

OBJS += \
./source/SD2_I2C.o \
./source/SD2_board.o \
./source/TP3.o \
./source/animacion3D.o \
./source/display_utils.o \
./source/font5x7.o \
./source/mma8451.o \
./source/mtb.o \
./source/oled.o \
./source/pato_lucas.o \
./source/request_manager.o \
./source/ringBuffer.o \
./source/rs485_drv.o \
./source/semihost_hardfault.o \
./source/uart0_drv.o 

C_DEPS += \
./source/SD2_I2C.d \
./source/SD2_board.d \
./source/TP3.d \
./source/animacion3D.d \
./source/display_utils.d \
./source/font5x7.d \
./source/mma8451.d \
./source/mtb.d \
./source/oled.d \
./source/pato_lucas.d \
./source/request_manager.d \
./source/ringBuffer.d \
./source/rs485_drv.d \
./source/semihost_hardfault.d \
./source/uart0_drv.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MKL43Z256VLH4 -DCPU_MKL43Z256VLH4_cm0plus -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__REDLIB__ -I"C:\Users\lraff\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TP3_SD2_Raffagnini_Rodriguez\board" -I"C:\Users\lraff\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TP3_SD2_Raffagnini_Rodriguez\source" -I"C:\Users\lraff\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TP3_SD2_Raffagnini_Rodriguez" -I"C:\Users\lraff\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TP3_SD2_Raffagnini_Rodriguez\drivers" -I"C:\Users\lraff\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TP3_SD2_Raffagnini_Rodriguez\component\uart" -I"C:\Users\lraff\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TP3_SD2_Raffagnini_Rodriguez\component\serial_manager" -I"C:\Users\lraff\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TP3_SD2_Raffagnini_Rodriguez\utilities" -I"C:\Users\lraff\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TP3_SD2_Raffagnini_Rodriguez\component\lists" -I"C:\Users\lraff\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TP3_SD2_Raffagnini_Rodriguez\device" -I"C:\Users\lraff\Documents\MCUXpressoIDE_11.5.1_7266\workspace\TP3_SD2_Raffagnini_Rodriguez\CMSIS" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


