################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../app/src/device.c \
../app/src/gptp_cbk.c \
../app/src/main.c 

OBJS += \
./app/src/device.o \
./app/src/gptp_cbk.o \
./app/src/main.o 

C_DEPS += \
./app/src/device.d \
./app/src/gptp_cbk.d \
./app/src/main.d 


# Each subdirectory must supply rules for building sources it contributes
app/src/%.o: ../app/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@app/src/device.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


