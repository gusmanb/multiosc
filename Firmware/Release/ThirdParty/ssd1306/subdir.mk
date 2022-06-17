################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ThirdParty/ssd1306/ssd1306.c \
../ThirdParty/ssd1306/ssd1306_fonts.c 

OBJS += \
./ThirdParty/ssd1306/ssd1306.o \
./ThirdParty/ssd1306/ssd1306_fonts.o 

C_DEPS += \
./ThirdParty/ssd1306/ssd1306.d \
./ThirdParty/ssd1306/ssd1306_fonts.d 


# Each subdirectory must supply rules for building sources it contributes
ThirdParty/ssd1306/%.o ThirdParty/ssd1306/%.su: ../ThirdParty/ssd1306/%.c ThirdParty/ssd1306/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F103x6 -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/geniw/source/stm32/ClockGenerator/ClockGenerator/ThirdParty/ssd1306" -I"C:/Users/geniw/source/stm32/ClockGenerator/ClockGenerator/ThirdParty/Si5351-lib/inc" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-ThirdParty-2f-ssd1306

clean-ThirdParty-2f-ssd1306:
	-$(RM) ./ThirdParty/ssd1306/ssd1306.d ./ThirdParty/ssd1306/ssd1306.o ./ThirdParty/ssd1306/ssd1306.su ./ThirdParty/ssd1306/ssd1306_fonts.d ./ThirdParty/ssd1306/ssd1306_fonts.o ./ThirdParty/ssd1306/ssd1306_fonts.su

.PHONY: clean-ThirdParty-2f-ssd1306

