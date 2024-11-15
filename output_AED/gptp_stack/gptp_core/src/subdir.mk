################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../gptp_stack/gptp_core/src/gptp.c \
../gptp_stack/gptp_core/src/gptp_err.c \
../gptp_stack/gptp_core/src/gptp_frame.c \
../gptp_stack/gptp_core/src/gptp_internal.c \
../gptp_stack/gptp_core/src/gptp_pdelay.c \
../gptp_stack/gptp_core/src/gptp_pi.c \
../gptp_stack/gptp_core/src/gptp_signaling.c \
../gptp_stack/gptp_core/src/gptp_sync.c \
../gptp_stack/gptp_core/src/gptp_timer.c 

OBJS += \
./gptp_stack/gptp_core/src/gptp.o \
./gptp_stack/gptp_core/src/gptp_err.o \
./gptp_stack/gptp_core/src/gptp_frame.o \
./gptp_stack/gptp_core/src/gptp_internal.o \
./gptp_stack/gptp_core/src/gptp_pdelay.o \
./gptp_stack/gptp_core/src/gptp_pi.o \
./gptp_stack/gptp_core/src/gptp_signaling.o \
./gptp_stack/gptp_core/src/gptp_sync.o \
./gptp_stack/gptp_core/src/gptp_timer.o 

C_DEPS += \
./gptp_stack/gptp_core/src/gptp.d \
./gptp_stack/gptp_core/src/gptp_err.d \
./gptp_stack/gptp_core/src/gptp_frame.d \
./gptp_stack/gptp_core/src/gptp_internal.d \
./gptp_stack/gptp_core/src/gptp_pdelay.d \
./gptp_stack/gptp_core/src/gptp_pi.d \
./gptp_stack/gptp_core/src/gptp_signaling.d \
./gptp_stack/gptp_core/src/gptp_sync.d \
./gptp_stack/gptp_core/src/gptp_timer.d 


# Each subdirectory must supply rules for building sources it contributes
gptp_stack/gptp_core/src/%.o: ../gptp_stack/gptp_core/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@gptp_stack/gptp_core/src/gptp.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


