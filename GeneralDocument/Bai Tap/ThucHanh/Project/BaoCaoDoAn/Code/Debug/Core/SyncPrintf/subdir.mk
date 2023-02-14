################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/SyncPrintf/SyncPrintf.c 

OBJS += \
./Core/SyncPrintf/SyncPrintf.o 

C_DEPS += \
./Core/SyncPrintf/SyncPrintf.d 


# Each subdirectory must supply rules for building sources it contributes
Core/SyncPrintf/%.o Core/SyncPrintf/%.su: ../Core/SyncPrintf/%.c Core/SyncPrintf/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"D:/Courses/CE437.N11 - ChuyenDeNhung1/CE437_Workspace/Lab 6/Core/SyncPrintf" -I"D:/Courses/CE437.N11 - ChuyenDeNhung1/CE437_Workspace/Lab 6/NetworkLayer" -I"D:/Courses/CE437.N11 - ChuyenDeNhung1/CE437_Workspace/Lab 6/Core/CanProtocol" -I"D:/Courses/CE437.N11 - ChuyenDeNhung1/CE437_Workspace/Lab 6/DiagnosticLayer" -I"D:/Courses/CE437.N11 - ChuyenDeNhung1/CE437_Workspace/Lab 6/fasthash" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-SyncPrintf

clean-Core-2f-SyncPrintf:
	-$(RM) ./Core/SyncPrintf/SyncPrintf.d ./Core/SyncPrintf/SyncPrintf.o ./Core/SyncPrintf/SyncPrintf.su

.PHONY: clean-Core-2f-SyncPrintf

