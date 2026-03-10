################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/Src/ODrive/ODriveCAN.cpp 

OBJS += \
./Core/Src/ODrive/ODriveCAN.o 

CPP_DEPS += \
./Core/Src/ODrive/ODriveCAN.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/ODrive/%.o Core/Src/ODrive/%.su Core/Src/ODrive/%.cyclo: ../Core/Src/ODrive/%.cpp Core/Src/ODrive/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-ODrive

clean-Core-2f-Src-2f-ODrive:
	-$(RM) ./Core/Src/ODrive/ODriveCAN.cyclo ./Core/Src/ODrive/ODriveCAN.d ./Core/Src/ODrive/ODriveCAN.o ./Core/Src/ODrive/ODriveCAN.su

.PHONY: clean-Core-2f-Src-2f-ODrive

