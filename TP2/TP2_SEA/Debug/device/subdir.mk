################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../device/system_MKL43Z4.c 

OBJS += \
./device/system_MKL43Z4.o 

C_DEPS += \
./device/system_MKL43Z4.d 


# Each subdirectory must supply rules for building sources it contributes
device/%.o: ../device/%.c device/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MKL43Z256VLH4 -DCPU_MKL43Z256VLH4_cm0plus -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__REDLIB__ -I"C:\Users\lraff\git\TPs_SEA\TP2\TP2_SEA\board" -I"C:\Users\lraff\git\TPs_SEA\TP2\TP2_SEA\source" -I"C:\Users\lraff\git\TPs_SEA\TP2\TP2_SEA" -I"C:\Users\lraff\git\TPs_SEA\TP2\TP2_SEA\drivers" -I"C:\Users\lraff\git\TPs_SEA\TP2\TP2_SEA\component\uart" -I"C:\Users\lraff\git\TPs_SEA\TP2\TP2_SEA\component\serial_manager" -I"C:\Users\lraff\git\TPs_SEA\TP2\TP2_SEA\utilities" -I"C:\Users\lraff\git\TPs_SEA\TP2\TP2_SEA\component\lists" -I"C:\Users\lraff\git\TPs_SEA\TP2\TP2_SEA\device" -I"C:\Users\lraff\git\TPs_SEA\TP2\TP2_SEA\CMSIS" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


