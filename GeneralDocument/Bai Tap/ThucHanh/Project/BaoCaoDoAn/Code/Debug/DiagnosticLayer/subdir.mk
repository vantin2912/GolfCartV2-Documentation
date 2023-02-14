################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../DiagnosticLayer/ECUDiagnostic.c \
../DiagnosticLayer/KeyCalculate.c \
../DiagnosticLayer/TesterDiagnostic.c 

OBJS += \
./DiagnosticLayer/ECUDiagnostic.o \
./DiagnosticLayer/KeyCalculate.o \
./DiagnosticLayer/TesterDiagnostic.o 

C_DEPS += \
./DiagnosticLayer/ECUDiagnostic.d \
./DiagnosticLayer/KeyCalculate.d \
./DiagnosticLayer/TesterDiagnostic.d 


# Each subdirectory must supply rules for building sources it contributes
DiagnosticLayer/%.o DiagnosticLayer/%.su: ../DiagnosticLayer/%.c DiagnosticLayer/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"D:/Courses/CE437.N11 - ChuyenDeNhung1/CE437_Workspace/Lab 6/Core/SyncPrintf" -I"D:/Courses/CE437.N11 - ChuyenDeNhung1/CE437_Workspace/Lab 6/NetworkLayer" -I"D:/Courses/CE437.N11 - ChuyenDeNhung1/CE437_Workspace/Lab 6/Core/CanProtocol" -I"D:/Courses/CE437.N11 - ChuyenDeNhung1/CE437_Workspace/Lab 6/DiagnosticLayer" -I"D:/Courses/CE437.N11 - ChuyenDeNhung1/CE437_Workspace/Lab 6/fasthash" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-DiagnosticLayer

clean-DiagnosticLayer:
	-$(RM) ./DiagnosticLayer/ECUDiagnostic.d ./DiagnosticLayer/ECUDiagnostic.o ./DiagnosticLayer/ECUDiagnostic.su ./DiagnosticLayer/KeyCalculate.d ./DiagnosticLayer/KeyCalculate.o ./DiagnosticLayer/KeyCalculate.su ./DiagnosticLayer/TesterDiagnostic.d ./DiagnosticLayer/TesterDiagnostic.o ./DiagnosticLayer/TesterDiagnostic.su

.PHONY: clean-DiagnosticLayer

