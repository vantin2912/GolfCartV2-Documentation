################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../fasthash/fasthash.c 

OBJS += \
./fasthash/fasthash.o 

C_DEPS += \
./fasthash/fasthash.d 


# Each subdirectory must supply rules for building sources it contributes
fasthash/%.o fasthash/%.su: ../fasthash/%.c fasthash/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"D:/Courses/CE437.N11 - ChuyenDeNhung1/CE437_Workspace/Lab 6/Core/SyncPrintf" -I"D:/Courses/CE437.N11 - ChuyenDeNhung1/CE437_Workspace/Lab 6/NetworkLayer" -I"D:/Courses/CE437.N11 - ChuyenDeNhung1/CE437_Workspace/Lab 6/Core/CanProtocol" -I"D:/Courses/CE437.N11 - ChuyenDeNhung1/CE437_Workspace/Lab 6/DiagnosticLayer" -I"D:/Courses/CE437.N11 - ChuyenDeNhung1/CE437_Workspace/Lab 6/fasthash" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-fasthash

clean-fasthash:
	-$(RM) ./fasthash/fasthash.d ./fasthash/fasthash.o ./fasthash/fasthash.su

.PHONY: clean-fasthash

