################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../gptp_stack/port/gptp_port.c \
../gptp_stack/port/gptp_port_platform.c 

OBJS += \
./gptp_stack/port/gptp_port.o \
./gptp_stack/port/gptp_port_platform.o 

C_DEPS += \
./gptp_stack/port/gptp_port.d \
./gptp_stack/port/gptp_port_platform.d 


# Each subdirectory must supply rules for building sources it contributes
gptp_stack/port/%.o: ../gptp_stack/port/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@gptp_stack/port/gptp_port.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


