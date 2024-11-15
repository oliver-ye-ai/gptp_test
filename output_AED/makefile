################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include gptp_stack/port/subdir.mk
-include gptp_stack/gptp_core/src/subdir.mk
-include generate/src/subdir.mk
-include board/subdir.mk
-include app/src/subdir.mk
-include RTD/src/subdir.mk
-include Project_Settings/Startup_Code/subdir.mk
-include Project_Settings/Linker_Files/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_SIZE += \
s32k344_gptp_example_aed.siz \


# All Target
all: s32k344_gptp_example_aed.elf secondary-outputs

# Tool invocations
s32k344_gptp_example_aed.elf: $(OBJS) C:/Users/Applied/workspaceS32DS.4.0/S32K344_gptp_ds_/Project_Settings/Linker_Files/linker_flash_s32k344.ld $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Standard S32DS C Linker'
	arm-none-eabi-gcc -o "s32k344_gptp_example_aed.elf" "@s32k344_gptp_example_aed.args"  $(USER_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

s32k344_gptp_example_aed.siz: s32k344_gptp_example_aed.elf
	@echo 'Invoking: Standard S32DS Print Size'
	arm-none-eabi-size --format=berkeley s32k344_gptp_example_aed.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) ./*
	-@echo ' '

secondary-outputs: $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
