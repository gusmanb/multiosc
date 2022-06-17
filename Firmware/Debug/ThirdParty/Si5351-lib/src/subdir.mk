################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ThirdParty/Si5351-lib/src/si5351.c 

OBJS += \
./ThirdParty/Si5351-lib/src/si5351.o 

C_DEPS += \
./ThirdParty/Si5351-lib/src/si5351.d 


# Each subdirectory must supply rules for building sources it contributes
ThirdParty/Si5351-lib/src/%.o ThirdParty/Si5351-lib/src/%.su: ../ThirdParty/Si5351-lib/src/%.c ThirdParty/Si5351-lib/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103x6 -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/geniw/source/stm32/ClockGenerator/ClockGenerator/ThirdParty/ssd1306" -I"C:/Users/geniw/source/stm32/ClockGenerator/ClockGenerator/ThirdParty/Si5351-lib/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-ThirdParty-2f-Si5351-2d-lib-2f-src

clean-ThirdParty-2f-Si5351-2d-lib-2f-src:
	-$(RM) ./ThirdParty/Si5351-lib/src/si5351.d ./ThirdParty/Si5351-lib/src/si5351.o ./ThirdParty/Si5351-lib/src/si5351.su

.PHONY: clean-ThirdParty-2f-Si5351-2d-lib-2f-src

