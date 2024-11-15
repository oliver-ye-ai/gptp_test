################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../RTD/src/C40_Ip.c \
../RTD/src/C40_Ip_Ac.c \
../RTD/src/Cache_Ip.c \
../RTD/src/Cache_Ip_HwAcc_ArmCoreMx.c \
../RTD/src/Clock_Ip.c \
../RTD/src/Clock_Ip_Data.c \
../RTD/src/Clock_Ip_Divider.c \
../RTD/src/Clock_Ip_DividerTrigger.c \
../RTD/src/Clock_Ip_ExtOsc.c \
../RTD/src/Clock_Ip_FracDiv.c \
../RTD/src/Clock_Ip_Frequency.c \
../RTD/src/Clock_Ip_Gate.c \
../RTD/src/Clock_Ip_IntOsc.c \
../RTD/src/Clock_Ip_Irq.c \
../RTD/src/Clock_Ip_Monitor.c \
../RTD/src/Clock_Ip_Pll.c \
../RTD/src/Clock_Ip_ProgFreqSwitch.c \
../RTD/src/Clock_Ip_Selector.c \
../RTD/src/Clock_Ip_Specific.c \
../RTD/src/Dem.c \
../RTD/src/Dem_stub.c \
../RTD/src/Det.c \
../RTD/src/Det_stub.c \
../RTD/src/Emios_Gpt_Ip.c \
../RTD/src/EthIf.c \
../RTD/src/EthSwt.c \
../RTD/src/EthTrcv.c \
../RTD/src/Eth_43_GMAC.c \
../RTD/src/Eth_43_GMAC_Ipw.c \
../RTD/src/Eth_43_GMAC_Ipw_Irq.c \
../RTD/src/Eth_43_GMAC_Irq.c \
../RTD/src/Fee.c \
../RTD/src/Gmac_Ip.c \
../RTD/src/Gmac_Ip_Hw_Access.c \
../RTD/src/Gmac_Ip_Irq.c \
../RTD/src/Gpt.c \
../RTD/src/Gpt_Ipw.c \
../RTD/src/Igf_Port_Ip.c \
../RTD/src/IntCtrl_Ip.c \
../RTD/src/Mcu.c \
../RTD/src/Mcu_Dem_Wrapper.c \
../RTD/src/Mcu_Ipw.c \
../RTD/src/MemAcc.c \
../RTD/src/MemAcc_Sema4.c \
../RTD/src/Mem_43_INFLS.c \
../RTD/src/Mem_43_INFLS_Ipw.c \
../RTD/src/Mem_43_INFLS_Software_Semaphore.c \
../RTD/src/Mpu_M7_Ip.c \
../RTD/src/OsIf_Interrupts.c \
../RTD/src/OsIf_Timer.c \
../RTD/src/OsIf_Timer_System.c \
../RTD/src/OsIf_Timer_System_Internal_Systick.c \
../RTD/src/Os_counter_api.c \
../RTD/src/Os_event.c \
../RTD/src/Os_isr.c \
../RTD/src/Os_multicore.c \
../RTD/src/Os_scheduler.c \
../RTD/src/Osif_Software_Semaphore.c \
../RTD/src/Pit_Ip.c \
../RTD/src/Platform.c \
../RTD/src/Platform_Ipw.c \
../RTD/src/Port.c \
../RTD/src/Port_Ipw.c \
../RTD/src/Power_Ip.c \
../RTD/src/Power_Ip_AEC.c \
../RTD/src/Power_Ip_CortexM7.c \
../RTD/src/Power_Ip_DCM_GPR.c \
../RTD/src/Power_Ip_FLASH.c \
../RTD/src/Power_Ip_MC_ME.c \
../RTD/src/Power_Ip_MC_RGM.c \
../RTD/src/Power_Ip_MC_RGM_Irq.c \
../RTD/src/Power_Ip_PMC.c \
../RTD/src/Power_Ip_PMC_Irq.c \
../RTD/src/Power_Ip_Private.c \
../RTD/src/Ram_Ip.c \
../RTD/src/Rtc_Ip.c \
../RTD/src/SchM_Eth_43_GMAC.c \
../RTD/src/SchM_Fee.c \
../RTD/src/SchM_Gpt.c \
../RTD/src/SchM_Mcl.c \
../RTD/src/SchM_Mcu.c \
../RTD/src/SchM_MemAcc.c \
../RTD/src/SchM_Mem_43_INFLS.c \
../RTD/src/SchM_Platform.c \
../RTD/src/SchM_Port.c \
../RTD/src/Siul2_Port_Ip.c \
../RTD/src/Stm_Ip.c \
../RTD/src/System_Ip.c \
../RTD/src/Tspc_Port_Ip.c 

OBJS += \
./RTD/src/C40_Ip.o \
./RTD/src/C40_Ip_Ac.o \
./RTD/src/Cache_Ip.o \
./RTD/src/Cache_Ip_HwAcc_ArmCoreMx.o \
./RTD/src/Clock_Ip.o \
./RTD/src/Clock_Ip_Data.o \
./RTD/src/Clock_Ip_Divider.o \
./RTD/src/Clock_Ip_DividerTrigger.o \
./RTD/src/Clock_Ip_ExtOsc.o \
./RTD/src/Clock_Ip_FracDiv.o \
./RTD/src/Clock_Ip_Frequency.o \
./RTD/src/Clock_Ip_Gate.o \
./RTD/src/Clock_Ip_IntOsc.o \
./RTD/src/Clock_Ip_Irq.o \
./RTD/src/Clock_Ip_Monitor.o \
./RTD/src/Clock_Ip_Pll.o \
./RTD/src/Clock_Ip_ProgFreqSwitch.o \
./RTD/src/Clock_Ip_Selector.o \
./RTD/src/Clock_Ip_Specific.o \
./RTD/src/Dem.o \
./RTD/src/Dem_stub.o \
./RTD/src/Det.o \
./RTD/src/Det_stub.o \
./RTD/src/Emios_Gpt_Ip.o \
./RTD/src/EthIf.o \
./RTD/src/EthSwt.o \
./RTD/src/EthTrcv.o \
./RTD/src/Eth_43_GMAC.o \
./RTD/src/Eth_43_GMAC_Ipw.o \
./RTD/src/Eth_43_GMAC_Ipw_Irq.o \
./RTD/src/Eth_43_GMAC_Irq.o \
./RTD/src/Fee.o \
./RTD/src/Gmac_Ip.o \
./RTD/src/Gmac_Ip_Hw_Access.o \
./RTD/src/Gmac_Ip_Irq.o \
./RTD/src/Gpt.o \
./RTD/src/Gpt_Ipw.o \
./RTD/src/Igf_Port_Ip.o \
./RTD/src/IntCtrl_Ip.o \
./RTD/src/Mcu.o \
./RTD/src/Mcu_Dem_Wrapper.o \
./RTD/src/Mcu_Ipw.o \
./RTD/src/MemAcc.o \
./RTD/src/MemAcc_Sema4.o \
./RTD/src/Mem_43_INFLS.o \
./RTD/src/Mem_43_INFLS_Ipw.o \
./RTD/src/Mem_43_INFLS_Software_Semaphore.o \
./RTD/src/Mpu_M7_Ip.o \
./RTD/src/OsIf_Interrupts.o \
./RTD/src/OsIf_Timer.o \
./RTD/src/OsIf_Timer_System.o \
./RTD/src/OsIf_Timer_System_Internal_Systick.o \
./RTD/src/Os_counter_api.o \
./RTD/src/Os_event.o \
./RTD/src/Os_isr.o \
./RTD/src/Os_multicore.o \
./RTD/src/Os_scheduler.o \
./RTD/src/Osif_Software_Semaphore.o \
./RTD/src/Pit_Ip.o \
./RTD/src/Platform.o \
./RTD/src/Platform_Ipw.o \
./RTD/src/Port.o \
./RTD/src/Port_Ipw.o \
./RTD/src/Power_Ip.o \
./RTD/src/Power_Ip_AEC.o \
./RTD/src/Power_Ip_CortexM7.o \
./RTD/src/Power_Ip_DCM_GPR.o \
./RTD/src/Power_Ip_FLASH.o \
./RTD/src/Power_Ip_MC_ME.o \
./RTD/src/Power_Ip_MC_RGM.o \
./RTD/src/Power_Ip_MC_RGM_Irq.o \
./RTD/src/Power_Ip_PMC.o \
./RTD/src/Power_Ip_PMC_Irq.o \
./RTD/src/Power_Ip_Private.o \
./RTD/src/Ram_Ip.o \
./RTD/src/Rtc_Ip.o \
./RTD/src/SchM_Eth_43_GMAC.o \
./RTD/src/SchM_Fee.o \
./RTD/src/SchM_Gpt.o \
./RTD/src/SchM_Mcl.o \
./RTD/src/SchM_Mcu.o \
./RTD/src/SchM_MemAcc.o \
./RTD/src/SchM_Mem_43_INFLS.o \
./RTD/src/SchM_Platform.o \
./RTD/src/SchM_Port.o \
./RTD/src/Siul2_Port_Ip.o \
./RTD/src/Stm_Ip.o \
./RTD/src/System_Ip.o \
./RTD/src/Tspc_Port_Ip.o 

C_DEPS += \
./RTD/src/C40_Ip.d \
./RTD/src/C40_Ip_Ac.d \
./RTD/src/Cache_Ip.d \
./RTD/src/Cache_Ip_HwAcc_ArmCoreMx.d \
./RTD/src/Clock_Ip.d \
./RTD/src/Clock_Ip_Data.d \
./RTD/src/Clock_Ip_Divider.d \
./RTD/src/Clock_Ip_DividerTrigger.d \
./RTD/src/Clock_Ip_ExtOsc.d \
./RTD/src/Clock_Ip_FracDiv.d \
./RTD/src/Clock_Ip_Frequency.d \
./RTD/src/Clock_Ip_Gate.d \
./RTD/src/Clock_Ip_IntOsc.d \
./RTD/src/Clock_Ip_Irq.d \
./RTD/src/Clock_Ip_Monitor.d \
./RTD/src/Clock_Ip_Pll.d \
./RTD/src/Clock_Ip_ProgFreqSwitch.d \
./RTD/src/Clock_Ip_Selector.d \
./RTD/src/Clock_Ip_Specific.d \
./RTD/src/Dem.d \
./RTD/src/Dem_stub.d \
./RTD/src/Det.d \
./RTD/src/Det_stub.d \
./RTD/src/Emios_Gpt_Ip.d \
./RTD/src/EthIf.d \
./RTD/src/EthSwt.d \
./RTD/src/EthTrcv.d \
./RTD/src/Eth_43_GMAC.d \
./RTD/src/Eth_43_GMAC_Ipw.d \
./RTD/src/Eth_43_GMAC_Ipw_Irq.d \
./RTD/src/Eth_43_GMAC_Irq.d \
./RTD/src/Fee.d \
./RTD/src/Gmac_Ip.d \
./RTD/src/Gmac_Ip_Hw_Access.d \
./RTD/src/Gmac_Ip_Irq.d \
./RTD/src/Gpt.d \
./RTD/src/Gpt_Ipw.d \
./RTD/src/Igf_Port_Ip.d \
./RTD/src/IntCtrl_Ip.d \
./RTD/src/Mcu.d \
./RTD/src/Mcu_Dem_Wrapper.d \
./RTD/src/Mcu_Ipw.d \
./RTD/src/MemAcc.d \
./RTD/src/MemAcc_Sema4.d \
./RTD/src/Mem_43_INFLS.d \
./RTD/src/Mem_43_INFLS_Ipw.d \
./RTD/src/Mem_43_INFLS_Software_Semaphore.d \
./RTD/src/Mpu_M7_Ip.d \
./RTD/src/OsIf_Interrupts.d \
./RTD/src/OsIf_Timer.d \
./RTD/src/OsIf_Timer_System.d \
./RTD/src/OsIf_Timer_System_Internal_Systick.d \
./RTD/src/Os_counter_api.d \
./RTD/src/Os_event.d \
./RTD/src/Os_isr.d \
./RTD/src/Os_multicore.d \
./RTD/src/Os_scheduler.d \
./RTD/src/Osif_Software_Semaphore.d \
./RTD/src/Pit_Ip.d \
./RTD/src/Platform.d \
./RTD/src/Platform_Ipw.d \
./RTD/src/Port.d \
./RTD/src/Port_Ipw.d \
./RTD/src/Power_Ip.d \
./RTD/src/Power_Ip_AEC.d \
./RTD/src/Power_Ip_CortexM7.d \
./RTD/src/Power_Ip_DCM_GPR.d \
./RTD/src/Power_Ip_FLASH.d \
./RTD/src/Power_Ip_MC_ME.d \
./RTD/src/Power_Ip_MC_RGM.d \
./RTD/src/Power_Ip_MC_RGM_Irq.d \
./RTD/src/Power_Ip_PMC.d \
./RTD/src/Power_Ip_PMC_Irq.d \
./RTD/src/Power_Ip_Private.d \
./RTD/src/Ram_Ip.d \
./RTD/src/Rtc_Ip.d \
./RTD/src/SchM_Eth_43_GMAC.d \
./RTD/src/SchM_Fee.d \
./RTD/src/SchM_Gpt.d \
./RTD/src/SchM_Mcl.d \
./RTD/src/SchM_Mcu.d \
./RTD/src/SchM_MemAcc.d \
./RTD/src/SchM_Mem_43_INFLS.d \
./RTD/src/SchM_Platform.d \
./RTD/src/SchM_Port.d \
./RTD/src/Siul2_Port_Ip.d \
./RTD/src/Stm_Ip.d \
./RTD/src/System_Ip.d \
./RTD/src/Tspc_Port_Ip.d 


# Each subdirectory must supply rules for building sources it contributes
RTD/src/%.o: ../RTD/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@RTD/src/C40_Ip.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


