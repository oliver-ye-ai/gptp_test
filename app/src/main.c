/*
 *   Copyright 2017, 2019-2024 NXP
 *
 *   NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be
 *   used strictly in accordance with the applicable license terms. By expressly
 *   accepting such terms or by downloading, installing, activating and/or otherwise
 *   using the software, you are agreeing that you have read, and that you agree to
 *   comply with and are bound by, such license terms. If you do not agree to be
 *   bound by the applicable license terms, then you may not retain, install,
 *   activate or otherwise use the software.
 *
 *   This file contains sample code only. It is not part of the production code deliverables.
 */

 /**
 * @file main.c
 *
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "Mcu.h"
#include "Port.h"
#include "Eth_43_GMAC.h"
#include "OsIf.h"
#include "Gmac_Ip.h"
#include "Gmac_Ip_Hw_Access.h"
#include "device.h"
#include "Devassert.h"
#include "gptp.h"
#include "gptp_port_platform.h"
#include "s32k344_gptp_config.h"
#include "Clock_Ip.h"
#include "Schm_MemAcc.h"
#include "Schm_Fee.h"
#include "Fee.h"
#include "EthTrcv.h"
#include "EthIf_Cbk.h"
#include "Gpt.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define MILLISECOND_IN_NS               (1000000U)
#define PIT0_1KHZ_CNT                   (PIT_FREQUENCY/1000u)

#define PDELAY_IDX_OFFSET               (0x0)
#define RRATIO_IDX_OFFSET               (0x1)

#define PORT_COUNT                      (1u)
#define PORT_DATA_ENTRY_SIZE            (8u)
/* In our case pdelay & rratio are written per port. */
#define PORT_DATA_ENTRY_COUNT           (2u)

/*******************************************************************************
 * Data types
 ******************************************************************************/

typedef enum {
    WRITE_PENDING,
    WRITE_IN_PROGRESS,
    WRITE_COMPLETE
} write_state_t;

typedef struct {
    uint8_t au8Data[PORT_DATA_ENTRY_SIZE];
    uint8_t u8Offset;
    write_state_t eState;
} gptp_nvm_data_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static void EEPROM_Init(void);
static void EEPROM_Enqueue_Write(const uint8_t cu8Port, 
                                 const uint8_t cu8Offset,
                                 const uint32_t cu32Length,
                                 const uint8_t *cpu8Data);
static void EEPROM_Write_Poll(void);
static uint8_t EEPROM_Read(const uint8_t cu8Port, 
                           const uint8_t cu8Offset,
                           const uint32_t cu32Length,
                           uint8_t *pu8Data);
static void EEPROM_Poll(void);
static void Eth_Poll(void);
static void Eth_PollLinkStatus(void);
static void Eth_Configure_1PPS_Output(uint8_t u8PPSCtrl);

void Poll_Gptp_Eth(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

extern const Eth_43_GMAC_ConfigType Eth_43_GMAC_xPredefinedConfig;

static gptp_nvm_data_t              sarPerPortData[PORT_COUNT * PORT_DATA_ENTRY_COUNT];
static MemIf_StatusType             seMemStatus = MEMIF_IDLE;

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief           This function initializes all components necessary for
 *                  simulated EEPROM in data flash.
 *
 * @details         This function prepares cache data entries, enables MemmAcc
 *                  and Fee.
*/
static void EEPROM_Init(void)
{
    uint32_t u32PortCacheEntry;
    uint32_t u32PortDataEntry;

    /* Prepare cache default values. */
    for (u32PortCacheEntry = 0u; u32PortCacheEntry < (PORT_COUNT * PORT_DATA_ENTRY_COUNT); u32PortCacheEntry++)
    {
        sarPerPortData[u32PortCacheEntry].eState = WRITE_COMPLETE;
        sarPerPortData[u32PortCacheEntry].u8Offset = PORT_DATA_ENTRY_SIZE * u32PortCacheEntry;

        for (u32PortDataEntry = 0u; u32PortDataEntry < PORT_DATA_ENTRY_SIZE; u32PortDataEntry++)
        {
            sarPerPortData[u32PortCacheEntry].au8Data[u32PortDataEntry] = 0u;
        }
    }

    /* Init MemAcc. */
    MemAcc_Init(NULL_PTR);

    /* Init Fee. */
    Fee_Init(NULL_PTR);

    /* Perform init Fee driver. */
    do
    {
        EEPROM_Poll();
    } while (MEMIF_IDLE != seMemStatus);
}

/*!
 * @brief           This function queues up data to be written to EEPROM.
 * 
 * @param[in]       cu8Port Port ID.
 * @param[in]       cu8Offset EEPROM address offset.
 * @param[in]       cu32Length Data length.
 * @param[in]       cpu8Data Data to be enqueued.
*/
static void EEPROM_Enqueue_Write(const uint8_t cu8Port,
                                 const uint8_t cu8Offset,
                                 const uint32_t cu32Length,
                                 const uint8_t *cpu8Data)
{
    const uint32_t cu32Idx = (PORT_DATA_ENTRY_COUNT * cu8Port) + cu8Offset;
    uint32_t       u32Entry;

    if ((cu32Length <= PORT_DATA_ENTRY_SIZE) &&
        (cu32Idx < (PORT_COUNT * PORT_DATA_ENTRY_COUNT)))
    {
        for (u32Entry = 0u; u32Entry < cu32Length; u32Entry++)
        {
            sarPerPortData[cu32Idx].au8Data[u32Entry] = cpu8Data[u32Entry];
        }
        sarPerPortData[cu32Idx].eState = WRITE_PENDING;
    }
}

/*!
 * @brief           Executes actual write to EEPROM.
 *
 * @details         This function goes through the cache entry and executes
 *                  write above the first entry which has a WRITE_PENING state.
*/
static void EEPROM_Write_Poll(void)
{
    boolean  bIsWriteInProgress = false;
    uint32_t u32PortCacheEntry;
    uint16_t u16BlockNumber;

    EEPROM_Poll();
    if (MEMIF_IDLE == seMemStatus)
    {
        /* Clear status of previous write. */
        for (u32PortCacheEntry = 0u; u32PortCacheEntry < (PORT_COUNT * PORT_DATA_ENTRY_COUNT); u32PortCacheEntry++)
        {
            if (WRITE_IN_PROGRESS == sarPerPortData[u32PortCacheEntry].eState)
            {
                sarPerPortData[u32PortCacheEntry].eState = WRITE_COMPLETE;
            }
        }
        /* Write data from cache entry. */
        for (u32PortCacheEntry = 0u; u32PortCacheEntry < (PORT_COUNT * PORT_DATA_ENTRY_COUNT); u32PortCacheEntry++)
        {
            if ((WRITE_PENDING == sarPerPortData[u32PortCacheEntry].eState) &&
                !bIsWriteInProgress)
            {
                bIsWriteInProgress = true;
                /* In case of necessity handle error. */
                u16BlockNumber = sarPerPortData[u32PortCacheEntry].u8Offset == PDELAY_IDX_OFFSET ? FeeConf_FeeBlockConfiguration_FeeBlockConfiguration_0 : FeeConf_FeeBlockConfiguration_FeeBlockConfiguration_1;
                Fee_Write(u16BlockNumber, sarPerPortData[u32PortCacheEntry].au8Data);
                sarPerPortData[u32PortCacheEntry].eState = WRITE_IN_PROGRESS;
            }
        }
    }
}

/*!
 * @brief           This function reads data from EEPROM.
 * 
 * @param[in]       cu8Port Port ID.
 * @param[in]       cu8Offset EEPROM address offset.
 * @param[in]       cu32Length Data length.
 * @param[out]      pu8Data Data to be read.
 *
 * @return          0 on success.
*/
static uint8_t EEPROM_Read(const uint8_t cu8Port,
                           const uint8_t cu8Offset,
                           const uint32_t cu32Length,
                           uint8_t *pu8Data)
{
    uint8_t        u8Ret = 0u;
    const uint32_t cu32Idx = (PORT_DATA_ENTRY_COUNT * cu8Port) + cu8Offset;
    uint32_t       u32Entry;
    uint16_t       u16BlockNumber;

    if (cu32Idx >= (PORT_COUNT * PORT_DATA_ENTRY_COUNT))
    {
        u8Ret = 1u;
    }
    else
    {
        /* If data is about to be written or is being written at the moment,
           get the value from cache. */
        if ((WRITE_IN_PROGRESS == sarPerPortData[cu32Idx].eState) ||
            (WRITE_PENDING == sarPerPortData[cu32Idx].eState))
        {
            for (u32Entry = 0u; u32Entry < cu32Length; u32Entry++)
            {
                pu8Data[u32Entry] = sarPerPortData[cu32Idx].au8Data[u32Entry];
            }
        }
        else
        {
            u16BlockNumber = sarPerPortData[cu32Idx].u8Offset == PDELAY_IDX_OFFSET ? FeeConf_FeeBlockConfiguration_FeeBlockConfiguration_0 : FeeConf_FeeBlockConfiguration_FeeBlockConfiguration_1;

            /* Read data block. */
            Fee_Read(u16BlockNumber, 0u, pu8Data, cu32Length);

            /* Perform read data form Block 0. */
            do
            {
                EEPROM_Poll();
            } while (MEMIF_IDLE != seMemStatus);
        }
    }

    return u8Ret;
}

/*!
 * @brief           Executes actual EEPROM command.
 *
 * @details         This function call Fls/Fee main functions to execute
 *                  necessary actions.
*/
static void EEPROM_Poll(void)
{
    Fee_MainFunction();
    MemAcc_MainFunction();
    seMemStatus = Fee_GetStatus();
}

/*!
 * @brief           This function polls ethernet peripheral for events.
 *
 * @details         This function polls ethernet peripheral for following
 *                  events:
 *                      - RX frame received
 *                      - TX frame transmission completed
 *                      - Link state changed
 *                  When such event happens, corresponding EthIf_ Cbk function
 *                  is called from the context of this function.
*/
static void Eth_Poll(void)
{
    uint8            u8FifoIdx;
    Eth_RxStatusType rRxStatus;
 
    for (u8FifoIdx = 0u; u8FifoIdx < ETH_43_GMAC_MAX_RXFIFO_SUPPORTED; u8FifoIdx++)
    {
        Eth_43_GMAC_Receive(EthConf_EthCtrlConfig_EthCtrlConfig_0, u8FifoIdx,
                            &rRxStatus);
    }

    Eth_43_GMAC_TxConfirmation(EthConf_EthCtrlConfig_EthCtrlConfig_0);
}

/*!
 * @brief           This function watches the link status of all gPTP ports.
*/
static void Eth_PollLinkStatus(void)
{
    uint8_t                      u8CtrlIdx;
    /* Current link status. */
    static EthTrcv_LinkStateType seLinkState;
    /* Last link status for change detection. */
    static EthTrcv_LinkStateType seLastLinkState[ETH_43_GMAC_MAX_CTRLIDX_SUPPORTED] = {ETHTRCV_LINK_STATE_DOWN};

    for (u8CtrlIdx = 0u; u8CtrlIdx < ETH_43_GMAC_MAX_CTRLIDX_SUPPORTED; u8CtrlIdx++) {
        if (E_OK == EthTrcv_GetLinkState(u8CtrlIdx, &seLinkState))
        {
            /* Detect link status change. */
//            if (seLastLinkState[u8CtrlIdx] != seLinkState)
//            {
//                seLastLinkState[u8CtrlIdx] = seLinkState;
//                /* Notify via EthIf API. */
//                EthIf_TrcvLinkStateChg(u8CtrlIdx, seLinkState);
//            }
        }
    }
}

/*!
 * @brief           This function configures PPS output.
 *
 * @details         This function configures PPS output parameters.
 *
 * @param[in]       u8PPSCtrl Configuration of PPSCTRL field of MAC_PPS_CONTROL
 *                  register:
 *                      0 - 1Hz, short 20ns pulse
 *                      1 - 1Hz, 50% duty cycle
 *                      2 - 2Hz, 50% duty cycle
 *                      3 - 4Hz, 50% duty cycle
 *                      4 - 8Hz, 50% duty cycle
 *                      .....
 *                      15 - 32.768 kHz, 50% duty cycle
*/
static void Eth_Configure_1PPS_Output(uint8_t u8PPSCtrl)
{
    /* Clear the current PPSCTRL setting. */
    Gmac_apxBases[0]->MAC_PPS_CONTROL &= ~GMAC_MAC_PPS_CONTROL_PPSCTRL_PPSCMD_MASK;
     /* Set 1PPS output to 1Hz and 50% duty cycle. */
    Gmac_apxBases[0]->MAC_PPS_CONTROL |= GMAC_MAC_PPS_CONTROL_PPSCTRL_PPSCMD(u8PPSCtrl);
}

/*!
 * @brief           Callback function for EEPROM write.
*/
uint8_t EEPROM_Write_CB(uint8_t u8PdelayMachine,
                        gptp_def_nvm_data_t eNvmDataType,
                        float64_t f64Value,
                        gptp_def_mem_write_stat *peWriteStat)
{
    uint8_t u8Offset = eNvmDataType == GPTP_DEF_NVM_PDELAY ? PDELAY_IDX_OFFSET : RRATIO_IDX_OFFSET;
    uint8_t au8Data[PORT_DATA_ENTRY_SIZE];
    uint8_t u8Idx;

    if (GPTP_DEF_MEM_WRITE_INIT == *peWriteStat)
    {
        for (u8Idx = 0u; u8Idx < PORT_DATA_ENTRY_SIZE; u8Idx++)
        {
           au8Data[u8Idx] = ((uint8_t*)&f64Value)[u8Idx];
        }

        EEPROM_Enqueue_Write(u8PdelayMachine, u8Offset, PORT_DATA_ENTRY_SIZE,
                             au8Data);

        *peWriteStat = GPTP_DEF_MEM_WRITE_FINISH;
    }

    return 0u;
}

/*!
 * @brief           Callback function for EEPROM read.
*/
uint8_t EEPROM_Read_CB(uint8_t u8PdelayMachine,
                       gptp_def_nvm_data_t eNvmDataType,
                       float64_t *f64Value)
{
    uint8_t u8Status;
    uint8_t u8Offset = eNvmDataType == GPTP_DEF_NVM_PDELAY ? PDELAY_IDX_OFFSET : RRATIO_IDX_OFFSET;
    uint8_t au8Data[PORT_DATA_ENTRY_SIZE];
    uint8_t u8Idx;

    u8Status = EEPROM_Read(u8PdelayMachine, u8Offset, PORT_DATA_ENTRY_SIZE,
                           au8Data);

    for (u8Idx = 0u; u8Idx < PORT_DATA_ENTRY_SIZE; u8Idx++)
    {
       ((uint8_t*)f64Value)[u8Idx] = au8Data[u8Idx];
    }

   return u8Status;
}

/*!
 * @brief           This function calls periodically Eth and GPTP services.
*/
void Poll_Gptp_Eth(void)
{
    static volatile uint64_t u64PitIsrCountMs = 0;

    /* Increment 1ms = 1000000 ns. */
    GPTP_PORT_IncFreeRunningTimer(MILLISECOND_IN_NS);

    /*  Poll GPTP TimerPeriodic every 9ms. */
    if (0U == (u64PitIsrCountMs % 9u))
    {
        GPTP_TimerPeriodic();
    }

    /* Poll EEPROM write function every 5 ms. */
    if (0U == (u64PitIsrCountMs % 5u))
    {
        EEPROM_Write_Poll();
    }

    /*  Poll Ethernet interface every 2ms. */
    if (0U == (u64PitIsrCountMs % 2u))
    {
        Eth_PollLinkStatus();
        Eth_Poll();
    }

    u64PitIsrCountMs++;
}

/*!
 * @brief           Main function of this application.
*/
int main(void)
{
    /* Initialize HW peripherals. */
    device_init();

    /* Initialize EEPROM and I2C peripheral. */
    EEPROM_Init();

    /* Configure 1PPS output to 1Hz / 50% duty cycle. */
    Eth_Configure_1PPS_Output(1);

    /* Initialize GPTP stack. */
    gptp_err_type_t err = GPTP_GptpInit(&rGptpCfgParams);
    DevAssert(GPTP_ERR_OK == err);

    /* Start IRQ-mode GPT timer 1kHz. */
    Gpt_StartTimer(GptConf_GptChannelConfiguration_GptChannelConfiguration_0, PIT0_1KHZ_CNT);

    while (true)
    {
        /* Nothing to be called within the endless loop
         * (Poll_Gptp_Eth-Callback controlled application). */
        EXECUTE_WAIT();
    }

    return 0;
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
