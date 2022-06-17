################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Core/Startup/startup_stm32f103c6tx.s 

OBJS += \
./Core/Startup/startup_stm32f103c6tx.o 

S_DEPS += \
./Core/Startup/startup_stm32f103c6tx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Startup/%.o: ../Core/Startup/%.s Core/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m3 -g3 -DDEBUG -c -I"C:/Users/geniw/source/stm32/ClockGenerator/ClockGenerator/ThirdParty/ssd1306" -I"C:/Users/geniw/source/stm32/ClockGenerator/ClockGenerator/ThirdParty/Si5351-lib/inc" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@" "$<"

clean: clean-Core-2f-Startup

clean-Core-2f-Startup:
	-$(RM) ./Core/Startup/startup_stm32f103c6tx.d ./Core/Startup/startup_stm32f103c6tx.o

.PHONY: clean-Core-2f-Startup

