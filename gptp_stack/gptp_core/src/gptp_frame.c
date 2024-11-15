/*
 * Copyright 2019-2024 NXP
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
 * @file gptp_frame.c
 *
 */

#include "gptp_def.h"
#include "gptp_err.h"
#include "gptp_frame.h"
#include "gptp_port.h"
#include "gptp_internal.h"

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static uint8_t GPTP_MD_Uint8ToArray(uint8_t au8Out[1],
                                    uint8_t u8Input,
                                    uint8_t u8Offset);

static uint8_t GPTP_MD_Uint16ToArray(uint8_t au8Out[2],
                                     uint16_t u16Input,
                                     uint8_t u8Offset);

static uint8_t GPTP_MD_Uint24ToArray(uint8_t au8Out[3],
                                     uint32_t u24Input,
                                     uint8_t u8Offset);

static uint8_t GPTP_MD_Uint32ToArray(uint8_t au8Out[4],
                                     uint32_t u32Input,
                                     uint8_t u8Offset);

static uint8_t GPTP_MD_Uint48ToArray(uint8_t au8Out[6],
                                     uint64_t u64Input,
                                     uint8_t u8Offset);

static uint8_t GPTP_MD_Uint64ToArray(uint8_t au8Out[8],
                                     uint64_t u64Input,
                                     uint8_t u8Offset);

static uint8_t GPTP_MD_ArrayToUint8(const uint8_t au8In[1]);

static int8_t GPTP_MD_ArrayToSint8(const uint8_t au8In[1]);

static uint16_t GPTP_MD_ArrayToUint16(const uint8_t au8In[2]);

static uint32_t GPTP_MD_ArrayToUint32(const uint8_t au8In[4]);

static uint64_t GPTP_MD_ArrayToUint48(const uint8_t au8In[6]);

static uint64_t GPTP_MD_ArrayToUint64(const uint8_t au8In[8]);

/*******************************************************************************
 * Local Functions
 ******************************************************************************/

  /*FUNCTION**********************************************************************
 *
 * Function Name : GPTP_MD_Uint8ToArray
 * Description   : The function converts uint8 value into the uint8 array.
 *                 The function increments the offset for the next usage by the
 *                 number of bytes used in the conversion.
 *
 *END**************************************************************************/
static uint8_t GPTP_MD_Uint8ToArray(uint8_t au8Out[1],
                                    uint8_t u8Input,
                                    uint8_t u8Offset)
{
    uint8_t OffsetOut;

    au8Out[0] = (uint8_t)(u8Input);

    /* Increment the offset by 1 */
    OffsetOut = u8Offset + 1u;
    return (OffsetOut);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : GPTP_MD_Uint16ToArray
 * Description   : The function converts uint16 value into the uint8 array.
 *                 The function increments the offset for the next usage by the
 *                 number of bytes used in the conversion.
 *
 *END**************************************************************************/
static uint8_t GPTP_MD_Uint16ToArray(uint8_t au8Out[2],
                                     uint16_t u16Input,
                                     uint8_t u8Offset)
{
    uint8_t OffsetOut;

    au8Out[0] = (uint8_t)((u16Input >> 8u) & (uint16_t)GPTP_DEF_BIT_MASK_24L_8H);
    au8Out[1] = (uint8_t)((u16Input) & (uint16_t)GPTP_DEF_BIT_MASK_24L_8H);

    /* Increment the offset by 2 */
    OffsetOut = u8Offset + 2u;
    return (OffsetOut);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : GPTP_MD_Uint24ToArray
 * Description   : The function converts uint32 value into the uint8 array.
 *                 It uses only 3 bytes from the data array.
 *                 The function increments the offset for the next usage by the
 *                 number of bytes used in the conversion.
 *
 *END**************************************************************************/
static uint8_t GPTP_MD_Uint24ToArray(uint8_t au8Out[3],
                                     uint32_t u24Input,
                                     uint8_t u8Offset)
{
    uint8_t OffsetOut;

    au8Out[0] = (uint8_t)((u24Input >> 16u) & (uint32_t)GPTP_DEF_BIT_MASK_24L_8H);
    au8Out[1] = (uint8_t)((u24Input >> 8u) & (uint32_t)GPTP_DEF_BIT_MASK_24L_8H);
    au8Out[2] = (uint8_t)((u24Input) & (uint32_t)GPTP_DEF_BIT_MASK_24L_8H);

    /* Increment the offset by 3 */
    OffsetOut = u8Offset + 3u;
    return (OffsetOut);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : GPTP_MD_Uint32ToArray
 * Description   : The function converts uint32 value into the uint8 array.
 *                 The function increments the offset for the next usage by the
 *                 number of bytes used in the conversion.
 *
 *END**************************************************************************/
static uint8_t GPTP_MD_Uint32ToArray(uint8_t au8Out[4],
                              uint32_t u32Input,
                              uint8_t u8Offset)
{
    uint8_t OffsetOut;

    au8Out[0] = (uint8_t)((u32Input >> 24u) & (uint32_t)GPTP_DEF_BIT_MASK_24L_8H);
    au8Out[1] = (uint8_t)((u32Input >> 16u) & (uint32_t)GPTP_DEF_BIT_MASK_24L_8H);
    au8Out[2] = (uint8_t)((u32Input >> 8u) & (uint32_t)GPTP_DEF_BIT_MASK_24L_8H);
    au8Out[3] = (uint8_t)((u32Input) & (uint32_t)GPTP_DEF_BIT_MASK_24L_8H);

    /* Increment the offset by 4 */
    OffsetOut = u8Offset + 4u;
    return (OffsetOut);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : GPTP_MD_Uint48ToArray
 * Description   : The function converts uint64 value into the uint8 array.
 *                 It uses only 6 bytes from the data array.
 *                 The function increments the offset for the next usage by the
 *                 number of bytes used in the conversion.
 *
 *END**************************************************************************/
static uint8_t GPTP_MD_Uint48ToArray(uint8_t au8Out[6],
                                     uint64_t u64Input,
                                     uint8_t u8Offset)
{
    uint8_t OffsetOut;

    au8Out[0] = (uint8_t)((u64Input >> 40u) & (uint64_t)GPTP_DEF_BIT_MASK_56L_8H);
    au8Out[1] = (uint8_t)((u64Input >> 32u) & (uint64_t)GPTP_DEF_BIT_MASK_56L_8H);
    au8Out[2] = (uint8_t)((u64Input >> 24u) & (uint64_t)GPTP_DEF_BIT_MASK_56L_8H);
    au8Out[3] = (uint8_t)((u64Input >> 16u) & (uint64_t)GPTP_DEF_BIT_MASK_56L_8H);
    au8Out[4] = (uint8_t)((u64Input >> 8u) & (uint64_t)GPTP_DEF_BIT_MASK_56L_8H);
    au8Out[5] = (uint8_t)((u64Input) & (uint64_t)GPTP_DEF_BIT_MASK_56L_8H);

    /* Increment the offset by 6 */
    OffsetOut = u8Offset + 6u;
    return (OffsetOut);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : GPTP_MD_Uint64ToArray
 * Description   : The function converts uint64 value into the uint8 array.
 *                 The function increments the offset for the next usage by the
 *                 number of bytes used in the conversion.
 *
 *END**************************************************************************/
static uint8_t GPTP_MD_Uint64ToArray(uint8_t au8Out[8],
                                     uint64_t u64Input,
                                     uint8_t u8Offset)
{
    uint8_t OffsetOut;

    au8Out[0] = (uint8_t)((u64Input >> 56u) & (uint64_t)GPTP_DEF_BIT_MASK_56L_8H);
    au8Out[1] = (uint8_t)((u64Input >> 48u) & (uint64_t)GPTP_DEF_BIT_MASK_56L_8H);
    au8Out[2] = (uint8_t)((u64Input >> 40u) & (uint64_t)GPTP_DEF_BIT_MASK_56L_8H);
    au8Out[3] = (uint8_t)((u64Input >> 32u) & (uint64_t)GPTP_DEF_BIT_MASK_56L_8H);
    au8Out[4] = (uint8_t)((u64Input >> 24u) & (uint64_t)GPTP_DEF_BIT_MASK_56L_8H);
    au8Out[5] = (uint8_t)((u64Input >> 16u) & (uint64_t)GPTP_DEF_BIT_MASK_56L_8H);
    au8Out[6] = (uint8_t)((u64Input >> 8u) & (uint64_t)GPTP_DEF_BIT_MASK_56L_8H);
    au8Out[7] = (uint8_t)((u64Input) & (uint64_t)GPTP_DEF_BIT_MASK_56L_8H);

    /* Increment the offset by 8 */
    OffsetOut = u8Offset + 8u;
    return (OffsetOut);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : GPTP_MD_ArrayToUint8
 * Description   : The function converts the uint8 array into the uint8.
 *
 *END**************************************************************************/
static uint8_t GPTP_MD_ArrayToUint8(const uint8_t au8In[1])
{
    uint8_t u8Out;

    u8Out = au8In[0];

    return u8Out;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : GPTP_MD_ArrayToUint8
 * Description   : The function converts the uint8 array into the uint8.
 *
 *END**************************************************************************/
static int8_t GPTP_MD_ArrayToSint8(const uint8_t au8In[1])
{
    int8_t s8Out;

    s8Out = (int8_t)au8In[0];

    return s8Out;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : GPTP_MD_ArrayToUint16
 * Description   : The function converts the uint8 array into the uint16.
 *
 *END**************************************************************************/
static uint16_t GPTP_MD_ArrayToUint16(const uint8_t au8In[2])
{
    uint16_t u16Out;

    u16Out = ((uint16_t)(((uint16_t)au8In[0]) << 8u) | \
                          (uint16_t)au8In[1]);

    return u16Out;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : GPTP_MD_ArrayToUint32
 * Description   : The function converts the uint8 array into the uint32.
 *
 *END**************************************************************************/
static uint32_t GPTP_MD_ArrayToUint32(const uint8_t au8In[4])
{
    uint32_t u32Out;

    u32Out = (uint32_t)((((uint32_t)au8In[0]) << 24u) | \
                        (((uint32_t)au8In[1]) << 16u) | \
                        (((uint32_t)au8In[2]) << 8u)  | \
                         ((uint32_t)au8In[3]));

    return u32Out;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : GPTP_MD_ArrayToUint48
 * Description   : The function converts the uint8 array into the uint64.
 *                 Only 6 fields from the array are used.
 *
 *END**************************************************************************/
static uint64_t GPTP_MD_ArrayToUint48(const uint8_t au8In[6])
{
    uint64_t u48Out;

    u48Out = (uint64_t)((((uint64_t)au8In[0]) << 40u) | \
                        (((uint64_t)au8In[1]) << 32u) | \
                        (((uint64_t)au8In[2]) << 24u) | \
                        (((uint64_t)au8In[3]) << 16u) | \
                        (((uint64_t)au8In[4]) << 8u)  | \
                         ((uint64_t)au8In[5]));

    return u48Out;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : GPTP_MD_ArrayToUint64
 * Description   : The function converts the uint8 array into the uint64.
 *
 *END**************************************************************************/
static uint64_t GPTP_MD_ArrayToUint64(const uint8_t au8In[8])
{
    uint64_t u64Out;

    u64Out = (uint64_t)((((uint64_t)au8In[0]) << 56u) | \
                        (((uint64_t)au8In[1]) << 48u) | \
                        (((uint64_t)au8In[2]) << 40u) | \
                        (((uint64_t)au8In[3]) << 32u) | \
                        (((uint64_t)au8In[4]) << 24u) | \
                        (((uint64_t)au8In[5]) << 16u) | \
                        (((uint64_t)au8In[6]) << 8u)  | \
                         ((uint64_t)au8In[7]));

    return u64Out;
}

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief The function parses received PTP ethernet frame.
 *
 * The function parses received PTP ethernet frame.
 *
 * @param[in] rRxData structure containing all the necessary gPTP data for processing.
 * @param[in] prGptp pointer to gPTP global data structure
 * @param[in] prMessageTypeRcvd Message type received
 * @param[in] pu8DomainRcvd pointer to domain for received data
 * @param[in] pu8MachineRcvd pointer to machine for received data
 *
 * @return Error information
 *
 * @ requirements 529199
 * @ requirements 529210
 * @ requirements 152448
 * @ requirements 152455
 * @ requirements 162145
 * @ requirements 120464
 */

gptp_err_type_t GPTP_FRAME_ParseRx(gptp_def_rx_data_t rRxData,
                                   const gptp_def_data_t *prGptp,
                                   gptp_def_msg_type_t  *prMessageTypeRcvd,
                                   uint8_t *pu8DomainRcvd,
                                   uint8_t *pu8MachineRcvd)
{
    uint8_t                        u8HdrOffset;
    uint16_t                       u16TempSeqId;
    uint8_t                        u8TransportSpecific;
    uint32_t                       u32OrganizationSubType;
    gptp_err_type_t                eError;
    gptp_def_msg_type_t            rTempMsgId;
    uint8_t                        u8Seek;
    uint8_t                        u8DomainNum;
    uint8_t                        u8SyncMachineId = 0u;
    bool                           bDomainFound;
    bool                           bMachineFound;
    gptp_def_domain_t              *prDomain;
    gptp_def_sync_t                *prSyncMachine;
    gptp_def_pdelay_t              *prPdelayMachine;
    uint16_t                       u16VlanTci;
    uint16_t                       u16EthType;

    /* Initialization of variable, before it's used (will be loaded with correct address later) */
    prDomain = &prGptp->prPerDomainParams[0];

    /* Initialization of variable, before it's used (will be loaded with correct address later) */
    prSyncMachine = &prDomain->prSyncMachines[0];

    /* Initialization of variable, before it's used (will be loaded with correct address later) */
    prPdelayMachine = &prGptp->prPdelayMachines[0];

    /* No error */
    eError = GPTP_ERR_OK;

    /* Initialize TCI value */
    u16VlanTci = 0u;

    /* ETH II offset */
    u8HdrOffset = GPTP_PORT_FrameRxGetOffset();
    u16EthType = rRxData.u16EthType;

    /* If VLAN enabled */
    if (true == prGptp->rPerDeviceParams.bVlanEnabled)
    {
        /* If VLAN ETH type */
        if (GPTP_FR_ETH_TYPE_VLAN == u16EthType)
        {
            /* Store the TCI */
            u16VlanTci = GPTP_MD_ArrayToUint16(&(rRxData.cpu8RxData)[u8HdrOffset]);
            /* Increment the offset */
            u8HdrOffset = u8HdrOffset + GPTP_DEF_ETH_VLAN_LEN;

            /* If not PTP ETH type, register error don't allow to process the frame */
            u16EthType = GPTP_MD_ArrayToUint16(&(rRxData.cpu8RxData)[u8HdrOffset]);
            if (GPTP_FR_ETH_TYPE_PTP != u16EthType)
            {
                GPTP_ERR_Register(rRxData.u8PtpPort, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_M_NO_ETH_TYPE_PTP, 0u);
                eError = GPTP_ERR_M_NO_ETH_TYPE_PTP;
            }
        }

        /* If not VLAN ETH type */
        else
        {
            /* Register error */
            GPTP_ERR_Register(rRxData.u8PtpPort, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_M_NO_ETH_TYPE_VLAN, 0u);
            /* If not PTP ETH type, don't allow to process the message */
            if (GPTP_FR_ETH_TYPE_PTP != u16EthType)
            {
                eError = GPTP_ERR_M_NO_ETH_TYPE_VLAN;
            }
        }
    }
    /* If VLAN not enabled */
    else
    {
        /* If not PTP ETH Type, register error and don't allow to process the message */
        if (GPTP_FR_ETH_TYPE_PTP != u16EthType)
        {
            GPTP_ERR_Register(rRxData.u8PtpPort, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_M_NO_ETH_TYPE_PTP, 0u);
            eError = GPTP_ERR_M_NO_ETH_TYPE_PTP;
        }
    }

    if (GPTP_ERR_OK == eError)
    {
        /* Default error */
        eError = GPTP_ERR_M_MESSAGE_ID_INVALID;

        /* Temporary asignment of the Message ID to distinguish which message is going to be processed */
        rTempMsgId = (gptp_def_msg_type_t)(GPTP_MD_ArrayToUint8(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_MSG_ID_OFFSET]) & 0x0Fu);
        /* Get transport specific byte (nibble) */
        u8TransportSpecific = (GPTP_MD_ArrayToUint8(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_MSG_ID_OFFSET]) & 0xF0u);

        *prMessageTypeRcvd = GPTP_DEF_MSG_TYPE_UNKNOWN;
        *pu8DomainRcvd = 255u;
        *pu8MachineRcvd = 255u;

        switch (rTempMsgId)
        {
            /* Sync message */
            case GPTP_DEF_MSG_TYPE_SYNC:

                /* If transport specific value is equal to 1 */
                if ((uint8_t)GPTP_DEF_TRANSPORT_SPEC_1 == u8TransportSpecific)
                {
                    /* Clear default error */
                    eError = GPTP_ERR_OK;

                    /* Get domain number on wire */
                    u8DomainNum = GPTP_MD_ArrayToUint8(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_DOMAIN_NUM]);
                    /* Translate domain number on wire to logical domains of the gPTP stack */
                    bDomainFound = false;
                    /* Search for respective domain */
                    for (u8Seek = 0u; u8Seek < prGptp->rPerDeviceParams.u8NumberOfDomains; u8Seek++)
                    {
                        if (false == bDomainFound)
                        {
                            prDomain = &prGptp->prPerDomainParams[u8Seek];
                            if (u8DomainNum == prDomain->u8DomainNumber)
                            {
                                bDomainFound = true;
                                /* Save domain index into the domain number (temporary) */
                                u8DomainNum = u8Seek;
                            }
                        }
                    }

                    if (true == bDomainFound)
                    {
                        /* Find, to which sync machine the message belongs, by searching for the gPTP port */
                        bMachineFound = false;
                        /* Search for respective machine */
                        for (u8Seek = 0u; u8Seek < prDomain->u8NumberOfSyncsPerDom; u8Seek++)
                        {
                            if (false == bMachineFound)
                            {
                                prSyncMachine = &prDomain->prSyncMachines[u8Seek];
                                if (rRxData.u8PtpPort == prSyncMachine->u8GptpPort)
                                {
                                    bMachineFound = true;
                                    u8SyncMachineId = u8Seek;
                                }
                            }
                        }

                        if (true == bMachineFound)
                        {
                            /* Parse the data from the Sync message and store into the RxData structure */
                            prSyncMachine->rSyncMsgRx.rHeader.eMsgId = (gptp_def_msg_type_t)(GPTP_MD_ArrayToUint8(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_MSG_ID_OFFSET]) & 0x0Fu);
                            /* Used for all outgoing messages */
                            prSyncMachine->rSyncMsgRx.rHeader.u64CorrectionSubNs = GPTP_MD_ArrayToUint64(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_CORR_OFFSET]);
                            prSyncMachine->rSyncMsgRx.rHeader.u16SequenceId = GPTP_MD_ArrayToUint16(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SEQ_ID_OFFSET]);
                            prSyncMachine->rSyncMsgRx.rHeader.u16SourcePortId = GPTP_MD_ArrayToUint16(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SC_PORT_ID_OFFSET]) - 1u;
                            prSyncMachine->rSyncMsgRx.rHeader.u64SourceClockId = GPTP_MD_ArrayToUint64(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SC_CLK_ID_OFFSET]);
                            prSyncMachine->rSyncMsgRx.rHeader.s8MessagePeriodLog = GPTP_MD_ArrayToSint8(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_MSG_PER_LOG_OFFSET]);
                            prSyncMachine->rSyncMsgRx.rSyncRxTs.u32TimeStampNs = rRxData.u32TsNsec;
                            prSyncMachine->rSyncMsgRx.rSyncRxTs.u64TimeStampS = rRxData.u32TsSec;
                            prSyncMachine->rSyncMsgRx.u16VlanTci = u16VlanTci;

                            *pu8DomainRcvd = u8DomainNum;
                            *pu8MachineRcvd = u8SyncMachineId;
                            *prMessageTypeRcvd = GPTP_DEF_MSG_TYPE_SYNC;

                            /* Set flag - message has been received */
                            prSyncMachine->bSyncMsgReceived = true;
#ifdef GPTP_COUNTERS
                            GPTP_INTERNAL_IncrementPortStats(prGptp, rRxData.u8PtpPort, ieee8021AsPortStatRxSyncCount);
                            GPTP_INTERNAL_IncrementDomainStats(prGptp, u8DomainNum, u8SyncMachineId, ieee8021AsPortStatRxSyncCount);
#endif /* GPTP_COUNTERS */
                        }
                        else
                        {
                            GPTP_ERR_Register(u8SyncMachineId, u8DomainNum, GPTP_ERR_M_SYNC_MACHINE_UNKNOWN, \
                                              GPTP_MD_ArrayToUint16(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SEQ_ID_OFFSET]));
                            eError = GPTP_ERR_M_SYNC_MACHINE_UNKNOWN;
#ifdef GPTP_COUNTERS
                            GPTP_INTERNAL_IncrementPortStats(prGptp, rRxData.u8PtpPort, ieee8021AsPortStatRxPTPPacketDiscard);
#endif /* GPTP_COUNTERS */
                        }
                    }

                    else
                    {
                        GPTP_ERR_Register(u8SyncMachineId, u8DomainNum, GPTP_ERR_M_SYNC_DOMAIN_UNKNOWN, \
                                          GPTP_MD_ArrayToUint16(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SEQ_ID_OFFSET]));
                        eError = GPTP_ERR_M_SYNC_DOMAIN_UNKNOWN;
#ifdef GPTP_COUNTERS
                        GPTP_INTERNAL_IncrementPortStats(prGptp, rRxData.u8PtpPort, ieee8021AsPortStatRxPTPPacketDiscard);
#endif /* GPTP_COUNTERS */
                    }
                }

                else /* Transport specific other than 1 */
                {
                    /* Discard the message */
                    /* Log the error into the error log */
                    GPTP_ERR_Register(rRxData.u8PtpPort, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_M_TRANSPORT_SPECIFIC, \
                                      GPTP_MD_ArrayToUint16(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SEQ_ID_OFFSET]));
                    eError = GPTP_ERR_M_TRANSPORT_SPECIFIC;
#ifdef GPTP_COUNTERS
                    GPTP_INTERNAL_IncrementPortStats(prGptp, rRxData.u8PtpPort, ieee8021AsPortStatRxPTPPacketDiscard);
#endif /* GPTP_COUNTERS */
                }
            break;

            /* Propagation delay request message */
            case GPTP_DEF_MSG_TYPE_PD_REQ:
                /* If transport specific value is equal to 1 */
                if (((uint8_t)GPTP_DEF_TRANSPORT_SPEC_1 == u8TransportSpecific) || (true == prGptp->rPerDeviceParams.bSdoIdCompatibilityMode))
                {
                    /* Clear default error */
                    eError = GPTP_ERR_OK;
                    if (rRxData.u8PtpPort < prGptp->rPerDeviceParams.u8NumberOfPdelayMachines)
                    {
                        prPdelayMachine = &prGptp->prPdelayMachines[rRxData.u8PtpPort];

                        /* Parse the data from the Propagation delay request message and store into the RxData structure */
                        prPdelayMachine->rPdReqMsgRx.rHeader.eMsgId = (gptp_def_msg_type_t)(GPTP_MD_ArrayToUint8(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_MSG_ID_OFFSET]) & 0x0Fu);
                        prPdelayMachine->rPdReqMsgRx.rHeader.u16SequenceId = GPTP_MD_ArrayToUint16(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SEQ_ID_OFFSET]);
                        prPdelayMachine->rPdReqMsgRx.rHeader.u16SourcePortId = GPTP_MD_ArrayToUint16(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SC_PORT_ID_OFFSET]) - 1u;
                        prPdelayMachine->rPdReqMsgRx.rHeader.u64SourceClockId = GPTP_MD_ArrayToUint64(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SC_CLK_ID_OFFSET]);
                        prPdelayMachine->rPdReqMsgRx.rHeader.s8MessagePeriodLog = GPTP_MD_ArrayToSint8(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_MSG_PER_LOG_OFFSET]);
                        prPdelayMachine->rPdReqMsgRx.rT2Ts.u32TimeStampNs = rRxData.u32TsNsec;
                        prPdelayMachine->rPdReqMsgRx.rT2Ts.u64TimeStampS = rRxData.u32TsSec;
                        prPdelayMachine->rPdReqMsgRx.u8MajorSdoId = (u8TransportSpecific >> 4u);
                        prPdelayMachine->rPdReqMsgRx.u8SubdomainNum = GPTP_MD_ArrayToUint8(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_DOMAIN_NUM]);
                        prPdelayMachine->rPdReqMsgRx.u16VlanTci = u16VlanTci;

                        *pu8MachineRcvd = rRxData.u8PtpPort;
                        *prMessageTypeRcvd = GPTP_DEF_MSG_TYPE_PD_REQ;

                        /* Set flag - message has been received */
                        prPdelayMachine->bPdelayReqReceived = true;

#ifdef GPTP_COUNTERS
                        GPTP_INTERNAL_IncrementPortStats(prGptp, rRxData.u8PtpPort, ieee8021AsPortStatRxPdelayRequest);
#endif /* GPTP_COUNTERS */
                    }

                    else
                    {
                        GPTP_ERR_Register(rRxData.u8PtpPort, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_M_PDEL_MACHINE_UNKNOWN, \
                                          GPTP_MD_ArrayToUint16(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SEQ_ID_OFFSET]));
                        eError = GPTP_ERR_M_PDEL_MACHINE_UNKNOWN;
#ifdef GPTP_COUNTERS
                        GPTP_INTERNAL_IncrementPortStats(prGptp, rRxData.u8PtpPort, ieee8021AsPortStatRxPTPPacketDiscard);
#endif /* GPTP_COUNTERS */
                    }
                }

                else /* Transport specific other than 1 */
                {
                    /* Discard the message */
                    /* Log the error into the error log */
                    GPTP_ERR_Register(rRxData.u8PtpPort, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_M_TRANSPORT_SPECIFIC, \
                                      GPTP_MD_ArrayToUint16(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SEQ_ID_OFFSET]));
                    eError = GPTP_ERR_M_TRANSPORT_SPECIFIC;
#ifdef GPTP_COUNTERS
                    GPTP_INTERNAL_IncrementPortStats(prGptp, rRxData.u8PtpPort, ieee8021AsPortStatRxPTPPacketDiscard);
#endif /* GPTP_COUNTERS */
                }
            break;

            /* Propagation delay response message */
            case GPTP_DEF_MSG_TYPE_PD_RESP:
                /* If transport specific value is equal to 1 */
                if (((uint8_t)GPTP_DEF_TRANSPORT_SPEC_1 == u8TransportSpecific) || (true == prGptp->rPerDeviceParams.bSdoIdCompatibilityMode))
                {
                    /* Clear default error */
                    eError = GPTP_ERR_OK;

                    if (rRxData.u8PtpPort < prGptp->rPerDeviceParams.u8NumberOfPdelayMachines)
                    {
                        /* Temporary sequence Id assignment */
                        u16TempSeqId = GPTP_MD_ArrayToUint16(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SEQ_ID_OFFSET]);

                        prPdelayMachine = &prGptp->prPdelayMachines[rRxData.u8PtpPort];

                        /* If the sequence Id is the not same as in previously received message, OK */
                        if (prPdelayMachine->rPdReqMsgRx.rHeader.u16SequenceId != u16TempSeqId)
                        {
                            /* Parse the data from the propagation delay response message and store into the RxData structure */
                            prPdelayMachine->rPdRespMsgRx.rHeader.eMsgId = (gptp_def_msg_type_t)(GPTP_MD_ArrayToUint8(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_MSG_ID_OFFSET]) & 0x0Fu);
                            prPdelayMachine->rPdRespMsgRx.rHeader.u16SequenceId = GPTP_MD_ArrayToUint16(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SEQ_ID_OFFSET]);
                            prPdelayMachine->rPdRespMsgRx.rHeader.u16SourcePortId = GPTP_MD_ArrayToUint16(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SC_PORT_ID_OFFSET]) - 1u;
                            prPdelayMachine->rPdRespMsgRx.rHeader.u64SourceClockId = GPTP_MD_ArrayToUint64(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SC_CLK_ID_OFFSET]);
                            prPdelayMachine->rPdRespMsgRx.rHeader.s8MessagePeriodLog = GPTP_MD_ArrayToSint8(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_MSG_PER_LOG_OFFSET]);
                            prPdelayMachine->rPdRespMsgRx.rRequestingId.u64ClockId = GPTP_MD_ArrayToUint64(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_RQ_CLK_ID_OFFSET]);
                            prPdelayMachine->rPdRespMsgRx.rRequestingId.u16PortId = GPTP_MD_ArrayToUint16(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_RQ_PORT_ID_OFFSET]) - 1u;
                            prPdelayMachine->rPdRespMsgRx.rT2Ts.u64TimeStampS = GPTP_MD_ArrayToUint48(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_TS_S_OFFSET]);
                            prPdelayMachine->rPdRespMsgRx.rT2Ts.u32TimeStampNs = GPTP_MD_ArrayToUint32(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_TS_NS_OFFSET]);
                            prPdelayMachine->rPdRespMsgRx.rT4Ts.u32TimeStampNs = rRxData.u32TsNsec;
                            prPdelayMachine->rPdRespMsgRx.rT4Ts.u64TimeStampS = rRxData.u32TsSec;

                            *pu8MachineRcvd = rRxData.u8PtpPort;
                            *prMessageTypeRcvd = GPTP_DEF_MSG_TYPE_PD_RESP;

                            /* Set flag - message has been received */
                            prPdelayMachine->bPdelayRespReceived = true;
#ifdef GPTP_COUNTERS
                            GPTP_INTERNAL_IncrementPortStats(prGptp, rRxData.u8PtpPort, ieee8021AsPortStatRxPdelayResponse);
#endif /* GPTP_COUNTERS */
                        }
                        /* If the same sequence id has been received in previous message, error */
                        else
                        {
                            /* Log the error into the error log */
                            GPTP_ERR_Register(rRxData.u8PtpPort, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_M_PDEL_RESP_DBL_RCVD, \
                                              GPTP_MD_ArrayToUint16(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SEQ_ID_OFFSET]));
                            eError = GPTP_ERR_M_PDEL_RESP_DBL_RCVD;
#ifdef GPTP_COUNTERS
                            GPTP_INTERNAL_IncrementPortStats(prGptp, rRxData.u8PtpPort, ieee8021AsPortStatRxPTPPacketDiscard);
#endif /* GPTP_COUNTERS */
                        }
                    }

                    else
                    {
                        GPTP_ERR_Register(rRxData.u8PtpPort, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_M_PDEL_MACHINE_UNKNOWN, \
                                          GPTP_MD_ArrayToUint16(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SEQ_ID_OFFSET]));
                        eError = GPTP_ERR_M_PDEL_MACHINE_UNKNOWN;
#ifdef GPTP_COUNTERS
                        GPTP_INTERNAL_IncrementPortStats(prGptp, rRxData.u8PtpPort, ieee8021AsPortStatRxPTPPacketDiscard);
#endif /* GPTP_COUNTERS */
                    }
                }

                else /* Transport specific other than 1 */
                {
                    /* Discard the message */
                    /* Log the error into the error log */
                    GPTP_ERR_Register(rRxData.u8PtpPort, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_M_TRANSPORT_SPECIFIC, \
                                      GPTP_MD_ArrayToUint16(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SEQ_ID_OFFSET]));
                    eError = GPTP_ERR_M_TRANSPORT_SPECIFIC;
#ifdef GPTP_COUNTERS
                    GPTP_INTERNAL_IncrementPortStats(prGptp, rRxData.u8PtpPort, ieee8021AsPortStatRxPTPPacketDiscard);
#endif /* GPTP_COUNTERS */
                }
            break;

            /* Follow up message */
            case GPTP_DEF_MSG_TYPE_FOLLOW_UP:
                /* If transport specific value is equal to 1 */
                if ((uint8_t)GPTP_DEF_TRANSPORT_SPEC_1 == u8TransportSpecific)
                {
                    /* Clear default error */
                    eError = GPTP_ERR_OK;

                    /* Get domain number on wire */
                    u8DomainNum = GPTP_MD_ArrayToUint8(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_DOMAIN_NUM]);
                    /* Translate domain number on wire to logical domains of the gPTP stack */
                    bDomainFound = false;
                    /* Search for respective domain */
                    for (u8Seek = 0u; u8Seek < prGptp->rPerDeviceParams.u8NumberOfDomains; u8Seek++)
                    {
                        if (false == bDomainFound)
                        {
                            prDomain = &prGptp->prPerDomainParams[u8Seek];
                            if (u8DomainNum == prDomain->u8DomainNumber)
                            {
                                bDomainFound = true;
                                /* Save domain index into the domain number (temporary) */
                                u8DomainNum = u8Seek;
                            }
                        }
                    }

                    if (true == bDomainFound)
                    {
                        /* Find, to which sync machine the message belongs, by searching for the gPTP port */
                        bMachineFound = false;
                        /* Search for respective machine */
                        for (u8Seek = 0u; u8Seek < prDomain->u8NumberOfSyncsPerDom; u8Seek++)
                        {
                            if (false == bMachineFound)
                            {
                                prSyncMachine = &prDomain->prSyncMachines[u8Seek];
                                if (rRxData.u8PtpPort == prSyncMachine->u8GptpPort)
                                {
                                    bMachineFound = true;
                                    u8SyncMachineId = u8Seek;
                                }
                            }
                        }

                        if (true == bMachineFound)
                        {
                            /* Parse the data from the follow up message and store into the RxData structure */
                            prSyncMachine->rFupMsgRx.rHeader.eMsgId = (gptp_def_msg_type_t)(GPTP_MD_ArrayToUint8(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_MSG_ID_OFFSET]) & 0x0Fu);
                            prSyncMachine->rFupMsgRx.rHeader.u64CorrectionSubNs = GPTP_MD_ArrayToUint64(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_CORR_OFFSET]);
                            prSyncMachine->rFupMsgRx.rHeader.u16SequenceId = GPTP_MD_ArrayToUint16(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SEQ_ID_OFFSET]);
                            prSyncMachine->rFupMsgRx.rHeader.u16SourcePortId = GPTP_MD_ArrayToUint16(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SC_PORT_ID_OFFSET]) - 1u;
                            prSyncMachine->rFupMsgRx.rHeader.u64SourceClockId = GPTP_MD_ArrayToUint64(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SC_CLK_ID_OFFSET]);
                            prSyncMachine->rFupMsgRx.rHeader.s8MessagePeriodLog = GPTP_MD_ArrayToSint8(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_MSG_PER_LOG_OFFSET]);
                            prSyncMachine->rFupMsgRx.rSyncTxTs.u64TimeStampS = GPTP_MD_ArrayToUint48(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_TS_S_OFFSET]);
                            prSyncMachine->rFupMsgRx.rSyncTxTs.u32TimeStampNs = GPTP_MD_ArrayToUint32(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_TS_NS_OFFSET]);
                            prSyncMachine->rFupMsgRx.f64RateRatio = (float64_t)((int32_t)GPTP_MD_ArrayToUint32(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_CS_RATE_RATIO_OFFSET]));

                            /* calculate the rate ratio from cumulative scaled rate */
                            prSyncMachine->rFupMsgRx.f64RateRatio = (prSyncMachine->rFupMsgRx.f64RateRatio * GPTP_DEF_POW_2_M41) + 1.0 ;

                            /* Update GM Time Base Indicator */
                            prDomain->u16GmTimeBaseIndicator = GPTP_MD_ArrayToUint16(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_GM_TIME_BASE_INDIC]);

                            /* Update Last GM Phase Change */
                            prDomain->u32LastGmPhaseChangeH = GPTP_MD_ArrayToUint32(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_LAST_GM_PHASE_CHANGE_H]);
                            prDomain->u64LastGmPhaseChangeL = GPTP_MD_ArrayToUint64(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_LAST_GM_PHASE_CHANGE_L]);

                            /* Update Scaled Last GM Frequency Change */
                            prDomain->u32ScaledLastGmFreqChange = GPTP_MD_ArrayToUint32(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SCALED_LAST_GM_F_CHANGE]);

                            *pu8DomainRcvd = u8DomainNum;
                            *pu8MachineRcvd = u8SyncMachineId;
                            *prMessageTypeRcvd = GPTP_DEF_MSG_TYPE_FOLLOW_UP;

                            /* Set flag - message has been received */
                            prSyncMachine->bFupMsgReceived = true;
#ifdef GPTP_COUNTERS
                            GPTP_INTERNAL_IncrementPortStats(prGptp, rRxData.u8PtpPort, ieee8021AsPortStatRxFollowUpCount);
                            GPTP_INTERNAL_IncrementDomainStats(prGptp, u8DomainNum, u8SyncMachineId, ieee8021AsPortStatRxFollowUpCount);
#endif /* GPTP_COUNTERS */
                        }
                        else
                        {
                            GPTP_ERR_Register(u8SyncMachineId, u8DomainNum, GPTP_ERR_M_SYNC_MACHINE_UNKNOWN, \
                                              GPTP_MD_ArrayToUint16(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SEQ_ID_OFFSET]));
                            eError = GPTP_ERR_M_SYNC_MACHINE_UNKNOWN;
#ifdef GPTP_COUNTERS
                            GPTP_INTERNAL_IncrementPortStats(prGptp, rRxData.u8PtpPort, ieee8021AsPortStatRxPTPPacketDiscard);
#endif /* GPTP_COUNTERS */
                        }
                    }

                    else
                    {
                        GPTP_ERR_Register(u8SyncMachineId, u8DomainNum, GPTP_ERR_M_SYNC_DOMAIN_UNKNOWN, \
                                          GPTP_MD_ArrayToUint16(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SEQ_ID_OFFSET]));
                        eError = GPTP_ERR_M_SYNC_DOMAIN_UNKNOWN;
#ifdef GPTP_COUNTERS
                        GPTP_INTERNAL_IncrementPortStats(prGptp, rRxData.u8PtpPort, ieee8021AsPortStatRxPTPPacketDiscard);
#endif /* GPTP_COUNTERS */
                    }
                }

                else /* Transport specific other than 1 */
                {
                    /* Discard the message */
                    /* Log the error into the error log */
                    GPTP_ERR_Register(rRxData.u8PtpPort, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_M_TRANSPORT_SPECIFIC, \
                                      GPTP_MD_ArrayToUint16(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SEQ_ID_OFFSET]));
                    eError = GPTP_ERR_M_TRANSPORT_SPECIFIC;
#ifdef GPTP_COUNTERS
                    GPTP_INTERNAL_IncrementPortStats(prGptp, rRxData.u8PtpPort, ieee8021AsPortStatRxPTPPacketDiscard);
#endif /* GPTP_COUNTERS */
                }
            break;

            /* Propagation delay follow up message */
            case GPTP_DEF_MSG_TYPE_PD_RESP_FUP:

                /* Clear default error */
                eError = GPTP_ERR_OK;

                /* If transport specific value is equal to 1 */
                if (((uint8_t)GPTP_DEF_TRANSPORT_SPEC_1 == u8TransportSpecific) || (true == prGptp->rPerDeviceParams.bSdoIdCompatibilityMode))
                {
                    if (rRxData.u8PtpPort < prGptp->rPerDeviceParams.u8NumberOfPdelayMachines)
                    {
                        prPdelayMachine = &prGptp->prPdelayMachines[rRxData.u8PtpPort];

                        /* Parse the data from the propagation delay follow up message and store into the RxData structure */
                        prPdelayMachine->rPdRespFupMsgRx.rHeader.eMsgId = (gptp_def_msg_type_t)(GPTP_MD_ArrayToUint8(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_MSG_ID_OFFSET]) & (uint8_t)0x0Fu);
                        prPdelayMachine->rPdRespFupMsgRx.rHeader.u16SequenceId = GPTP_MD_ArrayToUint16(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SEQ_ID_OFFSET]);
                        prPdelayMachine->rPdRespFupMsgRx.rHeader.u16SourcePortId = GPTP_MD_ArrayToUint16(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SC_PORT_ID_OFFSET]) - 1u;
                        prPdelayMachine->rPdRespFupMsgRx.rHeader.u64SourceClockId = GPTP_MD_ArrayToUint64(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SC_CLK_ID_OFFSET]);
                        prPdelayMachine->rPdRespFupMsgRx.rHeader.s8MessagePeriodLog = GPTP_MD_ArrayToSint8(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_MSG_PER_LOG_OFFSET]);
                        prPdelayMachine->rPdRespFupMsgRx.rRequestingId.u64ClockId = GPTP_MD_ArrayToUint64(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_RQ_CLK_ID_OFFSET]);
                        prPdelayMachine->rPdRespFupMsgRx.rRequestingId.u16PortId = GPTP_MD_ArrayToUint16(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_RQ_PORT_ID_OFFSET]) - 1u;
                        prPdelayMachine->rPdRespFupMsgRx.rT3Ts.u64TimeStampS = GPTP_MD_ArrayToUint48(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_TS_S_OFFSET]);
                        prPdelayMachine->rPdRespFupMsgRx.rT3Ts.u32TimeStampNs = GPTP_MD_ArrayToUint32(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_TS_NS_OFFSET]);

                        *pu8MachineRcvd = rRxData.u8PtpPort;
                        *prMessageTypeRcvd = GPTP_DEF_MSG_TYPE_PD_RESP_FUP;

                        /* Set flag - message has been received */
                        prPdelayMachine->bPdelayRespFupReceived = true;
#ifdef GPTP_COUNTERS
                        GPTP_INTERNAL_IncrementPortStats(prGptp, rRxData.u8PtpPort, ieee8021AsPortStatRxPdelayResponseFollowUp);
#endif /* GPTP_COUNTERS */
                    }
                    else
                    {
                        GPTP_ERR_Register(rRxData.u8PtpPort, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_M_PDEL_MACHINE_UNKNOWN, \
                                          GPTP_MD_ArrayToUint16(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SEQ_ID_OFFSET]));
                        eError = GPTP_ERR_M_PDEL_MACHINE_UNKNOWN;
#ifdef GPTP_COUNTERS
                        GPTP_INTERNAL_IncrementPortStats(prGptp, rRxData.u8PtpPort, ieee8021AsPortStatRxPTPPacketDiscard);
#endif /* GPTP_COUNTERS */
                    }

                }

                else /* Transport specific other than 1 */
                {
                    /* Discard the message */
                    /* Log the error into the error log */
                    GPTP_ERR_Register(rRxData.u8PtpPort, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_M_TRANSPORT_SPECIFIC, \
                                      GPTP_MD_ArrayToUint16(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SEQ_ID_OFFSET]));
                    eError = GPTP_ERR_M_TRANSPORT_SPECIFIC;
#ifdef GPTP_COUNTERS
                    GPTP_INTERNAL_IncrementPortStats(prGptp, rRxData.u8PtpPort, ieee8021AsPortStatRxPTPPacketDiscard);
#endif /* GPTP_COUNTERS */
                }
            break;

            /* Announce message */
            case GPTP_DEF_MSG_TYPE_ANNOUNCE:
                /* not applicable in the Automotive */
#ifdef GPTP_COUNTERS
                GPTP_INTERNAL_IncrementPortStats(prGptp, rRxData.u8PtpPort, ieee8021AsPortStatRxAnnounce);
#endif /* GPTP_COUNTERS */
            break;

            /* Signaling message */
            case GPTP_DEF_MSG_TYPE_SIGNALING:

                /* Clear default error */
                eError = GPTP_ERR_OK;

                /* If transport specific value is equal to 1 */
                if ((uint8_t)GPTP_DEF_TRANSPORT_SPEC_1 == u8TransportSpecific)
                {
                    u32OrganizationSubType = GPTP_MD_ArrayToUint32(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SIG_ORG_SUBTYPE_OFFSET]) & GPTP_DEF_BIT_MASK_8L_24H;

                    /* Get domain number on wire */
                    u8DomainNum = GPTP_MD_ArrayToUint8(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_DOMAIN_NUM]);

                    /* Determine machine id from the Domain number and port */
                    bDomainFound = false;
                    /* Search for respective domain */
                    for (u8Seek = 0u; u8Seek < prGptp->rPerDeviceParams.u8NumberOfDomains; u8Seek++)
                    {
                        if (false == bDomainFound)
                        {
                            prDomain = &prGptp->prPerDomainParams[u8Seek];
                            if (u8DomainNum == prDomain->u8DomainNumber)
                            {
                                bDomainFound = true;
                                /* Save domain index into the domain number (temporary) */
                                u8DomainNum = u8Seek;
                            }
                        }
                    }

                    if (true == bDomainFound)
                    {
                        /* Find, to which signaling machine the message belongs, by searching for the gPTP port */
                        bMachineFound = false;
                        /* Search for respective machine */
                        for (u8Seek = 0u; u8Seek < prDomain->u8NumberOfSyncsPerDom; u8Seek++)
                        {
                            if (false == bMachineFound)
                            {
                                prSyncMachine = &prDomain->prSyncMachines[u8Seek];
                                if (rRxData.u8PtpPort == prSyncMachine->u8GptpPort)
                                {
                                    bMachineFound = true;
                                    u8SyncMachineId = u8Seek;
                                }
                            }
                        }

                        if (true == bMachineFound)
                        {
                            switch (u32OrganizationSubType)
                            {
                                /* Mesage interval request TLV */
                                case GPTP_FR_SIG_TLV_OSUB_T_MSG_INT:
                                    /* Parse the data from the signaling message and store into the structure */
                                    prDomain->s8SyncIntervalLogRcvd = (int8_t)GPTP_MD_ArrayToUint8(&(rRxData.cpu8RxData)[u8HdrOffset + \
                                                                      (uint8_t)GPTP_FR_SIG_TIME_SYNC_OFFSET]);

                                    *pu8DomainRcvd = u8DomainNum;
                                    *pu8MachineRcvd = u8SyncMachineId;
                                    *prMessageTypeRcvd = GPTP_DEF_MSG_TYPE_SIGNALING;

                                    /* Set flag - message has been received */
                                    prDomain->bSignalingIntervalRcvd = true;
                                break;

                                /* gPTP capable TLV */
                                case GPTP_FR_SIG_TLV_OSUB_T_CPBL:

                                break;

                                /* gPTP capable message interval request TLV definition */
                                case GPTP_FR_SIG_TLV_OSUB_T_CPBL_INT:

                                break;

                                default:
                                break;
                            }
                        }

                        else
                        {
                            GPTP_ERR_Register(u8SyncMachineId, u8DomainNum, GPTP_ERR_M_SYNC_MACHINE_UNKNOWN, \
                                              GPTP_MD_ArrayToUint16(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SEQ_ID_OFFSET]));
                            eError = GPTP_ERR_M_SYNC_MACHINE_UNKNOWN;
#ifdef GPTP_COUNTERS
                            GPTP_INTERNAL_IncrementPortStats(prGptp, rRxData.u8PtpPort, ieee8021AsPortStatRxPTPPacketDiscard);
#endif /* GPTP_COUNTERS */
                        }
                    }

                    else
                    {
                        GPTP_ERR_Register(rRxData.u8PtpPort, u8DomainNum, GPTP_ERR_M_SYNC_DOMAIN_UNKNOWN, \
                                          GPTP_MD_ArrayToUint16(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SEQ_ID_OFFSET]));
                        eError = GPTP_ERR_M_SYNC_DOMAIN_UNKNOWN;
#ifdef GPTP_COUNTERS
                        GPTP_INTERNAL_IncrementPortStats(prGptp, rRxData.u8PtpPort, ieee8021AsPortStatRxPTPPacketDiscard);
#endif /* GPTP_COUNTERS */
                    }
                }

                else /* Transport specific other than 1 */
                {
                    /* Discard the message */
                    /* Log the error into the error log */
                    GPTP_ERR_Register(rRxData.u8PtpPort, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_M_TRANSPORT_SPECIFIC, \
                                      GPTP_MD_ArrayToUint16(&(rRxData.cpu8RxData)[u8HdrOffset + (uint8_t)GPTP_FR_SEQ_ID_OFFSET]));
                    eError = GPTP_ERR_M_TRANSPORT_SPECIFIC;
#ifdef GPTP_COUNTERS
                GPTP_INTERNAL_IncrementPortStats(prGptp, rRxData.u8PtpPort, ieee8021AsPortStatRxPTPPacketDiscard);
#endif /* GPTP_COUNTERS */
                }
            break;

            /* Unknown message ID */
            default:
                /* Log the error into the error log */
                GPTP_ERR_Register(rRxData.u8PtpPort, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_M_MESSAGE_ID_INVALID, GPTP_ERR_SEQ_ID_NOT_SPECIF);
                eError = GPTP_ERR_M_MESSAGE_ID_INVALID;
#ifdef GPTP_COUNTERS
                GPTP_INTERNAL_IncrementPortStats(prGptp, rRxData.u8PtpPort, ieee8021AsPortStatRxPTPPacketDiscard);
#endif /* GPTP_COUNTERS */
            break;
        }
    }

#ifdef GPTP_COUNTERS
    else
    {
        GPTP_INTERNAL_IncrementPortStats(prGptp, rRxData.u8PtpPort, ieee8021AsPortStatRxPTPPacketDiscard);
    }
#endif /* GPTP_COUNTERS */

    /* Return the error */
    return eError;
}

/*!
 * @brief The function builds PTP ethernet frame to send
 *
 * The function builds PTP ethernet frame to send
 *
 * @param[in] prGptp Pointer to global data structure
 * @param[in] ptxData Pointer to tx data buffer
 * @param[in] u8Domain Domain number
 * @param[in] u8Port gPTP port number
 *
 * @ requirements 529199
 * @ requirements 529211
 * @ requirements 152451
 * @ requirements 152455
 * @ requirements 162145
 * @ requirements 120467
 * @ requirements 120456
 * @ requirements 162147
 */
void GPTP_FRAME_BuildTx(const gptp_def_data_t *prGptp,
                        gptp_def_tx_data_t *ptxData,
                        const uint8_t u8Domain,
                        const uint8_t u8Port)
{
    uint8_t      u8FrameOffset;
    uint8_t      u8TransportSpecOffset;
    uint8_t      u8FlagsOffset;
    uint8_t      u8MsgLengthOffset;
    uint8_t      u8DomainNumOffset;
    uint8_t      u8CorrectionOffset;
    uint8_t      u8VlanMsgLenAdd;
    float64_t    f64Temp;

    /* Frame Eth II */
    u8FrameOffset = 0x00u;

    /* uint48_t (6x8b) Destination MAC address */
    u8FrameOffset = GPTP_MD_Uint48ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], ptxData->u64DestMac, u8FrameOffset);

    /* uint48_t (6x8b) Source MAC address */
    u8FrameOffset = GPTP_MD_Uint48ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], prGptp->prPerPortParams[u8Port].rSourceMac.u48Mac, u8FrameOffset);

    /* If VLAN enabled */
    if (true == prGptp->rPerDeviceParams.bVlanEnabled)
    {
        /* uint16_t Eth type VLAN */
        u8FrameOffset = GPTP_MD_Uint16ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], ((uint16_t)GPTP_FR_ETH_TYPE_VLAN & (uint16_t)GPTP_DEF_BIT_MASK_48L_16H), u8FrameOffset);

        /* uint16_t VLAN ID */
        u8FrameOffset = GPTP_MD_Uint16ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (ptxData->u16VlanTci  & (uint16_t)GPTP_DEF_BIT_MASK_48L_16H), u8FrameOffset);

        /* Extend frame length if VLAN enabled */
        u8VlanMsgLenAdd = GPTP_DEF_ETH_VLAN_LEN;
    }

    else
    {
        /* Frame length not extended of VLAN length */
        u8VlanMsgLenAdd = 0u;
    }

    /* uint16_t Eth type PTP*/
    u8FrameOffset = GPTP_MD_Uint16ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], ((uint16_t)GPTP_FR_ETH_TYPE_PTP & (uint16_t)GPTP_DEF_BIT_MASK_48L_16H), u8FrameOffset);

    /* Frame PTP header*/
    /* Mix two 4bit information into one 8bit. Upper nibble - Transport Specific, Lower nibble - Msg Id */
    u8TransportSpecOffset = u8FrameOffset;
    u8FrameOffset = GPTP_MD_Uint8ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], ((uint8_t)(0x1u << 4u) | (uint8_t)ptxData->eMsgId), u8FrameOffset);

    /* Mix two 4bit information into one 8bit. Upper nibble - Reserved, Lower nibble - Version PTP */
    u8FrameOffset = GPTP_MD_Uint8ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (uint8_t)(0u | 0x2u), u8FrameOffset);

    /* Message Length - message specific, offset saved and used later. */
    u8MsgLengthOffset = u8FrameOffset;
    u8FrameOffset = GPTP_MD_Uint16ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], 0u, u8FrameOffset);

    /* Domain number - 0  for Pdelay (as initiator), variable for Sync/Fup and Pdelay responses */
    u8DomainNumOffset = u8FrameOffset;
    u8FrameOffset = GPTP_MD_Uint8ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], 0u, u8FrameOffset);

    /* Reserved */
    u8FrameOffset = GPTP_MD_Uint8ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], 0u, u8FrameOffset);

    /* Flags - message specific. The value is updated for each message */
    u8FlagsOffset = u8FrameOffset;
    u8FrameOffset = GPTP_MD_Uint16ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], 0u, u8FrameOffset);

    /* Correction field */
    u8CorrectionOffset = u8FrameOffset;
    u8FrameOffset = GPTP_MD_Uint64ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (uint64_t)0u, u8FrameOffset);

    /* Reserved */
    u8FrameOffset = GPTP_MD_Uint32ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], 0u, u8FrameOffset);

    /* Source clock identity */
    u8FrameOffset = GPTP_MD_Uint64ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], ptxData->rSrcPortID.u64ClockId, u8FrameOffset);

    /* Source port identity */
    u8FrameOffset = GPTP_MD_Uint16ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], ptxData->rSrcPortID.u16PortId, u8FrameOffset);

    /* Sequence id */
    u8FrameOffset = GPTP_MD_Uint16ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], ptxData->u16SequenceId, u8FrameOffset);

    /* Mesage type specific data */
    switch (ptxData->eMsgId)
    {
        case GPTP_DEF_MSG_TYPE_SYNC:
            /* Msg header continue - Control */
            u8FrameOffset = GPTP_MD_Uint8ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (uint8_t) GPTP_FR_CONTROL_SYNC, u8FrameOffset);

            /* Log message period */
            u8FrameOffset = GPTP_MD_Uint8ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (uint8_t) ptxData->s8LogMessagePeriod, u8FrameOffset);

            /* Message type specific content outside of the PTP header */
            /* Timestamp Seconds - In the Sync message always 0 (defined as reserved) */
            u8FrameOffset = GPTP_MD_Uint48ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (uint64_t)0u, u8FrameOffset);

            /* Timestamp Nanoseconds - In the Sync message always 0 (defined as reserved */
            u8FrameOffset = GPTP_MD_Uint32ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], 0u, u8FrameOffset);

            /* Update Flags value */
            u8FrameOffset = GPTP_MD_Uint16ToArray(&ptxData->pau8TxBuffPtr[u8FlagsOffset], (uint16_t) GPTP_DEF_SYNC_FLAGS, u8FrameOffset);

            /* Save the message length to the gPTP frame header */
            u8FrameOffset = GPTP_MD_Uint16ToArray(&ptxData->pau8TxBuffPtr[u8MsgLengthOffset], (uint16_t) GPTP_DEF_SYNC_MSG_LEN, u8FrameOffset);

            /* Save the domain number to the gPTP frame header */
            u8FrameOffset = GPTP_MD_Uint8ToArray(&ptxData->pau8TxBuffPtr[u8DomainNumOffset], prGptp->prPerDomainParams[u8Domain].u8DomainNumber, u8FrameOffset);

            /* Save message length for port interface */
            ptxData->u8FrameLength = GPTP_DEF_SYNC_MSG_LEN + GPTP_DEF_ETH_II_LEN + u8VlanMsgLenAdd;
        break;

        case GPTP_DEF_MSG_TYPE_PD_REQ:
            /* Msg header continue - Control */
            u8FrameOffset = GPTP_MD_Uint8ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (uint8_t)GPTP_FR_CONTROL_OTHER, u8FrameOffset);

            /* Log message period */
            u8FrameOffset = GPTP_MD_Uint8ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (uint8_t) ptxData->s8LogMessagePeriod, u8FrameOffset);

            /* Clear Reserved fields in the Pdelay request body */
            u8FrameOffset = GPTP_MD_Uint64ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], 0u, u8FrameOffset);
            u8FrameOffset = GPTP_MD_Uint64ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], 0u, u8FrameOffset);
            u8FrameOffset = GPTP_MD_Uint32ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], 0u, u8FrameOffset);

            /* Update Flags value */
            u8FrameOffset = GPTP_MD_Uint16ToArray(&ptxData->pau8TxBuffPtr[u8FlagsOffset], (uint16_t) GPTP_DEF_PDELAY_REQ_FLAGS, u8FrameOffset);

            /* Save the message length to the gPTP frame header */
            u8FrameOffset = GPTP_MD_Uint16ToArray(&ptxData->pau8TxBuffPtr[u8MsgLengthOffset], (uint16_t) GPTP_DEF_PDELAY_REQ_MSG_LEN, u8FrameOffset);

            /* Save message length for port interface */
            ptxData->u8FrameLength = GPTP_DEF_PDELAY_REQ_MSG_LEN + GPTP_DEF_ETH_II_LEN + u8VlanMsgLenAdd;

        break;

        case GPTP_DEF_MSG_TYPE_PD_RESP:
            /* Msg header continue - Control */
            u8FrameOffset = GPTP_MD_Uint8ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (uint8_t)GPTP_FR_CONTROL_OTHER, u8FrameOffset);

            /* Log message period */
            u8FrameOffset = GPTP_MD_Uint8ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (uint8_t) GPTP_FR_LOG_MSG_PERIOD_STOP, u8FrameOffset);

            /* Timestamp Seconds */
            u8FrameOffset = GPTP_MD_Uint48ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (ptxData->rTs.u64TimeStampS & (uint64_t)GPTP_DEF_BIT_MASK_16L_48H), u8FrameOffset);

            /* Timestamp Nanoseconds */
            u8FrameOffset = GPTP_MD_Uint32ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], ptxData->rTs.u32TimeStampNs, u8FrameOffset);

            /* Requesting clock Id */
            u8FrameOffset = GPTP_MD_Uint64ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], ptxData->rRequesting.u64ClockId, u8FrameOffset);

            /* Requestin port Id */
            u8FrameOffset = GPTP_MD_Uint16ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (ptxData->rRequesting.u16PortId + 1u), u8FrameOffset);

            /* Update transport specific (MajorSdoID) to the value, as received in the pdelay request */
            if (true == prGptp->rPerDeviceParams.bSdoIdCompatibilityMode)
            {
                u8FrameOffset = GPTP_MD_Uint8ToArray(&ptxData->pau8TxBuffPtr[u8TransportSpecOffset], ((uint8_t)(prGptp->prPdelayMachines[u8Port].rPdReqMsgRx.u8MajorSdoId << 4u) | \
                                    (uint8_t)ptxData->eMsgId), u8FrameOffset);
            }

            /* Update Flags value */
            u8FrameOffset = GPTP_MD_Uint16ToArray(&ptxData->pau8TxBuffPtr[u8FlagsOffset], (uint16_t) GPTP_DEF_PDELAY_RESP_FLAGS, u8FrameOffset);

            /* Save the message length to the gPTP frame header */
            u8FrameOffset = GPTP_MD_Uint16ToArray(&ptxData->pau8TxBuffPtr[u8MsgLengthOffset], (uint16_t) GPTP_DEF_PDELAY_RESP_MSG_LEN, u8FrameOffset);

            /* Save the domain number to the gPTP frame header */
            u8FrameOffset = GPTP_MD_Uint8ToArray(&ptxData->pau8TxBuffPtr[u8DomainNumOffset], ptxData->u8SubdomainNumber, u8FrameOffset);

            /* Save message length for port interface */
            ptxData->u8FrameLength = GPTP_DEF_PDELAY_RESP_MSG_LEN + GPTP_DEF_ETH_II_LEN + u8VlanMsgLenAdd;
        break;

        case GPTP_DEF_MSG_TYPE_FOLLOW_UP:
            /* Msg header continue - Control */
            u8FrameOffset = GPTP_MD_Uint8ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (uint8_t)GPTP_FR_CONTROL_FOLLOW_UP, u8FrameOffset);

            /* Log message period */
            u8FrameOffset = GPTP_MD_Uint8ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (uint8_t) ptxData->s8LogMessagePeriod, u8FrameOffset);

            /* Message type specific content outside of the PTP header */
            /* Timestamp Seconds - In the Sync message always 0 (defined as reserved) */
            u8FrameOffset = GPTP_MD_Uint48ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (ptxData->rTs.u64TimeStampS & (uint64_t)GPTP_DEF_BIT_MASK_16L_48H), u8FrameOffset);

            /* Timestamp Nanoseconds - In the Sync message always 0 (defined as reserved */
            u8FrameOffset = GPTP_MD_Uint32ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], ptxData->rTs.u32TimeStampNs, u8FrameOffset);

            /* TLV Information */
            /* Tlv type */
            u8FrameOffset = GPTP_MD_Uint16ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (uint16_t) GPTP_FR_FUP_TLV_TYPE, u8FrameOffset);

            /* Length field */
            u8FrameOffset = GPTP_MD_Uint16ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (uint16_t) GPTP_FR_FUP_TLV_FIELD_LEN, u8FrameOffset);

            /* Organization Id */
            u8FrameOffset = GPTP_MD_Uint24ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (uint32_t)(GPTP_FR_FUP_TLV_ORGANIZATION_ID), u8FrameOffset);

            /* Organization sub type */
            u8FrameOffset = GPTP_MD_Uint24ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (uint32_t)(GPTP_FR_FUP_TLV_ORG_SUB_TYPE), u8FrameOffset);

            /* Cumulative scale rate ratio */
            f64Temp = (ptxData->f64RateRatio - 1.0) * (float64_t)GPTP_DEF_POW_2_41;
            u8FrameOffset = GPTP_MD_Uint32ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (uint32_t)((int32_t)f64Temp), u8FrameOffset);

            /* GM Time Base Indicator */
            u8FrameOffset = GPTP_MD_Uint16ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], prGptp->prPerDomainParams[u8Domain].u16GmTimeBaseIndicator, u8FrameOffset);

            /* Last GP phase change */
            u8FrameOffset = GPTP_MD_Uint32ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (uint32_t)prGptp->prPerDomainParams[u8Domain].u32LastGmPhaseChangeH, u8FrameOffset);
            u8FrameOffset = GPTP_MD_Uint64ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (uint64_t)prGptp->prPerDomainParams[u8Domain].u64LastGmPhaseChangeL, u8FrameOffset);

            /* Scaled last GM frequency change */
            u8FrameOffset = GPTP_MD_Uint32ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], prGptp->prPerDomainParams[u8Domain].u32ScaledLastGmFreqChange, u8FrameOffset);

            /* Update Flags value */
            u8FrameOffset = GPTP_MD_Uint16ToArray(&ptxData->pau8TxBuffPtr[u8FlagsOffset], (uint16_t) GPTP_DEF_FUP_FLAGS, u8FrameOffset);

            /* Correction field */
            u8FrameOffset = GPTP_MD_Uint64ToArray(&ptxData->pau8TxBuffPtr[u8CorrectionOffset], ptxData->u64CorrectionSubNs, u8FrameOffset);

            /* Save the message length to the gPTP frame header */
            u8FrameOffset = GPTP_MD_Uint16ToArray(&ptxData->pau8TxBuffPtr[u8MsgLengthOffset], (uint16_t) GPTP_DEF_FOLLOW_UP_MSG_LEN, u8FrameOffset);

            /* Save the domain number to the gPTP frame header */
            u8FrameOffset = GPTP_MD_Uint8ToArray(&ptxData->pau8TxBuffPtr[u8DomainNumOffset], prGptp->prPerDomainParams[u8Domain].u8DomainNumber, u8FrameOffset);

            /* Save message length for port interface */
            ptxData->u8FrameLength = GPTP_DEF_FOLLOW_UP_MSG_LEN + GPTP_DEF_ETH_II_LEN + u8VlanMsgLenAdd;
        break;

        case GPTP_DEF_MSG_TYPE_PD_RESP_FUP:
            /* Msg header continue - Control */
            u8FrameOffset = GPTP_MD_Uint8ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (uint8_t)GPTP_FR_CONTROL_OTHER, u8FrameOffset);

            /* Log message period */
            u8FrameOffset = GPTP_MD_Uint8ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (uint8_t) GPTP_FR_LOG_MSG_PERIOD_STOP, u8FrameOffset);

            /* Message type specific content outside of the PTP header */
            /* Timestamp Seconds - In the Sync message always 0 (defined as reserved) */
            u8FrameOffset = GPTP_MD_Uint48ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (ptxData->rTs.u64TimeStampS & (uint64_t)
            GPTP_DEF_BIT_MASK_16L_48H), u8FrameOffset);

            /* Timestamp Nanoseconds - In the Sync message always 0 (defined as reserved */
            u8FrameOffset = GPTP_MD_Uint32ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], ptxData->rTs.u32TimeStampNs, u8FrameOffset);

            /* Requesting clock Id */
            u8FrameOffset = GPTP_MD_Uint64ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], ptxData->rRequesting.u64ClockId, u8FrameOffset);

            /* Requestin port Id */
            u8FrameOffset = GPTP_MD_Uint16ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (ptxData->rRequesting.u16PortId + 1u), u8FrameOffset);

            /* Update transport specific (MajorSdoID) to the value, as received in the pdelay request */
            if (true == prGptp->rPerDeviceParams.bSdoIdCompatibilityMode)
            {
                u8FrameOffset = GPTP_MD_Uint8ToArray(&ptxData->pau8TxBuffPtr[u8TransportSpecOffset], ((uint8_t)(prGptp->prPdelayMachines[u8Port].rPdReqMsgRx.u8MajorSdoId << 4u) | \
                                    (uint8_t)ptxData->eMsgId), u8FrameOffset);
            }

            /* Update Flags value */
            u8FrameOffset = GPTP_MD_Uint16ToArray(&ptxData->pau8TxBuffPtr[u8FlagsOffset], (uint16_t) GPTP_DEF_PDELAY_RESP_FUP_FLAGS, u8FrameOffset);

            /* Save the message length to the gPTP frame header */
            u8FrameOffset = GPTP_MD_Uint16ToArray(&ptxData->pau8TxBuffPtr[u8MsgLengthOffset], (uint16_t) GPTP_DEF_PDELAY_FUP_MSG_LEN, u8FrameOffset);

            /* Save the domain number to the gPTP frame header */
            u8FrameOffset = GPTP_MD_Uint8ToArray(&ptxData->pau8TxBuffPtr[u8DomainNumOffset], ptxData->u8SubdomainNumber, u8FrameOffset);

            /* Save message length for port interface */
            ptxData->u8FrameLength = GPTP_DEF_PDELAY_FUP_MSG_LEN + GPTP_DEF_ETH_II_LEN + u8VlanMsgLenAdd;
        break;

        /* Signaling message */
        case GPTP_DEF_MSG_TYPE_SIGNALING:
            /* Msg header continue - Control */
            u8FrameOffset = GPTP_MD_Uint8ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (uint8_t)GPTP_FR_CONTROL_OTHER, u8FrameOffset);

            /* Log message period */
            u8FrameOffset = GPTP_MD_Uint8ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (uint8_t) GPTP_FR_LOG_MSG_PERIOD_STOP, u8FrameOffset);

            /* Message type specific content outside of the PTP header */
            /* Target Port Identity - 10 Bytes */
            u8FrameOffset = GPTP_MD_Uint64ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], 0xFFFFFFFFFFFFFFFFu, u8FrameOffset);
            u8FrameOffset = GPTP_MD_Uint16ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], 0xFFFFu, u8FrameOffset);

            /* Message interval request TLV */
            /* Tlv type */
            u8FrameOffset = GPTP_MD_Uint16ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (uint16_t) GPTP_FR_SIG_TLV_TYPE, u8FrameOffset);

            /* Length field */
            u8FrameOffset = GPTP_MD_Uint16ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (uint16_t) GPTP_FR_SIG_TLV_FIELD_LEN, u8FrameOffset);

            /* Organization Id */
            u8FrameOffset = GPTP_MD_Uint24ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (uint32_t)(GPTP_FR_SIG_TLV_ORGANIZATION_ID), u8FrameOffset);

            /* Organization sub type */
            u8FrameOffset = GPTP_MD_Uint24ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (uint32_t)(GPTP_FR_SIG_TLV_OSUB_T_MSG_INT), u8FrameOffset);

            /* Link delay Interval */
            u8FrameOffset = GPTP_MD_Uint8ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (uint8_t) GPTP_FR_LOG_MSG_PERIOD_STOP, u8FrameOffset);

            /* Time synchronization interval */
            u8FrameOffset = GPTP_MD_Uint8ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (uint8_t) ptxData->s8LogMessagePeriod, u8FrameOffset);

            /* Announce interval - always disabled */
            u8FrameOffset = GPTP_MD_Uint8ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (uint8_t) GPTP_FR_LOG_MSG_PERIOD_STOP, u8FrameOffset);

            /* Signaling TLV Flags */
            u8FrameOffset = GPTP_MD_Uint8ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset],(uint8_t) GPTP_DEF_SIG_TLV_FLAGS, u8FrameOffset);

            /* Reserved */
            u8FrameOffset = GPTP_MD_Uint16ToArray(&ptxData->pau8TxBuffPtr[u8FrameOffset], (uint16_t) 0u, u8FrameOffset);

            /* Update Flags value */
            u8FrameOffset = GPTP_MD_Uint16ToArray(&ptxData->pau8TxBuffPtr[u8FlagsOffset], (uint16_t) GPTP_DEF_SIGNALING_FLAGS, u8FrameOffset);

            /* Save the message length to the gPTP frame header */
            u8FrameOffset = GPTP_MD_Uint16ToArray(&ptxData->pau8TxBuffPtr[u8MsgLengthOffset], (uint16_t) GPTP_DEF_SIGNALING_MSG_LEN, u8FrameOffset);

            /* Save message length for port interface */
            ptxData->u8FrameLength = GPTP_DEF_SIGNALING_MSG_LEN + GPTP_DEF_ETH_II_LEN + u8VlanMsgLenAdd;
        break;

        default:
        break;
    }
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
