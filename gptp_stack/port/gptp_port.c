/*
 * Copyright 2023-2024 NXP
 *
 * NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be
 * used strictly in accordance with the applicable license terms. By expressly
 * accepting such terms or by downloading, installing, activating and/or otherwise
 * using the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms.  If you do not agree to be
 * bound by the applicable license terms, then you may not retain, install,
 * activate or otherwise use the software.
 */

 /**
 * @file gptp_port.c
 *
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "gptp_def.h"
#include "gptp_port.h"
#include "gptp_port_platform.h"
#include "gptp.h"
#include "gptp_err.h"
#include "gptp_frame.h"
#include "gptp_pi.h"
#include "gptp_internal.h"
#include "Eth_43_GMAC.h"
#include "EthTrcv.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! Index of ethernet controller which manage clock timer frequency. */
#define GPTP_PORT_ETH_CTRL_IDX          (0u)
/*! Decimal weight coefficient used for calculation of PPB adjustment average (0.1 = 10%). */
#define GPTP_PORT_NEW_PPB_WEIGHT        (0.1)
/*! Ethernet header offset. */
#define GPTP_PORT_ETH_II_OFST           (0u)
/*! Float number comparison precision. */
#define GPTP_PORT_EPSILON               (0.00000001)

/*******************************************************************************
 * Data types
 ******************************************************************************/

/*!
 * @brief PTP time base information.
 */
typedef struct
{
    /* Time base is incremented evertime the PTP CLK slave changes its phase
       or frequency. */
    uint16_t u16TimeBaseIndicator;
    /* Change of phase during the last time base indicator increment (i.e.,
       current time offset between local (corrected) clock
       and the Grand Master). */
    gptp_def_timestamp_sig_t rLastPhaseChange;
    /* The last phase change had positive/negative offset. */
    bool bNegativePhase;
    /* Fractional frequency change (i.e., frequency change expressed as a pure
       fraction) during the last time base indicator increment. */
    float64_t f64LastFreqChange;
} gptp_port_ptp_tb_info_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static Eth_RateRatioType GPTP_PORT_PPBToPseudoRatio(int32_t i32PPB);

/*******************************************************************************
 * Variables
 ******************************************************************************/

static gptp_def_map_info_t     srPortMap;
static int32_t                 si32PPBadjustmentAverage;
static gptp_port_ptp_tb_info_t srTimeBaseInfo;

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief           This function converts PPB to pseudo ratio
*                  (frequency ratio).
 *
 * @details         This function converts PPB to pseudo ratio which is used for
 *                  clock timer frequency adjustment by ETH driver.
 *
 * @param[in]       i32PPB Parts per billion.
 *
 * @return          Eth_RateRatioType as a pseudo ratio.
 *
 * @ requirements 529199
 * @ requirements 539908
*/
static Eth_RateRatioType GPTP_PORT_PPBToPseudoRatio(int32_t i32PPB)
{
    Eth_RateRatioType rPseudoRatio;

    rPseudoRatio.IngressTimeStampDelta.diff.seconds = 1u;
    rPseudoRatio.IngressTimeStampDelta.diff.secondsHi = 0u;
    rPseudoRatio.IngressTimeStampDelta.sign = true;

    rPseudoRatio.OriginTimeStampDelta.diff.seconds = 1u;
    rPseudoRatio.OriginTimeStampDelta.diff.secondsHi = 0u;
    rPseudoRatio.OriginTimeStampDelta.sign = true;

    if (0 < i32PPB)
    {
        rPseudoRatio.IngressTimeStampDelta.diff.nanoseconds = (uint32_t)i32PPB;
        rPseudoRatio.OriginTimeStampDelta.diff.nanoseconds = 0u;
    }
    else
    {
        rPseudoRatio.OriginTimeStampDelta.diff.nanoseconds = (uint32_t)(GPTP_INTERNAL_AbsInt64ToUint64((int64_t)i32PPB));
        rPseudoRatio.IngressTimeStampDelta.diff.nanoseconds = 0u;
    }

    return rPseudoRatio;
}

/*!
 * @brief           This function finds gPTP port number in the port map.
 *
 * @details         The function finds the gPTP port number in the port map by
 *                  the Switch index and Switch port index.
 *
 * @param[out]      pu8GptpPort Pointer to gPTP port number
 * @param[in]       u8PortOnSw  Number of the respective port on switch.
 * @param[in]       u8Sw        Number of the respective switch.
 *
 * @return          Error status.
 * @retval          GPTP_ERR_OK: The frame was successfully found.
 * @retval          GPTP_ERR_V_NULL_PTR: Empty or invalid input arguments.
 * @retval          GPTP_ERR_API_ILLEGAL_PORT_NUM: Wrong port number.
 *
 * @ requirements 529199
 * @ requirements 529250
*/
gptp_err_type_t GPTP_PORT_PortLookup(uint8_t *pu8GptpPort,
                                     uint8_t u8PortOnSw,
                                     uint8_t u8Sw)
{
    (void)u8Sw;

    gptp_err_type_t eError;
    uint8_t         u8PortLookUp;

    eError = GPTP_ERR_API_ILLEGAL_PORT_NUM;

    if (NULL != pu8GptpPort)
    {
        for (u8PortLookUp = 0u; u8PortLookUp < srPortMap.u8NumOfGptpPorts; u8PortLookUp++)
        {
            if (srPortMap.prMapTable[u8PortLookUp].u8SwitchPort == u8PortOnSw)
            {
                /* Store found port. */
                *pu8GptpPort = u8PortLookUp;
                eError = GPTP_ERR_OK;
                /* Port has been found, stop the search loop. */
                break;
            }
        }
    }
    else
    {
        eError = GPTP_ERR_V_NULL_PTR;
    }

    return eError;
}

/*!
 * @brief           This function registers multicast MAC.
 *
 * @details         This function provides interface to register MAC in
 *                  the Ethernet module so the multicast MAC is forwarded.
 *
 * @param[in]       cpu8MacAddr Pointer to multicast MAC address.
 *
 * @ requirements 529199
 * @ requirements 529251
*/
void GPTP_PORT_MulticastForward(const uint8_t *cpu8MacAddr)
{
    uint8_t u8CtrlIdx;

    for (u8CtrlIdx = 0u; u8CtrlIdx < ETH_43_GMAC_MAX_CTRLIDX_SUPPORTED; u8CtrlIdx++)
    {
        (void)Eth_43_GMAC_UpdatePhysAddrFilter(u8CtrlIdx, cpu8MacAddr,
                                               ETH_ADD_TO_FILTER);
    }
}

/*!
 * @brief           This function initializes port module.
 *
 * @details         This function initializes port module with providing port
 *                  map table to the module.
 *
 * @param[in]       cprPortTable Pointer to the Port Map Table.
 * @param[in]       u8NumOfPorts Count of gPTP ports.
 *
 * @ requirements 529199
 * @ requirements 529252
*/
void GPTP_PORT_PortMapInit(const gptp_def_map_table_t *cprPortTable,
                           uint8_t u8NumOfPorts)
{
    srPortMap.u8NumOfGptpPorts = u8NumOfPorts;
    srPortMap.prMapTable = cprPortTable;
}

/*!
 * @brief           This function gets Time Base Information from the switch
 *                  driver.
 *
 * @details         This function gets Time Base Information from the switch
 *                  driver and provides it to the gPTP stack.
 *
 * @param[out]      pu16GmTimeBaseIndicator Pointer to the GM Time Base
 *                  Indicator variable which is updated by calling of this
 *                  function.
 * @param[out]      pu32LastGmPhaseChangeH Pointer to the Last GM Phase Change
 *                  (High) variable which is updated by calling of
 *                  this function.
 * @param[out]      pu64LastGmPhaseChangeL Pointer to the Last GM Phase Change
 *                  (Low) variable which is updated by calling of this function.
 * @param[out]      pu32ScaledLastGmFreqChange Pointer to the Scaled Last GM
 *                  Frequency Change variable which is updated by calling of
 *                  this function.
 *
 * @ requirements 529199
 * @ requirements 529253
*/
void GPTP_PORT_TimeBaseInfoGet(uint16_t *pu16GmTimeBaseIndicator,
                               uint32_t *pu32LastGmPhaseChangeH,
                               uint64_t *pu64LastGmPhaseChangeL,
                               uint32_t *pu32ScaledLastGmFreqChange)
{
    uint64_t  u64Seconds;
    uint64_t  au64Input[6];
    uint64_t  au64Output[6];
    float64_t f64Temp;

    /* GM time base indicator. */
    *pu16GmTimeBaseIndicator = srTimeBaseInfo.u16TimeBaseIndicator;

    /* Last GM phase change high and low. */
    u64Seconds = GPTP_INTERNAL_AbsInt64ToUint64(GPTP_INTERNAL_TsToS64(srTimeBaseInfo.rLastPhaseChange));
    for (uint16_t u16Cycle = 0u; u16Cycle < 6u; u16Cycle++)
    {
        /* Get lower 16 bits from seconds. */
        au64Input[u16Cycle] = u64Seconds & GPTP_DEF_BIT_MASK_48L_16H;
        /* Convert to nanoseconds. */
        au64Input[u16Cycle] = au64Input[u16Cycle] * GPTP_DEF_NS_IN_SECONDS;

        /* The first cycle. */
        if (0u == u16Cycle)
        {
            /* Add value form the time timestamp. */
            au64Input[u16Cycle] = au64Input[u16Cycle] + GPTP_INTERNAL_AbsInt64ToUint64((int64_t)srTimeBaseInfo.rLastPhaseChange.s32TimeStampNs);
        }
        /* Other cycles. */
        else
        {
            /* Add value from the previous cycle. */
            au64Input[u16Cycle] = au64Input[u16Cycle] + au64Input[(u16Cycle - 1u)];
        }

        /* Get lower 16 bits from input. */
        au64Output[u16Cycle] = au64Input[u16Cycle] & GPTP_DEF_BIT_MASK_48L_16H;
        /* Shift the input. */
        au64Input[u16Cycle] = au64Input[u16Cycle] >> 16u;

        /* Shift seconds for the next cycle. */
        u64Seconds = u64Seconds >> 16u;
    }

    *pu64LastGmPhaseChangeL = 0u | ((((uint64_t)au64Output[0] & (uint64_t)GPTP_DEF_BIT_MASK_48L_16H) << 16u) | \
                                    (((uint64_t)au64Output[1] & (uint64_t)GPTP_DEF_BIT_MASK_48L_16H) << 32u) | \
                                    (((uint64_t)au64Output[2] & (uint64_t)GPTP_DEF_BIT_MASK_48L_16H) << 48u));

    *pu32LastGmPhaseChangeH = (((uint32_t)au64Output[3] & (uint32_t)GPTP_DEF_BIT_MASK_48L_16H) | \
                               (((uint32_t)au64Output[4] & (uint32_t)GPTP_DEF_BIT_MASK_48L_16H) << 16u));

    /* Negative phase. */
    if (true == srTimeBaseInfo.bNegativePhase)
    {
        /* Multiplication by -1 using integer manipulation on number represented
           by two's complement (i.e., bitwise NOT changes the number's sign
           and then subtracts one). */
        *pu64LastGmPhaseChangeL = (~(*pu64LastGmPhaseChangeL)) + 1u;
        *pu32LastGmPhaseChangeH = (~(*pu32LastGmPhaseChangeH)) + 1u;
    }

    /* Scaled last GM frequency change. */
    f64Temp = srTimeBaseInfo.f64LastFreqChange * (float64_t)GPTP_DEF_POW_2_41;
    *pu32ScaledLastGmFreqChange = (uint32_t)((int32_t)f64Temp);
}

/*!
 * @brief           This function writes data to the NVM.
 *
 * @details         This function writes data to the NVM using flash driver.
 *
 * @param[in]       prPdelMachine Pointer to respective Pdelay machine.
 * @param[in]       eNvmDataType NVM data type to access.
 * @param[in]       cprGptp Pointer to global gPTP structure.
 *
 * @ requirements 529199
 * @ requirements 529254
 * @ requirements 152449
*/
void GPTP_PORT_ValueNvmWrite(gptp_def_pdelay_t *prPdelMachine,
                             gptp_def_nvm_data_t eNvmDataType,
                             const gptp_def_data_t *cprGptp)

{
    gptp_err_type_t         eError;
    gptp_def_mem_write_stat *peWriteStat;
    const float64_t         *cpf64Value;

    switch (eNvmDataType)
    {
        /* PropagationDelay write request. */
        case GPTP_DEF_NVM_PDELAY:
            cpf64Value = &(prPdelMachine->f64NeighborPropDelay);
            peWriteStat = &(prPdelMachine->rPdelayNvmWriteStat);
            break;
        /* NeighbourRateRatio write request. */
        case GPTP_DEF_NVM_RRATIO:
            cpf64Value = &(prPdelMachine->f64NeighborRateRatio);
            peWriteStat = &(prPdelMachine->rRratioNvmWriteStat);
            break;
        /* Undefined write request. */
        default:
            cpf64Value = NULL;
            peWriteStat = NULL;
            GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF,
                              GPTP_ERR_DOMAIN_NOT_SPECIF,
                              GPTP_ERR_N_UNABLE_NVM_WRITE,
                              GPTP_ERR_SEQ_ID_NOT_SPECIF);
            break;
    }

    if ((NULL != cprGptp->rPtpStackCallBacks.pfNvmWriteCB) &&
        (NULL != cpf64Value))
    {
        eError = (gptp_err_type_t)cprGptp->rPtpStackCallBacks.pfNvmWriteCB(prPdelMachine->u8GptpPort,
                                                                           eNvmDataType,
                                                                           *cpf64Value,
                                                                           peWriteStat);
        if (GPTP_ERR_OK != eError)
        {
            GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF,
                              GPTP_ERR_DOMAIN_NOT_SPECIF,
                              GPTP_ERR_N_UNABLE_NVM_WRITE,
                              GPTP_ERR_SEQ_ID_NOT_SPECIF);
        }
    }
}

/*!
 * @brief           This function reads data from the NVM.
 *
 * @details         This function reads data from the NVM using flash driver.
 *
 * @param[in]       prPdelMachine Pointer to respective Pdelay machine.
 * @param[in]       eNvmDataType NVM data type to access.
 * @param[in]       cprGptp Pointer to global gPTP structure.
 *
 * @ requirements 529199
 * @ requirements 529255
 * @ requirements 152449
*/
void GPTP_PORT_ValueNvmRead(gptp_def_pdelay_t *prPdelMachine,
                            gptp_def_nvm_data_t eNvmDataType,
                            const gptp_def_data_t *cprGptp)
{
    gptp_err_type_t eError;
    float64_t       *pf64Value;

    switch (eNvmDataType)
    {
        /* PropagationDelay read request. */
        case GPTP_DEF_NVM_PDELAY:
            pf64Value = &(prPdelMachine->f64NeighborPropDelay);
            break;
        /* NeighbourRateRatio read request. */
        case GPTP_DEF_NVM_RRATIO:
            pf64Value = &(prPdelMachine->f64NeighborRateRatio);
            break;
        /* Undefined read request. */
        default:
            pf64Value = NULL;
            GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF,
                              GPTP_ERR_DOMAIN_NOT_SPECIF,
                              GPTP_ERR_N_UNABLE_NVM_READ,
                              GPTP_ERR_SEQ_ID_NOT_SPECIF);
            break;
    }

    if ((NULL != cprGptp->rPtpStackCallBacks.pfNvmReadCB) &&
        (NULL != pf64Value))
    {
        eError = (gptp_err_type_t)cprGptp->rPtpStackCallBacks.pfNvmReadCB(prPdelMachine->u8GptpPort,
                                                                          eNvmDataType,
                                                                          pf64Value);
        if (GPTP_ERR_OK != eError)
        {
            /* Assign NaN value. */
            *pf64Value = ((float64_t)0.0f / (float64_t)0.0f);
            GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF,
                              GPTP_ERR_DOMAIN_NOT_SPECIF,
                              GPTP_ERR_N_UNABLE_NVM_READ,
                              GPTP_ERR_SEQ_ID_NOT_SPECIF);
        }
    }
}

/*!
 * @brief           This function sends PTP message.
 *
 * @details         This function sends PTP message using the ENET driver.
 *
 * @param[in]       u8Port gPTP port on which the PTP message is transmitted.
 * @param[in]       u8FramePrio Eth frame priority.
 * @param[in]       cprTxData Pointer to the structure containing data to send.
 * @param[in]       u8FrameId Frame ID of the transmitted message.
 *
 * @return          Error status,
 * @retval          GPTP_ERR_OK: The frame was successfully transmitted.
 * @retval          GPTP_ERR_V_NULL_PTR: Empty or invalid input arguments.
 * @retval          GPTP_ERR_M_MSG_BUFF_PTR_NULL: Transmit buffer was not
 *                  provided by eth driver.
 * @retval          GPTP_ERR_F_FRAME_SEND: The frame was not successfully
 *                  transmitted.
 *
 * @ requirements 529199
 * @ requirements 529256
*/
gptp_err_type_t GPTP_PORT_MsgSend(uint8_t u8Port,
                                  uint8_t u8FramePrio,
                                  const gptp_def_tx_data_t *cprTxData,
                                  uint8_t u8FrameId)
{
    Std_ReturnType        eStatus;
    BufReq_ReturnType     eBufStatus;
    gptp_err_type_t       eError;
    static Eth_BufIdxType seBuffIdx;
    static uint8_t        *spu8Buffer;
    static uint16_t       su16BufferLength;
    uint16_t              u16PTPFramePayloadLength;
    uint8_t               u8PhyPort;

    /* Error initial value. */
    spu8Buffer = NULL;
    eError = GPTP_ERR_OK;

    /* Check input arguments. */
    if ((NULL != cprTxData) && (cprTxData->u8FrameLength > GPTP_DEF_ETH_II_LEN))
    {
        /* Get real port ID from map table. */
        u8PhyPort = srPortMap.prMapTable[u8Port].u8SwitchPort;

        /* Payload length calculation of the gPTP frame (without eth header). */
        u16PTPFramePayloadLength = ((uint16_t)cprTxData->u8FrameLength - GPTP_DEF_ETH_II_LEN);
        su16BufferLength = u16PTPFramePayloadLength;

        /* Get transmit buffer to store gPTP frame payload. */
        eBufStatus = Eth_43_GMAC_ProvideTxBuffer(u8PhyPort, u8FramePrio,
                                                 &seBuffIdx, &spu8Buffer,
                                                 &su16BufferLength);

        /* The buffer is provided with desired length. */
        if ((BUFREQ_OK == eBufStatus) &&
            (NULL != spu8Buffer) &&
            (su16BufferLength >= u16PTPFramePayloadLength))
        {
            /* Copy only payload of the gPTP generated frame to buffer provided
               by Eth_43 driver. */
            for (uint16_t i = 0u; i < u16PTPFramePayloadLength; i++)
            {
                spu8Buffer[i] = cprTxData->pau8TxBuffPtr[GPTP_DEF_ETH_II_LEN + i];
            }

            /* If timestmp for this frame is requested by the gPTP stack. */
            if (true == cprTxData->bTsRequested)
            {
                /* Instruct the Ethernet driver to capture timestamp of egress
                   ethernet frame associated with buffIdx. */
                Eth_43_GMAC_EnableEgressTimeStamp(u8PhyPort, seBuffIdx);

                /* Only store frame metadata if egress timestamp is required
                   by stack. */
                if (NULL != cprTxData->prFrameMap)
                {
                    /* Store frame metadata before transmission.
                       Timestamp in metadata is inserted in TxConfirmation. */
                    cprTxData->prFrameMap->u8PtpFrameId = u8FrameId;
                    cprTxData->prFrameMap->u32BufferIndex = seBuffIdx;
                    cprTxData->prFrameMap->eTsEntryStatus = GPTP_DEF_TS_MAP_ENTRY_ENQUEUED;
                }
            }

            /* Enable transmission, destination MAC address is taken from
               generated PTP frame, which contains full eth header. */
            eStatus = Eth_43_GMAC_Transmit(u8PhyPort, seBuffIdx,
                                           GPTP_FR_ETH_TYPE_PTP, true,
                                           u16PTPFramePayloadLength,
                                           &cprTxData->pau8TxBuffPtr[0]);
            if ((Std_ReturnType)E_OK != eStatus)
            {
                /* Transmission falied. */
                eError = GPTP_ERR_F_FRAME_SEND;
            }
        }
        else
        {
            /* No Tx buffer. */
            eError = GPTP_ERR_M_MSG_BUFF_PTR_NULL;
        }
    }
    else
    {
        /* Invalid input arguments. */
        eError = GPTP_ERR_V_NULL_PTR;
    }

    return eError;
}

/*!
 * @brief           This function reads current link status of the gPTP port.
 *
 * @details         This function reads current link status of the gPTP port
 *                  using Switch driver.
 *
 * @param[in]       u8Port gPTP port number.
 * @param[out]      pbStat Pointer to the status variable, updated in
 *                  the function.
 *
 * @return          Error status.
 * @retval          GPTP_ERR_OK: The link status is successfully provided.
 * @retval          GPTP_ERR_V_NULL_PTR: Empty or invalid input arguments.
 * @retval          GPTP_ERR_I_NO_LINK_STATUS: Failed to read link status.
 *
 * @ requirements 529199
 * @ requirements 539909
*/
gptp_err_type_t GPTP_PORT_GetLinkStatus(uint8_t u8Port,
                                        bool *pbStat)
{
    gptp_err_type_t              eError;
    uint8_t                      u8TrcvIdx;
    static EthTrcv_LinkStateType seLinkState;

    /* Get real port ID from map table. */
    u8TrcvIdx = srPortMap.prMapTable[u8Port].u8SwitchPort;
    /* Init variables. */
    eError = GPTP_ERR_V_NULL_PTR;

    /* Check pointer validity. */
    if (NULL != pbStat)
    {
        /* Read link state. */
        if ((Std_ReturnType)E_OK == EthTrcv_GetLinkState(u8TrcvIdx, &seLinkState))
        {
            eError = GPTP_ERR_OK;

            if (ETHTRCV_LINK_STATE_ACTIVE == seLinkState)
            {
                /* Link is up. */
                *pbStat = true;
            }
            else
            {
                /* Link is down. */
                *pbStat = false;
            }
        }
        else
        {
            /* Failed to read link status. */
            eError = GPTP_ERR_I_NO_LINK_STATUS;
        }
    }

    return eError;
}

/*!
 * @brief           This function gets current Time Stamp time.
 *
 * @details         This function gets current Time Stamp time from the Switch
 *                  driver.
 *
 * @param[in]       eTsType Type of the Time Stamp (free-running of corrected).
 *
 * @return          gptp_def_timestamp_t with the current Time Stamp time.
 *
 * @ requirements 529199
 * @ requirements 529257
*/
gptp_def_timestamp_t GPTP_PORT_CurrentTimeGet(gptp_def_ts_type_t eTsType)
{
    static Eth_TimeStampQualType seTimeStampQuality;
    static Eth_TimeStampType     srEthTimestamp;
    Std_ReturnType               eStatus;
    gptp_def_timestamp_t         rTimeStamp;
    uint64_t                     u64FreeRunningCapture;

    switch (eTsType)
    {
        case GPTP_DEF_TS_FREERUNNING:
            u64FreeRunningCapture = GPTP_PORT_GetFreeRunningTimer();
            rTimeStamp.u64TimeStampS = u64FreeRunningCapture / (uint64_t)GPTP_DEF_NS_IN_SECONDS;
            rTimeStamp.u32TimeStampNs = (uint32_t)((uint64)u64FreeRunningCapture % (uint64)GPTP_DEF_NS_IN_SECONDS);
            break;

        case GPTP_DEF_TS_CORRECTED:
            eStatus = Eth_43_GMAC_GetCurrentTime(GPTP_PORT_ETH_CTRL_IDX,
                                                 &seTimeStampQuality,
                                                 &srEthTimestamp);
            if (((Std_ReturnType)E_OK == eStatus) &&
                (ETH_VALID == seTimeStampQuality))
            {
                rTimeStamp.u64TimeStampS = (((uint64_t)srEthTimestamp.secondsHi & (uint64_t)GPTP_DEF_BIT_MASK_16L_16H) << 32u) | (uint64_t)srEthTimestamp.seconds;
                rTimeStamp.u32TimeStampNs = srEthTimestamp.nanoseconds;
            }
            else
            {
                rTimeStamp.u64TimeStampS = 0u;
                rTimeStamp.u32TimeStampNs = 0u;
            }
            break;

        default:
            rTimeStamp.u64TimeStampS = 0u;
            rTimeStamp.u32TimeStampNs = 0u;
            break;
    }

    return rTimeStamp;
}

/*!
 * @brief           This function reads MAC Address of the gPTP port.
 *
 * @details         This function reads MAC Address of the gPTP port using
 *                  the Switch driver.
 *
 * @param[in]       u8Port gPTP port number.
 * @param[out]      pu64Mac Pointer to the MAC Address variable, updated in
 *                  the function.
 *
 * @return          Error status.
 * @retval          GPTP_ERR_OK: MAC Address of the gPTP port was successfully
 *                  obtained.
 * @retval          GPTP_ERR_V_NULL_PTR: Empty or invalid input arguments.
 * @retval          GPTP_ERR_API_ILLEGAL_PORT_NUM: Wrong port number.
 *
 * @ requirements 529199
 * @ requirements 529260
 * @ requirements 539907
*/
gptp_err_type_t GPTP_PORT_ObtainPortMac(uint8_t u8Port,
                                        uint64_t *pu64Mac)
{
    gptp_err_type_t eError;

    if (NULL != pu64Mac)
    {
        if (u8Port < srPortMap.u8NumOfGptpPorts)
        {
            Eth_43_GMAC_GetPhysAddr(srPortMap.prMapTable[u8Port].u8SwitchPort,
                                    (uint8_t *)pu64Mac);
            eError = GPTP_ERR_OK;
        }
        else
        {
            eError = GPTP_ERR_API_ILLEGAL_PORT_NUM;
        }
    }
    else
    {
        eError = GPTP_ERR_V_NULL_PTR;
    }

    return eError;
}

/*!
 * @brief           This function provides time synchronization information.
 *
 * @details         This function provides time synchronization information
 *                  received from the Grand Master to the Switch driver.
 *
 * @param[in]       prGptp Pointer to global gPTP structure.
 * @param[in]       cu8DomainNumber Domain number of the received Follow Up
 *                  message.
 * @param[in]       cpf64RateRatio Rate Ratio.
 * @param[in]       crOffset Current time offset between local (corrected) clock
 *                  and the Grand Master.
 * @param[in]       cbNegative Positive/negative offset.
 * @param[in]       ci8SyncIntervalLog Current Sync interval.
 * @param[out]      pbUpdated Pointer to flag signalizing stack the clock is
 *                  updated.
 *
 * @return          Error status.
 * @retval          GPTP_ERR_OK: The local clock is successfully updated.
 * @retval          GPTP_ERR_F_UPDATE_PTP: The local clock cannot be updated.
 *
 * @ requirements 529199
 * @ requirements 529261
*/
gptp_err_type_t GPTP_PORT_UpdateLocalClock(gptp_def_data_t *prGptp,
                                           const uint8_t cu8DomainNumber,
                                           const float64_t *cpf64RateRatio,
                                           const gptp_def_timestamp_sig_t crOffset,
                                           const bool cbNegative,
                                           const int8_t ci8SyncIntervalLog,
                                           bool *pbUpdated)
{
    gptp_err_type_t                 eError;
    static gptp_def_timestamp_sig_t srOffsetTmp;
    /* Holds time offset when applying time offset correction. */
    static Eth_TimeIntDiffType      srTimeStampDiff;
    static Eth_RateRatioType        srPseudoRatio;
    /* Placeholder for gPTP stack computed rate ratio. */
    static float64_t                sf64sRateRatio;
    static bool                     sbNegativeOfst;
    static int8_t                   si8SyncIntLog;
    static float64_t                f64RatioOld;
    float64_t                       f64RatioNew;
    int64_t                         i64PtpClkOffset;
    bool                            bUpdateClock;
    uint64_t                        u64Offset;
    int32_t                         i32PPBAdjustment;
    int32_t                         i32timeErrorNs;

    /* Local variable initialization with default value. */
    sf64sRateRatio = *cpf64RateRatio;
    sbNegativeOfst = cbNegative;
    bUpdateClock = false;
    si8SyncIntLog = ci8SyncIntervalLog;
    srOffsetTmp = crOffset;
    eError = GPTP_ERR_OK;
    f64RatioOld = 0.0;

    /* Call domain selection callback if configured. */
    if (NULL != prGptp->rPtpStackCallBacks.pfDomSelectionCB)
    {
        bUpdateClock = prGptp->rPtpStackCallBacks.pfDomSelectionCB(cu8DomainNumber,
                                                                   &sf64sRateRatio,
                                                                   &srOffsetTmp,
                                                                   &sbNegativeOfst,
                                                                   &si8SyncIntLog);
    }
    else
    {
        if (0u == cu8DomainNumber)
        {
            bUpdateClock = true;
        }
    }

    /* Clock update if requested. */
    if (true == bUpdateClock)
    {
        /* Store the reported offset to device params. */
        prGptp->rPerDeviceParams.rReportedOffset = crOffset;
        prGptp->rPerDeviceParams.bReportedOffsetNeg = cbNegative;
        prGptp->rPerDeviceParams.f64CorrClockRateRatio = sf64sRateRatio;

        u64Offset = GPTP_INTERNAL_AbsInt64ToUint64(crOffset.s64TimeStampS * (int64_t)GPTP_DEF_NS_IN_SECONDS);
        u64Offset += GPTP_INTERNAL_AbsInt64ToUint64((int64_t)crOffset.s32TimeStampNs);

        /* Absolute time offset correction. */
        if (u64Offset > prGptp->rPerDeviceParams.u64PIControllerMaxThreshold)
        {
            /* Calculate absolute time correction. */
            srTimeStampDiff.diff.secondsHi = (uint16_t)((uint32_t)(((uint64_t)crOffset.s64TimeStampS & (uint64_t)GPTP_DEF_BIT_MASK_32H_32L) >> 32u ) & (uint32_t)GPTP_DEF_BIT_MASK_16L_16H);
            srTimeStampDiff.diff.seconds = (uint32_t)((uint64_t)crOffset.s64TimeStampS & (uint64_t)GPTP_DEF_BIT_MASK_32L_32H);
            srTimeStampDiff.diff.nanoseconds = (uint32_t)crOffset.s32TimeStampNs;
            srTimeStampDiff.sign = !cbNegative;

            /* Calculate relative PPB correction from rateRatio reported by
               gPTP stack. */
            i32PPBAdjustment = GPTP_INTERNAL_Float64ToInt32((1.0 - sf64sRateRatio) * (float64_t)GPTP_DEF_NS_IN_SECONDS);

            /* Origin of average calculation. */
            si32PPBadjustmentAverage = i32PPBAdjustment;

            GPTP_PI_Clear();
        }
        /* PI controller mode adjusting oscilator PPM in range +-50 PPM from
           reference frequency. */
        else
        {
            /* Must be cleared (does not need to adjust absolute time offset). */
            srTimeStampDiff.diff.nanoseconds = 0u;
            srTimeStampDiff.diff.seconds = 0u;
            srTimeStampDiff.diff.secondsHi = 0u;
            srTimeStampDiff.sign = true;

            /* Convert crOffset to signed integer. */
            i32timeErrorNs = cbNegative ? crOffset.s32TimeStampNs : -crOffset.s32TimeStampNs;

            /* Calculate the PI Controller output for this control loop cycle. */
            i32PPBAdjustment = GPTP_PI_Update(ci8SyncIntervalLog,
                                              i32timeErrorNs);

            /* Calculate weighted average. */
            si32PPBadjustmentAverage = GPTP_INTERNAL_Float64ToInt32(((1.0 - GPTP_PORT_NEW_PPB_WEIGHT) * (float64_t)si32PPBadjustmentAverage) + (GPTP_PORT_NEW_PPB_WEIGHT * (float64_t)i32PPBAdjustment));
        }

        /* Covert PPB to pseudo ratio. */
        srPseudoRatio = GPTP_PORT_PPBToPseudoRatio(i32PPBAdjustment);

        /* Apply time and frequency correction to the GMAC timer. */
        if ((Std_ReturnType)E_OK == Eth_43_GMAC_SetCorrectionTime(GPTP_PORT_ETH_CTRL_IDX, &srTimeStampDiff, &srPseudoRatio))
        {
            /* Notification to gPTP stack, clock was successfully updated. */
            *pbUpdated = true;
            prGptp->rPerDeviceParams.bEverUpdated = true;
        }
        else
        {
            /* Notification to gPTP stack, clock was not successfully updated. */
            *pbUpdated = false;
            prGptp->rPerDeviceParams.bEverUpdated = false;
            eError = GPTP_ERR_F_UPDATE_PTP;
        }

        /* Update time base info structure. */
        f64RatioNew = ((float64_t)i32PPBAdjustment / 1000000000.0);
        i64PtpClkOffset = GPTP_INTERNAL_TsToS64(srOffsetTmp);
        /* The offset has changed or the old and new ratios are not the same. */
        if ((0 != i64PtpClkOffset) ||
            !(((f64RatioOld - f64RatioNew) < GPTP_PORT_EPSILON) &&
              ((f64RatioNew - f64RatioOld) < GPTP_PORT_EPSILON)))
        {
            srTimeBaseInfo.u16TimeBaseIndicator++;
            srTimeBaseInfo.rLastPhaseChange = srOffsetTmp;
            srTimeBaseInfo.bNegativePhase = sbNegativeOfst;
            srTimeBaseInfo.f64LastFreqChange = f64RatioNew;
        }

        f64RatioOld = f64RatioNew;
    }

    return eError;
}

/*!
 * @brief           This function stabilizes the local clock.
 *
 * @details         This function stabilizes the local clock running.
 *                  The stabilized clock runs at the current rate ratio.
 *                  It prevents clock from drifting, in case of Grand Master
 *                  loss.
 *
 * @param[in]       cpf64RateRatio Rate Ratio.
 *
 * @return          Error status
 * @retval          GPTP_ERR_OK: The Local clock is successfully stabilized.
 * @retval          GPTP_ERR_F_UPDATE_PTP: The Local clock cannot be stabilized.
 *
 * @ requirements 529199
 * @ requirements 529262
*/
gptp_err_type_t GPTP_PORT_FixLocalClock(const float64_t *cpf64RateRatio)
{
    (void)*cpf64RateRatio;

    gptp_err_type_t            eError;
    static Eth_TimeIntDiffType srTimeStampDiff;
    static Eth_RateRatioType   srPseudoRatio;

    /* Local variable initialization with default value. */
    srTimeStampDiff.diff.nanoseconds = 0u;
    srTimeStampDiff.diff.seconds = 0u;
    srTimeStampDiff.diff.secondsHi = 0u;
    srTimeStampDiff.sign = true;

    /* Use current average value of PI Controller instead of RateRatio. */
    srPseudoRatio = GPTP_PORT_PPBToPseudoRatio(si32PPBadjustmentAverage);

    /* Adjust only the frequency of local GMAC timer. */
    if ((Std_ReturnType)E_OK == Eth_43_GMAC_SetCorrectionTime(GPTP_PORT_ETH_CTRL_IDX, &srTimeStampDiff, &srPseudoRatio))
    {
        eError = GPTP_ERR_OK;
    }
    else
    {
        eError = GPTP_ERR_F_UPDATE_PTP;
    }

    return eError;
}

/*!
 * @brief           This function gets current time.
 *
 * @details         This function gets current Time Stamp(free running) clock
 *                  time and current PTP(corrected) clock time.
 *                  These Time Stamps are taken at once.
 *
 * @param[out]      prFreeRunClk Pointer to the free running clock variable,
 *                  updated in the function.
 * @param[out]      prCorrectedClk Pointer to the corrected clock variable,
 *                  updated in the function.
 *
 * @return          GPTP_ERR_OK in case the time is successfully read.
 *
 * @ requirements 529199
 * @ requirements 529263
*/
gptp_err_type_t GPTP_PORT_GetSwitchTimes(gptp_def_timestamp_t *prFreeRunClk,
                                         gptp_def_timestamp_t *prCorrectedClk)
{
    static Eth_TimeStampQualType seTimeStampQuality;
    static Eth_TimeStampType     srTimeStamp;
    gptp_err_type_t              eError;
    Std_ReturnType               eStatus;

    /* Capture HW GMAC timer timestamp. */
    eStatus = Eth_43_GMAC_GetCurrentTime(GPTP_PORT_ETH_CTRL_IDX,
                                         &seTimeStampQuality, &srTimeStamp);
    if (((Std_ReturnType)E_OK == eStatus) && (ETH_VALID == seTimeStampQuality))
    {
        /* Check address of destination buffers. */
        if ((NULL != prFreeRunClk) && (NULL != prCorrectedClk))
        {
            /* Calculate corrected timestamp. */
            prCorrectedClk->u64TimeStampS = (((uint64_t)srTimeStamp.secondsHi & (uint64_t)GPTP_DEF_BIT_MASK_16L_16H) << 32u) | (uint64_t)srTimeStamp.seconds;
            prCorrectedClk->u32TimeStampNs = srTimeStamp.nanoseconds;

            /* S32K3 does not have freeRunning HW timer, so use the corrected
               one from GMAC. */
            prFreeRunClk->u64TimeStampS = prCorrectedClk->u64TimeStampS;
            prFreeRunClk->u32TimeStampNs = prCorrectedClk->u32TimeStampNs;

            eError = GPTP_ERR_OK;
        }
        else
        {
            eError = GPTP_ERR_I_COUNTERS_NULL_PTR;
        }
    }
    else
    {
        eError = GPTP_ERR_F_TIME_READ;
    }

    return eError;
}

/*!
 * @brief           This function reads TX management object to obtain TX
 *                  timestamp.
 *
 * @details         This function reads TX management object which have been
 *                  identified as open. If the object is owned by UPPER LAYER
 *                  and contains valid egress timestamp, the timestamp
 *                  is provided to the timestamp handler.
 *
 * @param[in]       prFrameMap pointer to the frame map with open buffer index.
*/
void GPTP_PORT_ProcessMapEntry(gptp_def_tx_frame_map_t *prFrameMap)
{
    if (GPTP_DEF_TS_MAP_ENTRY_CNFRMD == prFrameMap->eTsEntryStatus)
    {
        if (GPTP_DEF_BUFF_INDEX_CLOSED != prFrameMap->u32BufferIndex)
        {
            if (GPTP_DEF_FRAME_INDEX_CLOSED != prFrameMap->u8PtpFrameId)
            {
                /* Pass the egress timestamp to gPTP stack together with
                   previously assigned frame id. */
                GPTP_TimeStampHandler(prFrameMap->u8EgressPort,
                                      prFrameMap->u32EgressTimeStampSeconds,
                                      prFrameMap->u32EgressTimeStampNanoseconds,
                                      prFrameMap->u8PtpFrameId);

                /* Set flag in buffer if and frame id as closed. */
                prFrameMap->u32BufferIndex = GPTP_DEF_BUFF_INDEX_CLOSED;
                prFrameMap->u8PtpFrameId = GPTP_DEF_FRAME_INDEX_CLOSED;
                /* Clear timestamp. */
                prFrameMap->u32EgressTimeStampNanoseconds = 0u;
                prFrameMap->u32EgressTimeStampSeconds = 0u;
                /* Mark the entry as free for next frame transmitted frame. */
                prFrameMap->eTsEntryStatus = GPTP_DEF_TS_MAP_ENTRY_UNUSED;
            }
        }
    }
}

/*!
 * @brief           This function executes platform specific functionality of
 *                  the Periodic function.
 *
 * @details         This function executes platform specific functionality of
 *                  the Periodic function. This function may be left empty or it
 *                  can perform the time stamp polling by the frame buffer and
 *                  frame table index.
 *
 * @param[in]       prGptp Pointer to global gPTP structure.
 *
 * @ requirements 529199
 * @ requirements 529264
*/
void GPTP_PORT_TimerPeriodic(gptp_def_data_t *prGptp)
{
    uint8_t                 u8Port;
    uint8_t                 u8Machine;
    uint8_t                 u8Domain;
    const gptp_def_pdelay_t *cprPdelay;
    const gptp_def_domain_t *cprDomain;
    const gptp_def_sync_t   *cprSync;

    /* TS polling. */
    for (u8Port = 0u; u8Port < prGptp->rPerDeviceParams.u8NumberOfPdelayMachines; u8Port++)
    {
        cprPdelay = &prGptp->prPdelayMachines[u8Port];

        /* Those Pdelay machines that are initiator enabled. */
        if (true == cprPdelay->bPdelayInitiatorEnabled)
        {
            /* If a buffer index is assigned. */
            if (GPTP_DEF_BUFF_INDEX_CLOSED != cprPdelay->rTxDataReq.prFrameMap->u32BufferIndex)
            {
                /* Check if TS is available and call TS handler. */
                GPTP_PORT_ProcessMapEntry(cprPdelay->rTxDataReq.prFrameMap);
            }
        }
        /* Those Pdelay machines that are responder enabled. */
        if (true == cprPdelay->bPdelayResponderEnabled)
        {
            /* If a buffer index is assigned. */
            if (GPTP_DEF_BUFF_INDEX_CLOSED != cprPdelay->rTxDataResp.prFrameMap->u32BufferIndex)
            {
                /* Check if TS is available and call TS handler. */
                GPTP_PORT_ProcessMapEntry(cprPdelay->rTxDataResp.prFrameMap);
            }
        }
    }

    /* For each domain. */
    for (u8Domain = 0u; u8Domain < prGptp->rPerDeviceParams.u8NumberOfDomains; u8Domain++)
    {
        cprDomain = &prGptp->prPerDomainParams[u8Domain];

        /* For each sync machine on domain. */
        for (u8Machine = 0u; u8Machine < cprDomain->u8NumberOfSyncsPerDom; u8Machine++)
        {
            cprSync = &cprDomain->prSyncMachines[u8Machine];
            /* If a buffer index is assigned. */
            if (GPTP_DEF_BUFF_INDEX_CLOSED != cprSync->rTxDataSyn.prFrameMap->u32BufferIndex)
            {
                /* Check if TS is available and call TS handler. */
                GPTP_PORT_ProcessMapEntry(cprSync->rTxDataSyn.prFrameMap);
            }
        }
    }
}

/*!
 * @brief           This function initializes platform part of
 *                  gptp_def_pdelay_t struct.
 *
 * @details         This function executes platform specific parts of
 *                  the initialization for each Path delay machine.
 *
 * @param[in]       cu8Port gPTP port on which the PTP message is transmitted.
 * @param[in]       cprPdelayInit Pointer to gPTP Pdelay Initialization.
 * @param[in]       prPdelay Pointer to gPTP INTERNAL stucture - Pdelay machine.
 *
 * @return          GPTP_ERR_OK in case the structure has been set.
 *
 * @ requirements 529199
 * @ requirements 529265
*/
gptp_err_type_t GPTP_PORT_InitPDelayStruct(const uint8_t cu8Port,
                                           const gptp_def_init_params_pdel_t *cprPdelayInit,
                                           gptp_def_pdelay_t *prPdelay)
{
    gptp_err_type_t eError;

    /* Error initial value. */
    eError = GPTP_ERR_OK;

    /* Check if prFrameMapReq and prFrameMapResp are not null. */
    if ((NULL != cprPdelayInit->prFrameMapReq) &&
        (NULL != cprPdelayInit->prFrameMapResp))
    {
        prPdelay->rTxDataReq.prFrameMap = cprPdelayInit->prFrameMapReq;
        prPdelay->rTxDataResp.prFrameMap = cprPdelayInit->prFrameMapResp;
        prPdelay->rTxDataReFu.prFrameMap = NULL;
    }
    else
    {
        if (NULL == cprPdelayInit->prFrameMapReq)
        {
            eError = GPTP_ERR_I_PD_STRCT_NULL_PTR;
            GPTP_ERR_Register(cu8Port, GPTP_ERR_DOMAIN_NOT_SPECIF,
                              GPTP_ERR_I_PD_STRCT_NULL_PTR,
                              GPTP_ERR_SEQ_ID_NOT_SPECIF);
        }

        if (NULL == cprPdelayInit->prFrameMapResp)
        {
            eError = GPTP_ERR_I_PD_STRCT_NULL_PTR;
            GPTP_ERR_Register(cu8Port, GPTP_ERR_DOMAIN_NOT_SPECIF,
                              GPTP_ERR_I_PD_STRCT_NULL_PTR,
                              GPTP_ERR_SEQ_ID_NOT_SPECIF);
        }
    }

    prPdelay->rTxDataReq.prFrameMap->u32BufferIndex = GPTP_DEF_BUFF_INDEX_CLOSED;
    prPdelay->rTxDataReq.prFrameMap->u8PtpFrameId = GPTP_DEF_FRAME_INDEX_CLOSED;
    prPdelay->rTxDataResp.prFrameMap->u32BufferIndex = GPTP_DEF_BUFF_INDEX_CLOSED;
    prPdelay->rTxDataResp.prFrameMap->u8PtpFrameId = GPTP_DEF_FRAME_INDEX_CLOSED;

    return eError;
}

/*!
 * @brief           This function initializes platform part of
 *                  gptp_def_domain_t struct.
 *
 * @details         This function executes platform specific parts of
 *                  the initialization for each Domain.
 *
 * @param[in]       cu8Domain Respective Domain number.
 * @param[in]       cprDomainInit Pointer to gPTP Domain parameters
 *                  initialization.
 * @param[in]       prDomain Pointer to gPTP INTERNAL stucture - Per domain
 *                  parameters.
 *
 * @return          GPTP_ERR_OK in case the structure has been set.
 *
 * @ requirements 529199
 * @ requirements 529266
*/
gptp_err_type_t GPTP_PORT_InitDomainStruct(const uint8_t cu8Domain,
                                           const gptp_def_init_params_domain_t *cprDomainInit,
                                           gptp_def_domain_t *prDomain)
{
    (void)cu8Domain;
    (void)cprDomainInit;

    prDomain->rTxDataSig.prFrameMap = NULL;

    return GPTP_ERR_OK;
}

/*!
 * @brief           This function initializes platform part of gptp_def_sync_t
 *                  struct.
 *
 * @details         This function executes platform specific parts of
 *                  the initialization for each Sync machine.
 *
 * @param[in]       cu8Machine Sync machine index for the domain
 * @param[in]       cu8Domain Respective Domain number
 * @param[in]       cprSyncInit Pointer to Sync initialization parameters
 * @param[in]       prSync Pointer to respective Sync machine structure
 *
 * @return          GPTP_ERR_OK in case the structure has been set.
 *
 * @ requirements 529199
 * @ requirements 529267
*/
gptp_err_type_t GPTP_PORT_InitSyncStruct(const uint8_t cu8Machine,
                                         const uint8_t cu8Domain,
                                         const gptp_def_init_params_sync_t *cprSyncInit,
                                         gptp_def_sync_t *prSync)
{
    gptp_err_type_t eError;

    /* Error initial value. */
    eError = GPTP_ERR_OK;

    /* Check if prFrameMapSync is not null. */
    if (NULL != cprSyncInit->prFrameMapSync)
    {
        prSync->rTxDataSyn.prFrameMap = cprSyncInit->prFrameMapSync;
        prSync->rTxDataFup.prFrameMap = NULL;
    }
    else
    {
        eError = GPTP_ERR_I_SYNC_STRCT_NULL_PTR;
        GPTP_ERR_Register(cu8Machine, cu8Domain, GPTP_ERR_I_SYNC_STRCT_NULL_PTR,
                          GPTP_ERR_SEQ_ID_NOT_SPECIF);
    }

    prSync->rTxDataSyn.prFrameMap->u32BufferIndex = GPTP_DEF_BUFF_INDEX_CLOSED;
    prSync->rTxDataSyn.prFrameMap->u8PtpFrameId = GPTP_DEF_FRAME_INDEX_CLOSED;

    return eError;
}

/*!
 * @brief           This function returns Ethernet II offset.
 *
 * @details         This function provides frame offset to the Eth II part of
 *                  the frame.
 *
 * @return          Ethernet II offset.
 *
 * @ requirements 529199
 * @ requirements 529268
*/
uint8_t GPTP_PORT_FrameRxGetOffset(void)
{
    /* No offset, because porting layer operates on the 2nd network layer,
       so ethernet header always starts on the first byte. */
    return (uint8_t)GPTP_PORT_ETH_II_OFST;
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
