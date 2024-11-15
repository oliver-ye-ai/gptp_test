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
 * @file gptp.c
 *
 */

#include "gptp_def.h"
#include "gptp_err.h"
#include "gptp_port.h"
#include "gptp.h"
#include "gptp_timer.h"
#include "gptp_frame.h"
#include "gptp_sync.h"
#include "gptp_pdelay.h"
#include "gptp_signaling.h"
#include "gptp_internal.h"
#include "gptp_pi.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/
static const uint8_t cau8ManId[GPTP_DEF_MAN_IDENTITY_LENGTH] = GPTP_DEF_MANUFACTURER_IDENTITY;
static const uint8_t cau8ProductRev[GPTP_DEF_PROD_REV_LENGTH] = GPTP_PORT_PRODUCT_REVISION;

static gptp_def_data_t          rGptpDataStruct =
{
    /* Initialize Product details */
    .rProductDetails.cpu8ManId = cau8ManId,
    .rProductDetails.cpu8ProdRev = cau8ProductRev,
    .rProductDetails.au8ProdDesc = GPTP_PORT_PRODUCT_DESCRIPTION
};

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static uint8_t GPTP_FrameIdReset(uint8_t *FrameId,
                                 uint8_t u8Value);

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 *
 * @brief   This function restarts frame ID counter.
 *
 * @details The function restarts frame ID counter to the default value.
 *
 * @param[in] *FrameId Pointer to the frame ID counter.
 * @param[in] u8Value  Reset value for the counter.
 * @returns   Temporary frame ID for the current transmission.
 *
 */
static uint8_t GPTP_FrameIdReset(uint8_t *FrameId,
                                 uint8_t u8Value)
{
    uint8_t u8TempId;

    *FrameId = u8Value;
    u8TempId = *FrameId;
    (*FrameId)++;

    return u8TempId;
}

/*!
 *
 * @brief      This function sends the gPTP message
 *
 * @details    This function increments frame ID, and stores details about the transmitted frame for the
 *             future check. Then, the complete data frame is prepared and sent. This function is called
 *             from the inside of the stack only and should not be used by the application.
 *
 * @param[in]  prTxData Pointer to structure containing data for transmission
 * @param[in]  u8Port gPTP port number from which the message is to be sent
 * @param[in]  u8MachineNum Index of machine, from which the message is to be sent. For Pdelay messages
 *                          the machine index is the same as gPTP port number.
 * @param[in]  u8DomainNum Index of domain, from which the message is to be sent. For Pdelay messages
 *                         the domain index is 0.
 *
 * @ requirements 529199
 * @ requirements 162146
 */
void GPTP_MsgTransmit(gptp_def_tx_data_t *prTxData,
                      const uint8_t u8Port,
                      const uint8_t u8MachineNum,
                      const uint8_t u8DomainNum)
{
    uint8_t              u8TempFrameId;
    gptp_err_type_t      eErr;

    switch (prTxData->eMsgId)
    {
        case GPTP_DEF_MSG_TYPE_SYNC:
            if (GPTP_DEF_FID_SYNC_MAX > rGptpDataStruct.rPerDeviceParams.u8NextFrameIdSync)
            {
                u8TempFrameId = rGptpDataStruct.rPerDeviceParams.u8NextFrameIdSync;
                rGptpDataStruct.rPerDeviceParams.u8NextFrameIdSync++;
            }

            else
            {
                u8TempFrameId = GPTP_FrameIdReset(&rGptpDataStruct.rPerDeviceParams.u8NextFrameIdSync, GPTP_DEF_FID_SYNC_START);
            }

            /* Store Frame frame information to the frame ID table, to the NextFrameId index  */
            rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8TempFrameId].u8Port = u8Port;
            rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8TempFrameId].eMsgType = prTxData->eMsgId;
            rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8TempFrameId].u16SequenceId = prTxData->u16SequenceId;
            rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8TempFrameId].u8MachineId = u8MachineNum;
            rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8TempFrameId].u8DomainId = u8DomainNum;
            rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8TempFrameId].bSyncActingGm = prTxData->bSyncActingGm;
        break;

        case GPTP_DEF_MSG_TYPE_PD_REQ:
            if ((GPTP_DEF_FID_PDREQ_START <= rGptpDataStruct.rPerDeviceParams.u8NextFrameIdPdReq) && \
               (GPTP_DEF_FID_PDREQ_MAX > rGptpDataStruct.rPerDeviceParams.u8NextFrameIdPdReq))
            {
                u8TempFrameId = rGptpDataStruct.rPerDeviceParams.u8NextFrameIdPdReq;
                rGptpDataStruct.rPerDeviceParams.u8NextFrameIdPdReq++;
            }

            else
            {
                u8TempFrameId = GPTP_FrameIdReset(&rGptpDataStruct.rPerDeviceParams.u8NextFrameIdPdReq, GPTP_DEF_FID_PDREQ_START);
            }

            /* Store Frame frame information to the frame ID table, to the NextFrameId index  */
            rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8TempFrameId].u8Port = u8Port;
            rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8TempFrameId].eMsgType = prTxData->eMsgId;
            rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8TempFrameId].u16SequenceId = prTxData->u16SequenceId;
            rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8TempFrameId].u8MachineId = u8MachineNum;
            rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8TempFrameId].u8DomainId = u8DomainNum;
            rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8TempFrameId].bSyncActingGm = false;
        break;

        case GPTP_DEF_MSG_TYPE_PD_RESP:
            u8TempFrameId = rGptpDataStruct.rPerDeviceParams.u8NextFrameIdPdResp;
            if ((GPTP_DEF_FID_PDRESP_START <= rGptpDataStruct.rPerDeviceParams.u8NextFrameIdPdResp) && \
               (GPTP_DEF_FID_PDRESP_MAX > rGptpDataStruct.rPerDeviceParams.u8NextFrameIdPdResp))
            {
                u8TempFrameId = rGptpDataStruct.rPerDeviceParams.u8NextFrameIdPdResp;
                rGptpDataStruct.rPerDeviceParams.u8NextFrameIdPdResp++;
            }

            else
            {
                u8TempFrameId = GPTP_FrameIdReset(&rGptpDataStruct.rPerDeviceParams.u8NextFrameIdPdResp, GPTP_DEF_FID_PDRESP_START);
            }

            /* Store Frame frame information to the frame ID table, to the NextFrameId index  */
            rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8TempFrameId].u8Port = u8Port;
            rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8TempFrameId].eMsgType = prTxData->eMsgId;
            rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8TempFrameId].u16SequenceId = prTxData->u16SequenceId;
            rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8TempFrameId].u8MachineId = u8MachineNum;
            rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8TempFrameId].u8DomainId = u8DomainNum;
            rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8TempFrameId].bSyncActingGm = false;
        break;

        default:
            u8TempFrameId = 255u;
            /* Store Frame frame information to the frame ID table, to the NextFrameId index  */
            rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8TempFrameId].u8Port = 0u;
            rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8TempFrameId].eMsgType = GPTP_DEF_MSG_TYPE_UNKNOWN;
            rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8TempFrameId].u16SequenceId = 65535u;
            rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8TempFrameId].u8MachineId = 0u;
            rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8TempFrameId].u8DomainId = 0u;
            rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8TempFrameId].bSyncActingGm = false;
        break;
    }

    if (NULL != prTxData->pau8TxBuffPtr)
    {
        GPTP_FRAME_BuildTx(&rGptpDataStruct, prTxData, u8DomainNum, u8Port);
        eErr = GPTP_PORT_MsgSend(u8Port, rGptpDataStruct.rPerDeviceParams.u8EthFramePrio, prTxData, u8TempFrameId);
        if (GPTP_ERR_OK != eErr)
        {
            GPTP_ERR_Register(u8Port, u8DomainNum, eErr, prTxData->u16SequenceId);
        }

#ifdef GPTP_COUNTERS
        switch (prTxData->eMsgId)
        {
            case GPTP_DEF_MSG_TYPE_SYNC:
                GPTP_INTERNAL_IncrementPortStats(&rGptpDataStruct, u8Port, ieee8021AsPortStatTxSyncCount);
                GPTP_INTERNAL_IncrementDomainStats(&rGptpDataStruct, u8DomainNum, u8MachineNum, ieee8021AsPortStatTxSyncCount);
            break;

            case GPTP_DEF_MSG_TYPE_FOLLOW_UP:
                GPTP_INTERNAL_IncrementPortStats(&rGptpDataStruct, u8Port, ieee8021AsPortStatTxFollowUpCount);
                GPTP_INTERNAL_IncrementDomainStats(&rGptpDataStruct, u8DomainNum, u8MachineNum, ieee8021AsPortStatTxFollowUpCount);
            break;

            case GPTP_DEF_MSG_TYPE_PD_REQ:
                GPTP_INTERNAL_IncrementPortStats(&rGptpDataStruct, u8Port, ieee8021AsPortStatTxPdelayRequest);
            break;

            case GPTP_DEF_MSG_TYPE_PD_RESP:
                GPTP_INTERNAL_IncrementPortStats(&rGptpDataStruct, u8Port, ieee8021AsPortStatTxPdelayResponse);
            break;

            case GPTP_DEF_MSG_TYPE_PD_RESP_FUP:
                GPTP_INTERNAL_IncrementPortStats(&rGptpDataStruct, u8Port, ieee8021AsPortStatTxPdelayResponseFollowUp);
            break;

            case GPTP_DEF_MSG_TYPE_ANNOUNCE:
                GPTP_INTERNAL_IncrementPortStats(&rGptpDataStruct, u8Port, ieee8021AsPortStatTxAnnounce);
            break;

            default:
            break;
        }
#endif /* GPTP_COUNTERS */

    }
    else
    {
        eErr = GPTP_ERR_M_MSG_BUFF_PTR_NULL;
        GPTP_ERR_Register(u8Port, u8DomainNum, eErr, prTxData->u16SequenceId);
    }
}

/*!
 *
 * @brief      This function initializes gPTP stack.
 *
 * @details    This function checks initial parameters for invalid values. Uses initial parameters
 *             to initialize the internal gPTP structures. The function checks the current link status.
 *             For all "up" links which are used for gPTP, it starts the gPTP software timer.
 *
 * @param[in]  prInitParams Pointer to structure containing initial parameters
 *
 * @return     gPTP error code.
 *
 * @ requirements 529199
 * @ requirements 529200
 * @ requirements 120448
 * @ requirements 120449
 * @ requirements 152447
 * @ requirements 120454
 * @ requirements 341436
 */
gptp_err_type_t GPTP_GptpInit(const gptp_def_init_params_t *prInitParams)
{
    gptp_err_type_t                         eError;
    gptp_err_type_t                         eErrorTmp;
    static bool                             bLinkStat;
    static uint64_t                         u64PortMac;
    uint8_t                                 u8Port;
    uint8_t                                 u8NumberOfPorts;
    uint8_t                                 u8Machine;
    uint8_t                                 u8Domain;
    uint8_t                                 u8Seek;
    uint8_t                                 u8DomainChckA;
    uint8_t                                 u8DomainChckB;
    uint16_t                                u16Clean;
    bool                                    bSlaveFound;
    bool                                    bDomNumFound;
    gptp_def_port_t                         *prPort;
    gptp_def_domain_t                       *prDomain;
    gptp_def_sync_t                         *prSync;
    gptp_def_pdelay_t                       *prPdelay;
    const gptp_def_init_params_domain_t     *prDomainInit;
    const gptp_def_init_params_pdel_t       *prPdelayInit;
    const gptp_def_init_params_sync_t       *prSyncInit;
    uint8_t                                 au8MacAddr[6];

    /* Error initial value */
    eError = GPTP_ERR_OK;

    /* Initialize error notification callback function before the Error management is initialized */
    rGptpDataStruct.rPtpStackCallBacks.pfErrNotify = prInitParams->rPtpStackCallBacks.pfErrNotify;

    /* Init Err management and error log clear */
    GPTP_ERR_MgmtInit(&rGptpDataStruct);
    GPTP_ERR_LogClear();

    /* Initialize Integral windup limit, there is no need to check value (0 avoids windup functionality) */
    rGptpDataStruct.rPerDeviceParams.rPIControllerConfig.u32IntegralWindupLimit = prInitParams->rPIControllerConfig.u32IntegralWindupLimit;

    /* Initialize damping ratio for PI Controller, there is no need to check value (0 can ignore P term calculation) */
    rGptpDataStruct.rPerDeviceParams.rPIControllerConfig.f64DampingRatio = prInitParams->rPIControllerConfig.f64DampingRatio;

    /* Check natural frequency ratio, because dividing by 0 occur exception */
    if ((0.0 > prInitParams->rPIControllerConfig.f64NatFreqRatio) || (0.0 < prInitParams->rPIControllerConfig.f64NatFreqRatio))
    {
        /* Initialize natural frequency ratio for PI Controller */
        rGptpDataStruct.rPerDeviceParams.rPIControllerConfig.f64NatFreqRatio = prInitParams->rPIControllerConfig.f64NatFreqRatio;

        /* Initialize PI Controller */
        GPTP_PI_Init(&rGptpDataStruct.rPerDeviceParams.rPIControllerConfig);
    }
    else
    {
        /* Register error -  missing configuration */
        GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_PI_INCORRECT_CFG, GPTP_ERR_SEQ_ID_NOT_SPECIF);
    }

    /* Initialize PI Controler maximum threshold */
    rGptpDataStruct.rPerDeviceParams.u64PIControllerMaxThreshold = prInitParams->u64PIControllerMaxThreshold;
    
    /* Register multicast MAC */
    au8MacAddr[0] = (uint8_t)((uint64_t)GPTP_DEF_ETH_PTP_MAC_MULTICAST >> 40u) & 255u;
    au8MacAddr[1] = (uint8_t)((uint64_t)GPTP_DEF_ETH_PTP_MAC_MULTICAST >> 32u) & 255u;
    au8MacAddr[2] = (uint8_t)((uint64_t)GPTP_DEF_ETH_PTP_MAC_MULTICAST >> 24u) & 255u;
    au8MacAddr[3] = (uint8_t)((uint64_t)GPTP_DEF_ETH_PTP_MAC_MULTICAST >> 16u) & 255u;
    au8MacAddr[4] = (uint8_t)((uint64_t)GPTP_DEF_ETH_PTP_MAC_MULTICAST >> 8u) & 255u;
    au8MacAddr[5] = (uint8_t)((uint64_t)GPTP_DEF_ETH_PTP_MAC_MULTICAST) & 255u;
    GPTP_PORT_MulticastForward(au8MacAddr);

    /* Assign automatic variable before it's used */
    u8NumberOfPorts = 0u;

    if (0u == prInitParams->u8GptpPortsCount)
    {
        eError = GPTP_ERR_I_NO_PORT_INITIALIZED;
        GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_I_NO_PORT_INITIALIZED, GPTP_ERR_SEQ_ID_NOT_SPECIF);
    }

    if (GPTP_ERR_OK == eError)
    {
        rGptpDataStruct.rPerDeviceParams.u8PortsCount = prInitParams->u8GptpPortsCount;
        u8NumberOfPorts = rGptpDataStruct.rPerDeviceParams.u8PortsCount;

        if (NULL != prInitParams->pcrPortMapTable)
        {
            /* Pass pointers to arrays */
             GPTP_PORT_PortMapInit(prInitParams->pcrPortMapTable, u8NumberOfPorts);
        }
        else
        {
            eError = GPTP_ERR_I_PORT_MAP_NULL_PTR;
            GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_I_PORT_MAP_NULL_PTR, GPTP_ERR_SEQ_ID_NOT_SPECIF);
        }
    }

    /* Check pointers if initialized */
#ifdef GPTP_COUNTERS
    if (NULL != prInitParams->prCntrsPort)
    {
        rGptpDataStruct.prCntrsPort = prInitParams->prCntrsPort;
    }
    else
    {
        eError = GPTP_ERR_I_COUNTERS_NULL_PTR;
        GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_I_COUNTERS_NULL_PTR, GPTP_ERR_SEQ_ID_NOT_SPECIF);
    }
#endif /* GPTP_COUNTERS */

#ifdef GPTP_TIME_VALIDATION
    if (NULL != prInitParams->rPtpStackCallBacks.pfTimeBaseProvideNotifSlave)
    {
        rGptpDataStruct.rPtpStackCallBacks.pfTimeBaseProvideNotifSlave = prInitParams->rPtpStackCallBacks.pfTimeBaseProvideNotifSlave;
    }
    else
    {
        eError = GPTP_ERR_F_NULL_PTR;
        GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_F_NULL_PTR, GPTP_ERR_SEQ_ID_NOT_SPECIF);
    }

    if (NULL != prInitParams->rPtpStackCallBacks.pfTimeBaseProvideNotifMaster)
    {
        rGptpDataStruct.rPtpStackCallBacks.pfTimeBaseProvideNotifMaster = prInitParams->rPtpStackCallBacks.pfTimeBaseProvideNotifMaster;
    }
    else
    {
        eError = GPTP_ERR_F_NULL_PTR;
        GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_F_NULL_PTR, GPTP_ERR_SEQ_ID_NOT_SPECIF);
    }

    if (NULL != prInitParams->rPtpStackCallBacks.pfSetPdelayInitiatorData)
    {
        rGptpDataStruct.rPtpStackCallBacks.pfSetPdelayInitiatorData = prInitParams->rPtpStackCallBacks.pfSetPdelayInitiatorData;
    }
    else
    {
        eError = GPTP_ERR_F_NULL_PTR;
        GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_F_NULL_PTR, GPTP_ERR_SEQ_ID_NOT_SPECIF);
    }

    if (NULL != prInitParams->rPtpStackCallBacks.pfSetPdelayResponderData)
    {
        rGptpDataStruct.rPtpStackCallBacks.pfSetPdelayResponderData = prInitParams->rPtpStackCallBacks.pfSetPdelayResponderData;
    }
    else
    {
        eError = GPTP_ERR_F_NULL_PTR;
        GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_F_NULL_PTR, GPTP_ERR_SEQ_ID_NOT_SPECIF);
    }
#endif /* GPTP_TIME_VALIDATION */

    if ((NULL != prInitParams->paPort) && (NULL != prInitParams->paDomain) && (NULL != prInitParams->paPdelay))
    {
        rGptpDataStruct.prPerPortParams = prInitParams->paPort;
        rGptpDataStruct.prPerDomainParams = prInitParams->paDomain;
        rGptpDataStruct.prPdelayMachines = prInitParams->paPdelay;
    }
    else
    {
        if (NULL == prInitParams->paPort)
        {
            eError = GPTP_ERR_I_PORT_STRCT_NULL_PTR;
            GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_I_PORT_STRCT_NULL_PTR, GPTP_ERR_SEQ_ID_NOT_SPECIF);
        }

        if (NULL == prInitParams->paDomain)
        {
            eError = GPTP_ERR_I_DOM_STRCT_NULL_PTR;
            GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_I_DOM_STRCT_NULL_PTR, GPTP_ERR_SEQ_ID_NOT_SPECIF);
        }

        if (NULL == prInitParams->paPdelay)
        {
            eError = GPTP_ERR_I_PD_STRCT_NULL_PTR;
            GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_I_PD_STRCT_NULL_PTR, GPTP_ERR_SEQ_ID_NOT_SPECIF);
        }
    }

    if (0u != prInitParams->u8GptpDomainsCount)
    {
        for (u8Domain = 0u; u8Domain < prInitParams->u8GptpDomainsCount; u8Domain++)
        {
            if (NULL != prInitParams->pcrDomainsSyncsParam[u8Domain].prSync)
            {
                rGptpDataStruct.prPerDomainParams[u8Domain].prSyncMachines = prInitParams->pcrDomainsSyncsParam[u8Domain].prSync;
            }
            else
            {
                eError = GPTP_ERR_I_SYNC_STRCT_NULL_PTR;
                GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, u8Domain, GPTP_ERR_I_SYNC_STRCT_NULL_PTR, GPTP_ERR_SEQ_ID_NOT_SPECIF);
            }
        }
    }
    else
    {
        eError = GPTP_ERR_I_NO_DOM_INITIALIZED;
        GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_I_NO_DOM_INITIALIZED, GPTP_ERR_SEQ_ID_NOT_SPECIF);
    }

    /* Check domains */
    for (u8DomainChckA = 0u; u8DomainChckA < prInitParams->u8GptpDomainsCount; u8DomainChckA++)
    {
        bDomNumFound = false;
        for (u8DomainChckB = 0u; u8DomainChckB < prInitParams->u8GptpDomainsCount; u8DomainChckB++)
        {
            if (u8DomainChckA != u8DomainChckB)
            {
                /* Check if the domain number is used twice */
                if (prInitParams->pcrDomainsSyncsParam[u8DomainChckA].u8DomainNumber == prInitParams->pcrDomainsSyncsParam[u8DomainChckB].u8DomainNumber)
                {
                    eError = GPTP_ERR_I_DOM_NUM_USED_TWICE;
                    GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, u8DomainChckA, GPTP_ERR_I_DOM_NUM_USED_TWICE, GPTP_ERR_SEQ_ID_NOT_SPECIF);
                }
            }

            /* If the domain is synced GM */
            if (true == prInitParams->pcrDomainsSyncsParam[u8DomainChckA].bSyncedGm)
            {
                if (prInitParams->pcrDomainsSyncsParam[u8DomainChckA].u8RefDomForSynced == prInitParams->pcrDomainsSyncsParam[u8DomainChckB].u8DomainNumber)
                {
                    bDomNumFound = true;
                }
            }

            else
            {
                /* fake found, because the domain is not synced GM */
                bDomNumFound = true;
            }
        }

        /* Reference domain for synced GM is not initialized */
        if (false == bDomNumFound)
        {
            eError = GPTP_ERR_I_DOM_TO_REF_NOT_INIT;
            GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, u8DomainChckA, GPTP_ERR_I_DOM_TO_REF_NOT_INIT, GPTP_ERR_SEQ_ID_NOT_SPECIF);
        }
    }

    /* Per devide params initilize */
    rGptpDataStruct.rPerDeviceParams.u8EthFramePrio = prInitParams->u8EthFramePrio;
    rGptpDataStruct.rPerDeviceParams.f64PdelAvgWeight = prInitParams->f64PdelAvgWeight;
    rGptpDataStruct.rPerDeviceParams.f64RratioAvgWeight = prInitParams->f64RratioAvgWeight;
    rGptpDataStruct.rPerDeviceParams.f64RratioMaxDev = prInitParams->f64RratioMaxDev;
    rGptpDataStruct.rPerDeviceParams.f64PdelayNvmWriteThr = prInitParams->f64PdelayNvmWriteThr;
    rGptpDataStruct.rPerDeviceParams.f64RratioNvmWriteThr = prInitParams->f64RratioNvmWriteThr;
    rGptpDataStruct.rPerDeviceParams.u8NextFrameIdSync = 0u;
    rGptpDataStruct.rPerDeviceParams.u8NextFrameIdPdReq = 100u;
    rGptpDataStruct.rPerDeviceParams.u8NextFrameIdPdResp = 175u;
    rGptpDataStruct.rPerDeviceParams.u8NumberOfDomains = prInitParams->u8GptpDomainsCount;
    rGptpDataStruct.rPerDeviceParams.u8NumberOfPdelayMachines = rGptpDataStruct.rPerDeviceParams.u8PortsCount;
    rGptpDataStruct.rPerDeviceParams.bSdoIdCompatibilityMode = prInitParams->bSdoIdCompatibilityMode;
    rGptpDataStruct.rPerDeviceParams.bVlanEnabled = prInitParams->bVlanEnabled;
    rGptpDataStruct.rPerDeviceParams.u16VlanTci = prInitParams->u16VlanTci;
    rGptpDataStruct.rPerDeviceParams.bSignalingEnabled = prInitParams->bSignalingEnabled;
    rGptpDataStruct.rPerDeviceParams.rEstimGmOffset.s64TimeStampS = 0;
    rGptpDataStruct.rPerDeviceParams.rEstimGmOffset.s32TimeStampNs = 0;
    rGptpDataStruct.rPerDeviceParams.f64CorrClockRateRatio = 1.0;
    rGptpDataStruct.rPerDeviceParams.bEverUpdated = false;
    rGptpDataStruct.rPerDeviceParams.bEstimGmOffsetCalculated = false;
    rGptpDataStruct.rPerDeviceParams.rSynLockParam = prInitParams->rSynLockParam;
    rGptpDataStruct.rPtpStackCallBacks.pfDomSelectionCB = prInitParams->rPtpStackCallBacks.pfDomSelectionCB;
    rGptpDataStruct.rPtpStackCallBacks.pfNvmWriteCB = prInitParams->rPtpStackCallBacks.pfNvmWriteCB;
    rGptpDataStruct.rPtpStackCallBacks.pfNvmReadCB = prInitParams->rPtpStackCallBacks.pfNvmReadCB;
    rGptpDataStruct.rPtpStackCallBacks.pfErrNotify = prInitParams->rPtpStackCallBacks.pfErrNotify;
    rGptpDataStruct.rPtpStackCallBacks.pfSynNotify = prInitParams->rPtpStackCallBacks.pfSynNotify;

    /* Clean frame ID table */
    for (u16Clean = 0u; u16Clean < 256u; u16Clean++)
    {
        rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u16Clean].u8Port = (uint8_t)0xFFu;
        rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u16Clean].u8MachineId = 0u;
        rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u16Clean].u8DomainId = 0u;
        rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u16Clean].eMsgType = GPTP_DEF_MSG_TYPE_UNKNOWN;
        rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u16Clean].u16SequenceId = 0u;
    }

    /* Port Init */
    for (u8Port = 0u; u8Port < u8NumberOfPorts; u8Port++)
    {
        rGptpDataStruct.prPerPortParams[u8Port].u8PortId = u8Port;
        if (GPTP_ERR_OK == GPTP_PORT_ObtainPortMac(u8Port, &u64PortMac))
        {
            rGptpDataStruct.prPerPortParams[u8Port].rSourceMac.u48Mac = (u64PortMac & (uint64_t)GPTP_DEF_BIT_MASK_16L_48H);

            prPdelayInit = &prInitParams->pcrPdelayMachinesParam[u8Port];
            rGptpDataStruct.prPerPortParams[u8Port].u64ClockId = prPdelayInit->u64ClockId;

            if (0u == u8Port)
            {
                /* Copy Unique identifier to the structure product details */
                GPTP_INTERNAL_CpyMac(&rGptpDataStruct, u64PortMac);
            }

        }
        else
        {
            eError = GPTP_ERR_I_NO_SOURCE_MAC;
            GPTP_ERR_Register(u8Port, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_I_NO_SOURCE_MAC, GPTP_ERR_SEQ_ID_NOT_SPECIF);
        }
    }

#ifdef GPTP_USE_PRINTF
    /* Print product details to console */
    GPTP_INTERNAL_PrintDetails(&rGptpDataStruct);
#endif

    /* Pdelay machines initialization */
    for (u8Port = 0u; u8Port < u8NumberOfPorts; u8Port++)
    {
        prPdelay = &rGptpDataStruct.prPdelayMachines[u8Port];

        prPdelayInit = &prInitParams->pcrPdelayMachinesParam[u8Port];

        prPdelay->u8GptpPort = u8Port;
        prPdelay->ePdelayInitiatorState = GPTP_DEF_INITIATOR_NOT_ENABLED;
        prPdelay->ePdelayResponderState = GPTP_DEF_RESPONDER_NOT_ENABLED;
        prPdelay->bComputeNeighborRateRatio = true;
        prPdelay->bComputeNeighborPropDelay = true;
        prPdelay->u16SequenceId = (uint16_t)GPTP_DEF_PDELAY_FIRST_SEQ_ID;
        prPdelay->u16LostResponses = 0u;
        prPdelay->u16PdelLostReponsesAllowedCnt = prPdelayInit->u16PdelLostReponsesAllowedCnt;
        prPdelay->u16MsmtsTillSlowDown = prPdelayInit->u16MeasurementsTillSlowDown;
        prPdelay->u16PdelayResponsesCnt = 0u;
        prPdelay->bPdelayReqReceived = false;
        prPdelay->bPdelayRespReceived = false;
        prPdelay->bPdelayRespFupReceived = false;
        prPdelay->s8InitPdelayIntervalLog = prPdelayInit->s8PdelIntervalLogInit;
        prPdelay->s8OperPdelayIntervalLog = prPdelayInit->s8PdelIntervalLogOper;
        prPdelay->s8PdelayIntervalLog = prPdelayInit->s8PdelIntervalLogInit;
        prPdelay->bT1TsRegistred = false;
        prPdelay->u16T1TsSeqId = 0u;
        prPdelay->bT3TsRegistred = false;
        prPdelay->u16T3TsSeqId = 0u;
        prPdelay->u64NeighborPropDelayThreshNs = prPdelayInit->u64NeighborPropDelayThreshNs;
        prPdelay->bPdelayInitiatorEnabled = prPdelayInit->bPdelayInitiatorEnabled;
        prPdelay->bPdelayInitiatorEnabledPrev = prPdelayInit->bPdelayInitiatorEnabled;
        prPdelay->bPdelayUnicastResp = prPdelayInit->bPdelayUnicastResp;
        prPdelay->bPdelayResponderEnabled = true;
        prPdelay->bNeighborRateRatioValidCurrent = false;
        prPdelay->bPdelayValueValidCurrent = false;
        prPdelay->f64TempRateRatioCurrent = 1.0;
        prPdelay->f64NeighborRateRatioCurrent = 1.0;
        prPdelay->f64NeighborPropDelayCurrent = 0.0;
        prPdelay->f64PdelayToWrite = 0.0;
        prPdelay->f64RratioToWrite = 0.0;
        prPdelay->rPdelayNvmWriteStat = GPTP_DEF_MEM_WRITE_STOP;
        prPdelay->rRratioNvmWriteStat = GPTP_DEF_MEM_WRITE_STOP;

        /* Init the pointer - port enabled from the port structure */
        prPdelay->pbPortEnabled = &rGptpDataStruct.prPerPortParams[u8Port].bPortEnabled;

        /* Init the pointer - Clock id of the port */
        prPdelay->pu64ClockId = &rGptpDataStruct.prPerPortParams[u8Port].u64ClockId;
        prPdelay->u32NvmAddressPdelay = prPdelayInit->u32NvmAddressPdelay;
        prPdelay->u32NvmAddressRratio = prPdelayInit->u32NvmAddressRratio;
        prPdelay->s16DelayAsymmetry = prPdelayInit->s16DelayAsymmetry;

        eErrorTmp = GPTP_PORT_InitPDelayStruct(u8Port, prPdelayInit, prPdelay);
        if (GPTP_ERR_OK != eErrorTmp)
        {
            eError = eErrorTmp;
        }

        /* Load pdelay value from the NVM */
        GPTP_PORT_ValueNvmRead(prPdelay, GPTP_DEF_NVM_PDELAY, &rGptpDataStruct);
        prPdelay->bPdelayValueValid = GPTP_INTERNAL_PdelayCheck(prPdelay->u8GptpPort,
                                                                (float64_t)prPdelay->u64NeighborPropDelayThreshNs,
                                                                prPdelay->f64NeighborPropDelay);

        if (false == prPdelay->bPdelayValueValid)
        {
            /* Value from NVM is above the threshold or it's NAN */
            prPdelay->f64NeighborPropDelay = 0.0;
        }

        /* Load neighbor rate ratio value from the NVM */
        GPTP_PORT_ValueNvmRead(prPdelay, GPTP_DEF_NVM_RRATIO, &rGptpDataStruct);
        prPdelay->bNeighborRateRatioValid = GPTP_INTERNAL_RateRratioCheck(rGptpDataStruct.rPerDeviceParams.f64RratioMaxDev, \
                                                                          &prPdelay->f64NeighborRateRatio);

        prPdelay->bPdelAveragerInitialized = false;
        prPdelay->bRatioAveragerInitialized = false;

        /* If the initial Pdelay interval is outside of the limit */
        if ((prPdelay->s8InitPdelayIntervalLog < (int8_t)GPTP_DEF_LOG_PDEL_INT_MIN) ||
            (prPdelay->s8InitPdelayIntervalLog > (int8_t)GPTP_DEF_LOG_PDEL_INT_MAX))
        {
            /* If the value is 127, it's outside of the limit but the value is reserved */
            /* Not considered error, the messages are not transmitted */
            if (GPTP_DEF_SIG_TLV_STOP_SENDING != prPdelay->s8InitPdelayIntervalLog)
            {
                eError = GPTP_ERR_I_PDEL_INTERVAL_OOR;
                GPTP_ERR_Register(u8Port, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_I_NO_SOURCE_MAC, GPTP_ERR_SEQ_ID_NOT_SPECIF);
            }
        }

        /* If the operational Pdelay interval is outside of the limit */
        if ((prPdelay->s8OperPdelayIntervalLog < (int8_t)GPTP_DEF_LOG_PDEL_INT_MIN) ||
           (prPdelay->s8OperPdelayIntervalLog > (int8_t)GPTP_DEF_LOG_PDEL_INT_MAX))
        {
            /* If the value is 127, it's outside of the limit but the value is reserved */
            /* Not considered error, the messages are not transmitted */
            if (GPTP_DEF_SIG_TLV_STOP_SENDING != prPdelay->s8OperPdelayIntervalLog)
            {
                eError = GPTP_ERR_I_PDEL_INTERVAL_OOR;
                GPTP_ERR_Register(u8Port, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_I_NO_SOURCE_MAC, GPTP_ERR_SEQ_ID_NOT_SPECIF);
            }
        }
    }

    /* Flag signalizing if there is a Clock Slave Domain - if the local clock is intended to be update */
    rGptpDataStruct.bLocClkToUpdate = false;

    /* Domains initialization */
    for (u8Domain = 0u; u8Domain < rGptpDataStruct.rPerDeviceParams.u8NumberOfDomains; u8Domain++)
    {
        prDomain = &rGptpDataStruct.prPerDomainParams[u8Domain];

        prDomainInit = &prInitParams->pcrDomainsSyncsParam[u8Domain];

#ifdef GPTP_COUNTERS
        if (NULL != prDomainInit->prCntrsDom)
        {
            prDomain->prCntrsDom = prDomainInit->prCntrsDom;
        }
        else
        {
            eError = GPTP_ERR_I_COUNTERS_NULL_PTR;
            GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, u8Domain, GPTP_ERR_I_COUNTERS_NULL_PTR, GPTP_ERR_SEQ_ID_NOT_SPECIF);
        }
#endif /* GPTP_COUNTERS */

        prDomain->u8DomainIndex = u8Domain;
        prDomain->u8DomainNumber = prDomainInit->u8DomainNumber;
        prDomain->bRcvdSyncUsedForLocalClk = false;
        prDomain->u8NumberOfSyncsPerDom = prDomainInit->u8NumberOfSyncMachines;
        prDomain->bDomainIsGm = prDomainInit->bDomainIsGm;
        prDomain->u32SyncOutlierThrNs = prDomainInit->u32SyncOutlierThrNs;
        prDomain->u8OutlierIgnoreCnt = prDomainInit->u8OutlierIgnoreCnt;
        prDomain->rLastValidGmPlusCor.u64TimeStampS = 0u;
        prDomain->rLastValidGmPlusCor.u32TimeStampNs = 0u;
        prDomain->rLastValidTsOfGmPlusCor.u64TimeStampS = 0u;
        prDomain->rLastValidTsOfGmPlusCor.u32TimeStampNs = 0u;
        prDomain->u8OutlierCnt = 0u;
        prDomain->u16VlanTci = prDomainInit->u16VlanTci;
        prDomain->rPrevPrecOrigTs.u64TimeStampS = 0u;
        prDomain->rPrevPrecOrigTs.u32TimeStampNs = 0u;
        prDomain->bGmFailureReported = false;
        prDomain->bSyncValidEverReceived = false;

        /* If the domain is non GM */
        if (false == prDomain->bDomainIsGm)
        {
            GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_INFO, ("gptp: Initializing Domain %u as Bridge.\n", prDomain->u8DomainNumber));

            /* Local clock is expected to be updated */
            rGptpDataStruct.bLocClkToUpdate = true;

            /* At the start-up, the non GM domain is advancing correction */
            prDomain->bSyncActingGm = true;

            if (true == prDomainInit->bSyncedGm)
            {
                eError = GPTP_ERR_I_SYNCED_GM_ON_NON_GM;
                GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, u8Domain, GPTP_ERR_I_SYNCED_GM_ON_NON_GM, GPTP_ERR_SEQ_ID_NOT_SPECIF);
            }
        }
        /* If the domain is Grand Master */
        else
        {

            GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_INFO, ("gptp: Initializing Domain %u as Grand Master.\n", prDomain->u8DomainNumber));

            /* At the start-up, the GM domain is not advancing correction */
            prDomain->bSyncActingGm = false;
        }

        prDomain->bSyncedGm = prDomainInit->bSyncedGm;

        /* If domain is synced GM */
        if (true == prDomainInit->bSyncedGm)
        {
            /* Check all domains to find matching domain number */
            for (u8Seek = 0u; u8Seek < rGptpDataStruct.rPerDeviceParams.u8NumberOfDomains; u8Seek++)
            {
                if (prDomainInit->u8RefDomForSynced == prInitParams->pcrDomainsSyncsParam[u8Seek].u8DomainNumber)
                {
                    /* Save reference domain number for synced GM */
                    prDomain->u8RefDomForSyncedGIndex = u8Seek;
                }
            }
        }

        prDomain->bValidSyncReceived = false;
        prDomain->s8SyncIntervalLogRcvd = 0;
        prDomain->bSignalingIntervalRcvd = false;
        prDomain->u16SigSequenceId = 0u;
        prDomain->u8StartupTimeoutS = prDomainInit->u8StartupTimeoutS;
        prDomain->u8SyncReceiptTimeoutCnt = prDomainInit->u8SyncReceiptTimeoutCnt;
        prDomain->u64TimerLastSyncRcvd = 0u;

        /* If the domain is Grand Master */
        if (true == prDomain->bDomainIsGm)
        {
            GPTP_PORT_TimeBaseInfoGet(&(prDomain->u16GmTimeBaseIndicator), &(prDomain->u32LastGmPhaseChangeH), \
                                      &(prDomain->u64LastGmPhaseChangeL), &(prDomain->u32ScaledLastGmFreqChange));
        }

        /* If startup timeout for the Bridge to receive first valid Sync from GM is too long */
        if ((uint8_t)GPTP_DEF_STARTUP_TIMEOUT_MAX < prDomain->u8StartupTimeoutS)
        {
            eError = GPTP_ERR_I_START_TOUT_TOO_LONG;
            GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, u8Domain, GPTP_ERR_I_START_TOUT_TOO_LONG, GPTP_ERR_SEQ_ID_NOT_SPECIF);
        }

        eErrorTmp = GPTP_PORT_InitDomainStruct(u8Domain, prDomainInit, prDomain);
        if (GPTP_ERR_OK != eErrorTmp)
        {
            eError = eErrorTmp;
        }

        /* Sync machines initialization for the domain */
        /* Clear slave found flag - for cfg param checking */
        bSlaveFound = false;
        for (u8Machine = 0u; u8Machine < prDomain->u8NumberOfSyncsPerDom; u8Machine++)
        {
            prSync = &prDomain->prSyncMachines[u8Machine];

            prSyncInit = &prDomainInit->pcrDomainSyncMachinesPtr[u8Machine];

            prSync->u8GptpPort = prSyncInit->u8GptpPort;
            prSync->bSyncRole = prSyncInit->bMachineRole;
            prSync->eSyncMasterState = GPTP_DEF_SYNC_MASTER_NOT_EN;
            prSync->eSyncSlaveState = GPTP_DEF_SYNC_SLAVE_NOT_EN;
            prSync->bSyncMsgReceived = false;
            prSync->bFupMsgReceived = false;
            prSync->bTsTsRegistred = false;
            prSync->bTrTsRegistred = false ;
            prSync->u16SequenceId = (uint16_t)GPTP_DEF_SYNC_FIRST_SEQ_ID;
            prSync->u64CorrectionSubNs = 0u;
            prSync->s8InitSyncIntervalLog = prSyncInit->s8SyncIntervalLog;
            prSync->s8OperSyncIntervalLog = prSyncInit->s8SyncIntervalLog;
            prSync->s8SyncIntervalLog = prSyncInit->s8SyncIntervalLog;
            prSync->f64RateRatio = 1.0;

            /* Check if accessing "valid" port - index < maximum number of ports */
            if (prSync->u8GptpPort < u8NumberOfPorts)
            {
                /* Init the pointer - path delay on the corresponding port */
                prSync->pf64NeighborPropDelay = &rGptpDataStruct.prPdelayMachines[prSync->u8GptpPort].f64NeighborPropDelay;
                /* Init the pointer - path Neighbor Rate Ratio on the corresponding port */
                prSync->pf64NeighborRateRatio = &rGptpDataStruct.prPdelayMachines[prSync->u8GptpPort].f64NeighborRateRatio;
                /* Init the pointer - port enabled from the port structure */
                prSync->pbPortEnabled = &rGptpDataStruct.prPerPortParams[prSync->u8GptpPort].bPortEnabled;
                /* Init the pointer - delay assymetry on the corresponding port */
                prSync->ps16DelayAsymmetry = &rGptpDataStruct.prPdelayMachines[prSync->u8GptpPort].s16DelayAsymmetry;
            }
            else
            {
                eError = GPTP_ERR_I_TOO_MANY_SYNCS_DOM;
                GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, u8Domain, GPTP_ERR_I_TOO_MANY_SYNCS_DOM, GPTP_ERR_SEQ_ID_NOT_SPECIF);
            }

            eErrorTmp = GPTP_PORT_InitSyncStruct(u8Machine, u8Domain, prSyncInit, prSync);
            if (GPTP_ERR_OK != eErrorTmp)
            {
                eError = eErrorTmp;
            }

            /* Check for non GM domain */
            if (false == prDomain->bDomainIsGm)
            {
                if (GPTP_DEF_PTP_SLAVE == prSync->bSyncRole)
                {
                    /* If slave already found, it means there are more than 1 slave machine */
                    if (true == bSlaveFound)
                    {
                        eError = GPTP_ERR_I_TOO_MANY_SLAV_DOM;
                        GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, u8Domain, GPTP_ERR_I_TOO_MANY_SLAV_DOM, GPTP_ERR_SEQ_ID_NOT_SPECIF);
                    }

                    /* Only for the first occurrence of the slave machine - in one domain */
                    else
                    {
                        /* Slave found */
                        bSlaveFound = true;
                        /* Remember Slave Machine ID */
                        prDomain->u8SlaveMachineId = u8Machine;

                        /* Check if pdelay machine for the port, on which is slave machine, has enabled pdelay
                           initiator */
                        if (prSync->u8GptpPort < u8NumberOfPorts)
                        {
                            if (false == rGptpDataStruct.prPdelayMachines[prSync->u8GptpPort].bPdelayInitiatorEnabled)
                            {
                                eError = GPTP_ERR_I_NO_PDELAY_FOR_SLAVE;
                                GPTP_ERR_Register(u8Machine, u8Domain, GPTP_ERR_I_NO_PDELAY_FOR_SLAVE, GPTP_ERR_SEQ_ID_NOT_SPECIF);
                            }
                        }
                    }
                }
            }

            /* Check for GM domain */
            else
            {
                if (GPTP_DEF_PTP_SLAVE == prSync->bSyncRole)
                {
                    /* Slave found */
                    bSlaveFound = true;
                }
            }

            /* If the Sync interval is outside of the limit */
            if ((prSync->s8SyncIntervalLog < (int8_t)GPTP_DEF_LOG_SYNC_INT_MIN) ||
               (prSync->s8SyncIntervalLog > (int8_t)GPTP_DEF_LOG_SYNC_INT_MAX))
            {
                /* If the value is 127, it's outside of the limit but the value is reserved */
                /* Not considered error, the messages are not transmitted */
                if (GPTP_DEF_SIG_TLV_STOP_SENDING != prSync->s8SyncIntervalLog)
                {
                    eError = GPTP_ERR_I_SYNC_INTERVAL_OOR;
                    GPTP_ERR_Register(u8Machine, u8Domain, GPTP_ERR_I_SYNC_INTERVAL_OOR, GPTP_ERR_SEQ_ID_NOT_SPECIF);
                }
            }
        }
        /* Check if master / slave configured properly for domain roles */
        /* Check for non GM domain */
        if (false == prDomain->bDomainIsGm)
        {
            /* If slave not found for Non GM domain */
            if (false == bSlaveFound)
            {
                /* Non GM domain must have slave machine */
                eError = GPTP_ERR_I_NO_SLAVE_NON_GM_DOM;
                GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, u8Domain, GPTP_ERR_I_NO_SLAVE_NON_GM_DOM, GPTP_ERR_SEQ_ID_NOT_SPECIF);
            }
        }
        /* Check for GM domain */
        else
        {
            /* If slave not found for non GM domain */
            if (true == bSlaveFound)
            {
                /* GM must not have slave machine */
                eError = GPTP_ERR_I_SLAVE_ON_GM_DOMAIN;
                GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, u8Domain, GPTP_ERR_I_SLAVE_ON_GM_DOMAIN, GPTP_ERR_SEQ_ID_NOT_SPECIF);
            }
        }
    }

    /* If no error */
    if (GPTP_ERR_OK == eError)
    {
    /* Init timers */
        GPTP_TIMER_TimerInit(&rGptpDataStruct);
    }

    /* Dummy read of the Link status */
    for (u8Port = 0u; u8Port < u8NumberOfPorts; u8Port++)
    {
        /* Dummy status read ignores return value */
        (void)GPTP_PORT_GetLinkStatus(u8Port, &bLinkStat);
    }

    /* If no error */
    if (GPTP_ERR_OK == eError)
    {
        /* Port Init */
        for (u8Port = 0u; u8Port < u8NumberOfPorts; u8Port++)
        {
            prPdelay = &rGptpDataStruct.prPdelayMachines[u8Port];
            prPort= &rGptpDataStruct.prPerPortParams[u8Port];

            /* If the link is up */
            if (GPTP_ERR_OK != GPTP_PORT_GetLinkStatus(u8Port, &bLinkStat))
            {
                bLinkStat = false;
                eError = GPTP_ERR_I_NO_LINK_STATUS;
                GPTP_ERR_Register(u8Port, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_I_NO_LINK_STATUS, GPTP_ERR_SEQ_ID_NOT_SPECIF);
            }

            /* Link up */
            if (true == bLinkStat)
            {
                /* Port - enabled */
                prPort->bPortEnabled = true;

                /* Pdelay machines */
                prPdelay->bTimerPdelayTimerEnabled = true;

                /* Sync machines */
                for (u8Domain = 0u; u8Domain < rGptpDataStruct.rPerDeviceParams.u8NumberOfDomains; u8Domain++)
                {
                    prDomain = &rGptpDataStruct.prPerDomainParams[u8Domain];
                    for (u8Machine = 0u; u8Machine < prDomain->u8NumberOfSyncsPerDom; u8Machine++)
                    {
                        prSync = &prDomain->prSyncMachines[u8Machine];
                        if (u8Port == prSync->u8GptpPort)
                        {
                            prSync->bTimerSyncEnabled = true;
                        }
                    }
                }
            }

            /* Link down */
            else
            {
                /* Port - disabled */
                prPort->bPortEnabled = false;

                /* Pdelay machines */
                prPdelay->bTimerPdelayTimerEnabled = false;

                /* Sync machines */
                for (u8Domain = 0u; u8Domain < rGptpDataStruct.rPerDeviceParams.u8NumberOfDomains; u8Domain++)
                {
                    prDomain = &rGptpDataStruct.prPerDomainParams[u8Domain];
                    for (u8Machine = 0u; u8Machine < prDomain->u8NumberOfSyncsPerDom; u8Machine++)
                    {
                        prSync = &prDomain->prSyncMachines[u8Machine];
                        if (u8Port == prSync->u8GptpPort)
                        {
                            prSync->bTimerSyncEnabled = false;
                        }
                    }
                }
            }
        }
    }

    /* Reset sync lock and report to the app. */
    GPTP_INTERNAL_ResetSyncLock(&rGptpDataStruct);

    /* If no error */
    if (GPTP_ERR_OK == eError)
    {
        rGptpDataStruct.bGptpInitialized = true;

        GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_STATE, ("gptp: Initialization of gPTP stack version "));
        GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_STATE, (GPTP_DEF_GPTP_REV));
        GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_STATE, (" complete\n"));

#ifdef GPTP_COUNTERS
        GPTP_INTERNAL_ClearStats(&rGptpDataStruct);
#endif /* GPTP_COUNTERS */

    }
    return eError;
}

/*!
 *
 * @brief   This function handles the time stamps generated by the gPTP message transmission.
 *
 * @details This function handles the time stamps generated by the gPTP message transmission.
 *          The time stamp values are stored to the internal gPTP structures by the port number
 *          and frame ID for the future use.
 *
 * @param[in] u8Port gPTP port number from which the time stamp is received.
 * @param[in] u32TxTsS Tx time stamp value, seconds part.
 * @param[in] u32TxTsNs Tx time stamp value, nano seconds part.
 * @param[in] u8FrameId Id of frame, which generated the time stamp.
 *
 * @ requirements 529199
 * @ requirements 529201
 */
void GPTP_TimeStampHandler(uint8_t u8Port,
                           uint32_t u32TxTsS,
                           uint32_t u32TxTsNs,
                           uint8_t u8FrameId)
{
    const gptp_def_domain_t        *prDomain;
    uint8_t                        u8Domain;
    uint8_t                        u8Frame;
    uint8_t                        u8Machine;
    gptp_def_sync_t                *prSyncMachine;
    gptp_def_pdelay_t              *prPdelayMachine;

    /* Process timestamps only if the gPTP is initialized */
    if (true == rGptpDataStruct.bGptpInitialized)
    {
        u8Frame = u8FrameId;

        /* Process timestamp */
        if (rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8Frame].u8Port == u8Port)
        {
            u8Machine = rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8Frame].u8MachineId;
            u8Domain =  rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8Frame].u8DomainId;

            switch (rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8Frame].eMsgType)
            {
                case GPTP_DEF_MSG_TYPE_SYNC:
                    if (u8Frame < GPTP_DEF_FID_SYNC_MAX)
                    {
                        if (u8Domain < rGptpDataStruct.rPerDeviceParams.u8NumberOfDomains)
                        {
                            prDomain = &rGptpDataStruct.prPerDomainParams[u8Domain];
                            if (u8Machine < prDomain->u8NumberOfSyncsPerDom)
                            {
                                prSyncMachine = &prDomain->prSyncMachines[u8Machine];
                                prSyncMachine->rTsTs.u64TimeStampS = (uint64_t)u32TxTsS;
                                prSyncMachine->rTsTs.u32TimeStampNs = u32TxTsNs;
                                prSyncMachine->bTsTsRegistred = true;
                                prSyncMachine->u16TsTsSeqId = rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8Frame].u16SequenceId;
                                prSyncMachine->bSyncActingGm = rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8Frame].bSyncActingGm;
                                GPTP_SYNC_SyncMachine(&rGptpDataStruct, u8Domain, u8Machine, GPTP_DEF_CALL_ON_TIMESTAMP);
                            }
                            else
                            {
                                GPTP_ERR_Register(u8Machine, u8Domain, GPTP_ERR_T_SYNC_MACHINE_INDEX, rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8Frame].u16SequenceId);
                            }
                        }
                        else
                        {
                            GPTP_ERR_Register(u8Machine, u8Domain, GPTP_ERR_T_SYNC_DOMAIN_INDEX, rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8Frame].u16SequenceId);
                        }
                    }

                    else
                    {
                        GPTP_ERR_Register(u8Machine, u8Domain, GPTP_ERR_T_FRAME_ID_ERROR, rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8Frame].u16SequenceId);
                    }
                break;

                case GPTP_DEF_MSG_TYPE_PD_REQ:
                    if ((u8Frame >= GPTP_DEF_FID_PDREQ_START) && (u8Frame < GPTP_DEF_FID_PDREQ_MAX))
                    {
                        if (u8Machine < rGptpDataStruct.rPerDeviceParams.u8NumberOfPdelayMachines)
                        {
                            prPdelayMachine = &rGptpDataStruct.prPdelayMachines[u8Machine];
                            prPdelayMachine->rT1Ts.u64TimeStampS = (uint64_t)u32TxTsS;
                            prPdelayMachine->rT1Ts.u32TimeStampNs = u32TxTsNs;
                            prPdelayMachine->bT1TsRegistred = true;
                            prPdelayMachine->u16T1TsSeqId = rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8Frame].u16SequenceId;
                            /* Running the same code sequence as if it is called upon the message received */
                            GPTP_PDELAY_PdelayMachine(&rGptpDataStruct, prPdelayMachine, GPTP_DEF_CALL_PTP_RECEIVED);
                        }
                        else
                        {
                            GPTP_ERR_Register(u8Machine, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_T_PDEL_MACHINE_INDEX, \
                                              rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8Frame].u16SequenceId);
                        }
                    }

                    else
                    {
                        GPTP_ERR_Register(u8Machine, u8Domain, GPTP_ERR_T_FRAME_ID_ERROR, \
                                          rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8Frame].u16SequenceId);
                    }
                break;

                case GPTP_DEF_MSG_TYPE_PD_RESP:
                    if ((u8Frame >= GPTP_DEF_FID_PDRESP_START) && (u8Frame < GPTP_DEF_FID_PDRESP_MAX))
                    {
                        if (u8Machine < rGptpDataStruct.rPerDeviceParams.u8NumberOfPdelayMachines)
                        {
                            prPdelayMachine = &rGptpDataStruct.prPdelayMachines[u8Machine];
                            prPdelayMachine->rT3Ts.u64TimeStampS = (uint64_t)u32TxTsS;
                            prPdelayMachine->rT3Ts.u32TimeStampNs = u32TxTsNs;
                            prPdelayMachine->bT3TsRegistred = true;
                            prPdelayMachine->u16T3TsSeqId = rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8Frame].u16SequenceId;
                            GPTP_PDELAY_PdelayMachine(&rGptpDataStruct, prPdelayMachine, GPTP_DEF_CALL_ON_TIMESTAMP);
                        }
                        else
                        {
                            GPTP_ERR_Register(u8Machine, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_T_PDEL_MACHINE_INDEX, \
                                              rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8Frame].u16SequenceId);
                        }
                    }

                    else
                    {
                        GPTP_ERR_Register(u8Machine, u8Domain, GPTP_ERR_T_FRAME_ID_ERROR, rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8Frame].u16SequenceId);
                    }
                break;

                default:
                break;
            }
#ifdef GPTP_MIRROR_PORT
            /* Mark frame ID entry used in case the switch is mirroring gPTP messages */
            rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8Frame].u8Port = (uint8_t)0xFFu;
            rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8Frame].u8MachineId = (uint8_t)0xFFu;
            rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8Frame].u8DomainId  = (uint8_t)0xFFu;
            rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8Frame].eMsgType = GPTP_DEF_MSG_TYPE_UNKNOWN;
            rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8Frame].u16SequenceId = 0u;
            rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8Frame].bSyncActingGm = false;
        }
#else
        }
        /* Port ID does not match, register error */
        else
        {
            GPTP_ERR_Register(rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8Frame].u8MachineId, \
                rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8Frame].u8DomainId, GPTP_ERR_T_TS_NOT_IDENTIFIED, \
                rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8Frame].u16SequenceId);
        }

        /* Mark frame ID entry used in normal operation */
        rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8Frame].u8Port = (uint8_t)0xFFu;
        rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8Frame].u8MachineId = (uint8_t)0xFFu;
        rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8Frame].u8DomainId  = (uint8_t)0xFFu;
        rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8Frame].eMsgType = GPTP_DEF_MSG_TYPE_UNKNOWN;
        rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8Frame].u16SequenceId = 0u;
        rGptpDataStruct.rPerDeviceParams.arFrameIdTable[u8Frame].bSyncActingGm = false;
#endif /* GPTP_MIRROR_PORT */
    }
}

/*!
 *
 * @brief   This function updates the TX frame table.
 *
 * @details This function updates the TX frame table with the time stamp and port ID
 *          value for particular entry.
 *
 * @param[in] u8Port gPTP port number received the time stamp.
 * @param[in] u32BufferIndex Index of the frame buffer.
 * @param[in] u32TsSeconds Egress time stamp - seconds.
 * @param[in] u32TsNanoseconds Egress time stamp - nano seconds.
 *
 * @ requirements 535652
 * @ requirements 529199
 */
void GPTP_UpdateTimestampEntry(uint8_t u8Port,
                               uint32_t u32BufferIndex,
                               uint32_t u32TsSeconds,
                               uint32_t u32TsNanoseconds)
{
    const gptp_def_pdelay_t *prPdelay;
    const gptp_def_domain_t *prDomain;
    const gptp_def_sync_t   *prSync;
    uint8_t u8Domain;
    uint8_t u8Machine;
    
    /* Store Timestamp entry only if the gPTP is initialized */
    if (true == rGptpDataStruct.bGptpInitialized)
    {
        prPdelay = &rGptpDataStruct.prPdelayMachines[u8Port];

        if (prPdelay->rTxDataReq.prFrameMap->u32BufferIndex == u32BufferIndex)
        {
            prPdelay->rTxDataReq.prFrameMap->u32EgressTimeStampSeconds = u32TsSeconds;
            prPdelay->rTxDataReq.prFrameMap->u32EgressTimeStampNanoseconds = u32TsNanoseconds;
            prPdelay->rTxDataReq.prFrameMap->u8EgressPort = u8Port;
            prPdelay->rTxDataReq.prFrameMap->eTsEntryStatus = GPTP_DEF_TS_MAP_ENTRY_CNFRMD;
        }
        
        if (prPdelay->rTxDataResp.prFrameMap->u32BufferIndex == u32BufferIndex)
        {
            prPdelay->rTxDataResp.prFrameMap->u32EgressTimeStampSeconds = u32TsSeconds;
            prPdelay->rTxDataResp.prFrameMap->u32EgressTimeStampNanoseconds = u32TsNanoseconds;
            prPdelay->rTxDataResp.prFrameMap->u8EgressPort = u8Port;
            prPdelay->rTxDataResp.prFrameMap->eTsEntryStatus = GPTP_DEF_TS_MAP_ENTRY_CNFRMD;
        }
        
        /* Search in each domain */
        for (u8Domain = 0u; u8Domain < rGptpDataStruct.rPerDeviceParams.u8NumberOfDomains; u8Domain++)
        {
            prDomain = &rGptpDataStruct.prPerDomainParams[u8Domain];
            
            /* Search in each sync machine */ 
            for (u8Machine = 0u; u8Machine < prDomain->u8NumberOfSyncsPerDom; u8Machine++)
            {
                prSync = &prDomain->prSyncMachines[u8Machine];
                if (prSync->u8GptpPort == u8Port)
                {
                    if (prSync->rTxDataSyn.prFrameMap->u32BufferIndex == u32BufferIndex)
                    {
                        prSync->rTxDataSyn.prFrameMap->u32EgressTimeStampSeconds = u32TsSeconds;
                        prSync->rTxDataSyn.prFrameMap->u32EgressTimeStampNanoseconds = u32TsNanoseconds;
                        prSync->rTxDataSyn.prFrameMap->u8EgressPort = u8Port;
                        prSync->rTxDataSyn.prFrameMap->eTsEntryStatus = GPTP_DEF_TS_MAP_ENTRY_CNFRMD;
                    }
                }
            }
        }
    }
}

/*!
 *
 * @brief   This function receives the gPTP message.
 *
 * @details This function inspects the gPTP message type and extracts the gPTP data from
 *          the message.
 *
 * @param[in] rRxData structure containing all the necessary gPTP data for processing.
 *
 * @ requirements 529199
 * @ requirements 529202
 * @ requirements 152455
 * @ requirements 120457
 */
void GPTP_MsgReceive(gptp_def_rx_data_t rRxData)
{
    static uint8_t                 u8RcvdForMachine;
    static uint8_t                 u8RcvdForDomain;
    static gptp_def_msg_type_t     rMsgTypeRcvd;
    gptp_err_type_t                eError;
    gptp_def_domain_t              *prDomain;
    gptp_def_sync_t                *prSyncMachine;
    gptp_def_pdelay_t              *prPdelayMachine;

    /* Process received message only if the gPTP is initialized */
    if (true == rGptpDataStruct.bGptpInitialized)
    {
        eError = GPTP_FRAME_ParseRx(rRxData, &rGptpDataStruct, &rMsgTypeRcvd, &u8RcvdForDomain, &u8RcvdForMachine);

        if (GPTP_ERR_OK == eError)
        {
            prPdelayMachine = &rGptpDataStruct.prPdelayMachines[u8RcvdForMachine];
            switch (rMsgTypeRcvd)
            {
                case GPTP_DEF_MSG_TYPE_SYNC:
                case GPTP_DEF_MSG_TYPE_FOLLOW_UP:
                    prDomain = &rGptpDataStruct.prPerDomainParams[u8RcvdForDomain];
                    prSyncMachine = &prDomain->prSyncMachines[u8RcvdForMachine];

                    /* Domain which receives sync message, can't be GM */
                    if (false == prDomain->bDomainIsGm)
                    {
                        /* Check if the received sync / fup is for slave machine of the domain */
                        if (prDomain->u8SlaveMachineId == u8RcvdForMachine)
                        {
                            GPTP_SYNC_SyncMachine(&rGptpDataStruct, u8RcvdForDomain, u8RcvdForMachine, GPTP_DEF_CALL_PTP_RECEIVED);

                            /* If followup received */
                            if (GPTP_DEF_MSG_TYPE_FOLLOW_UP == rMsgTypeRcvd)
                            {
                                /* Send sync messages on non GM domain (bridge) */
                                GPTP_TIMER_SyncsSendNonGm(&rGptpDataStruct, prDomain, GPTP_DEF_CALL_PTP_RECEIVED);
                            }
                        }
                        else
                        {
                            /* If the sync message is received on the different machine than slave */
                            if (true == prSyncMachine->bSyncMsgReceived)
                            {
                                /* clear flag - sync message received */
                                prSyncMachine->bSyncMsgReceived = false;
                                /* Register error - sync received on master machine */
                                GPTP_ERR_Register(u8RcvdForMachine, u8RcvdForDomain, GPTP_ERR_M_SYNC_ON_MASTER_RCVD, GPTP_ERR_SEQ_ID_NOT_SPECIF);
#ifdef GPTP_COUNTERS
                                GPTP_INTERNAL_IncrementPortStats(&rGptpDataStruct, rRxData.u8PtpPort, ieee8021AsPortStatRxPTPPacketDiscard);
                                GPTP_INTERNAL_IncrementDomainStats(&rGptpDataStruct, u8RcvdForDomain, u8RcvdForMachine, ieee8021AsPortStatRxPTPPacketDiscard);
#endif /* GPTP_COUNTERS */
                            }

                            /* If the fup message is received on the different port than slave */
                            if (true == prSyncMachine->bFupMsgReceived)
                            {
                                /* clear flag - fup message received */
                                prSyncMachine->bFupMsgReceived = false;
                                /* Register error - fup received on master port */
                                GPTP_ERR_Register(u8RcvdForMachine, u8RcvdForDomain, GPTP_ERR_M_FUP_ON_MASTER_RCVD, GPTP_ERR_SEQ_ID_NOT_SPECIF);
#ifdef GPTP_COUNTERS
                                GPTP_INTERNAL_IncrementPortStats(&rGptpDataStruct, rRxData.u8PtpPort, ieee8021AsPortStatRxPTPPacketDiscard);
                                GPTP_INTERNAL_IncrementDomainStats(&rGptpDataStruct, u8RcvdForDomain, u8RcvdForMachine, ieee8021AsPortStatRxPTPPacketDiscard);
#endif /* GPTP_COUNTERS */
                            }
                        }
                    }

                    /* Domain is GM - Error */
                    else
                    {
                        /* If the sync message is received by Grand Master */
                        if (true == prSyncMachine->bSyncMsgReceived)
                        {
                            /* clear flag - sync message received */
                            prSyncMachine->bSyncMsgReceived = false;
                            /* Register error - sync received by Grand Master */
                            GPTP_ERR_Register(u8RcvdForMachine, u8RcvdForDomain, GPTP_ERR_M_SYNC_ON_GM_RCVD, GPTP_ERR_SEQ_ID_NOT_SPECIF);
#ifdef GPTP_COUNTERS
                            GPTP_INTERNAL_IncrementPortStats(&rGptpDataStruct, rRxData.u8PtpPort, ieee8021AsPortStatRxPTPPacketDiscard);
                            GPTP_INTERNAL_IncrementDomainStats(&rGptpDataStruct, u8RcvdForDomain, u8RcvdForMachine, ieee8021AsPortStatRxPTPPacketDiscard);
#endif /* GPTP_COUNTERS */
                        }

                        /* If the fup message is received by Grand Master */
                        if (true == prSyncMachine->bFupMsgReceived)
                        {
                            /* clear flag - fup message received */
                            prSyncMachine->bFupMsgReceived = false;
                            /* Register error - fup received by Grand Master */
                            GPTP_ERR_Register(u8RcvdForMachine, u8RcvdForDomain, GPTP_ERR_M_FUP_ON_GM_RCVD, GPTP_ERR_SEQ_ID_NOT_SPECIF);
#ifdef GPTP_COUNTERS
                            GPTP_INTERNAL_IncrementPortStats(&rGptpDataStruct, rRxData.u8PtpPort, ieee8021AsPortStatRxPTPPacketDiscard);
                            GPTP_INTERNAL_IncrementDomainStats(&rGptpDataStruct, u8RcvdForDomain, u8RcvdForMachine, ieee8021AsPortStatRxPTPPacketDiscard);
#endif /* GPTP_COUNTERS */
                        }
                    }

                break;

                case GPTP_DEF_MSG_TYPE_PD_REQ:
                    prPdelayMachine->u64SourceMac = rRxData.u64SourceMac;
                    /* fallthrough */
                case GPTP_DEF_MSG_TYPE_PD_RESP:
                case GPTP_DEF_MSG_TYPE_PD_RESP_FUP:
                    GPTP_PDELAY_PdelayMachine(&rGptpDataStruct, prPdelayMachine, GPTP_DEF_CALL_PTP_RECEIVED);
                break;

                case GPTP_DEF_MSG_TYPE_SIGNALING:
                    GPTP_SIGNALING_RxSignaling(&rGptpDataStruct, u8RcvdForDomain, u8RcvdForMachine);
                break;

                default:
                break;
            }
        }
    }
}

/*!
 *
 * @brief   This function should be called upon the "link up" event on the specific
 *          gPTP port.
 *
 * @details This function is called upon the link up event at the specific port
 *          and configures the gPTP stack accordingly (enables software timers,
 *          reset sync and pdelay intervals).
 *
 * @param[in] u8Switch  Number of the respective switch.
 * @param[in] u8Port    Number of the respective port on switch.
 *
 * @ requirements 529199
 * @ requirements 529202
 * @ requirements 529203
 */
void GPTP_LinkUpNotify(uint8_t u8Switch,
                       uint8_t u8Port)
{

    static uint8_t u8GptpPort;

    u8GptpPort = 0u;

    /* Call function only if the gPTP is initialized */
    if (true == rGptpDataStruct.bGptpInitialized)
    {
        if (GPTP_ERR_OK == GPTP_PORT_PortLookup(&u8GptpPort, u8Port, u8Switch))
        {
            GPTP_INTERNAL_LinkUp(u8GptpPort, &rGptpDataStruct);
        }
    }
}

/*!
 *
 * @brief   This function should be called upon the "link down" event on the specific
 *          gPTP port.
 *
 * @details This function is called upon the link down event at the specific port
 *          and configures the port accordingly (disables software timers, resets
 *          sync and pdelay state machines).
 *
 * @param[in] u8Switch  Number of the respective switch.
 * @param[in] u8Port    Number of the respective port on switch.
 *
 * @ requirements 529199
 * @ requirements 529204
 */
void GPTP_LinkDownNotify(uint8_t u8Switch,
                         uint8_t u8Port)
{
    static uint8_t u8GptpPort;

    u8GptpPort = 0u;

    /* Call function only if the gPTP is initialized */
    if (true == rGptpDataStruct.bGptpInitialized)
    {
        if (GPTP_ERR_OK == GPTP_PORT_PortLookup(&u8GptpPort, u8Port, u8Switch))
        {
            GPTP_INTERNAL_LinkDown(u8GptpPort, &rGptpDataStruct);
        }
    }
}

/*!
 *
 * @brief   This function increments gPTP internal software timer
 *
 * @details This function should be called once upon certain period.
 *          Recommended call period is 10ms.
 *
 * @ requirements 529199
 * @ requirements 529205
 */
void GPTP_TimerPeriodic(void)
{
    /* Call periodic function only if the gPTP is initialized */
    if (true == rGptpDataStruct.bGptpInitialized)
    {
        GPTP_TIMER_TimerPeriodic(&rGptpDataStruct);
    }
}

/*!
 *
 * @brief     This function provides access to the error log.
 *
 * @details   This function provides access to the error log. The function
 *            returns error detail stored at the log index.
 *
 * @param[in] u16ErrIndex Error log index.
 * @return    Error logged at the index.
 *
 * @ requirements 529199
 * @ requirements 529206
 */
gptp_err_error_t GPTP_ErrReadIndex(uint16_t u16ErrIndex)
{
    gptp_err_error_t rErrorDetail;

    /* Call error read function only if the gPTP is initialized */
    if (true == rGptpDataStruct.bGptpInitialized)
    {
        rErrorDetail = GPTP_ERR_ReadIndex(u16ErrIndex);
    }

    /* Stack not initialized */
    else
    {
        rErrorDetail.eErrorType = GPTP_ERR_I_NOT_INITIALIZED;
        rErrorDetail.u8RespGptpPortIndex = 0u;
        rErrorDetail.u8RespGptpDomainIndex = 0u;
        rErrorDetail.rLogTime.u64TimeStampS = 0u;
        rErrorDetail.rLogTime.u32TimeStampNs = 0u;
        rErrorDetail.u16RespMsgSeqId = 0u;
        rErrorDetail.bFreshLog = false;

        GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_I_NOT_INITIALIZED, GPTP_ERR_SEQ_ID_NOT_SPECIF);
    }

    return rErrorDetail;
}

/*!
 *
 * @brief       This function gets current sync ingerval from the specified domain/sync machine
 *
 * @details     The function provides current sync interval of the respective sync machine,
 *              for both - Master sync machines and Slave sync machines.
 *
 * @param[in]   u8Domain Domain number on which the sync interval is updated.
 * @param[in]   u8Machine Sync machine index on which the sync interval is updated.
 * @param[out]  s8SyncInterval Pointer to sync interval variable.
 *
 * @return      gPTP error code.
 *
 * @ requirements 529199
 * @ requirements 529207
 */
gptp_err_type_t GPTP_SyncIntervalGet(uint8_t u8Domain,
                                     uint8_t u8Machine,
                                     int8_t *s8SyncInterval)
{
    const gptp_def_domain_t     *prDomain;
    gptp_err_type_t             eError;
    uint8_t                     u8DomainNum;
    uint8_t                     u8DomainSeek;
    bool                        bDomainFound;

    eError = GPTP_ERR_OK;

    /* If the gPTP is initialized */
    if (true == rGptpDataStruct.bGptpInitialized)
    {
        bDomainFound = false;
        /* Search for domain number */
        for (u8DomainSeek = 0u; u8DomainSeek < rGptpDataStruct.rPerDeviceParams.u8NumberOfDomains; u8DomainSeek++)
        {
            prDomain = &rGptpDataStruct.prPerDomainParams[u8DomainSeek];
            if (u8Domain == prDomain->u8DomainNumber)
            {
                bDomainFound = true;
                u8DomainNum = u8Domain;
            }
        }

        /* If domain number found */
        if (true == bDomainFound)
        {
            prDomain = &rGptpDataStruct.prPerDomainParams[u8DomainNum];
            /* Check if machine ID is valid (lower than max number of sync machines under the domain) */
            if (u8Machine < prDomain->u8NumberOfSyncsPerDom)
            {
                /* Read and save sync interval */
                *s8SyncInterval = prDomain->prSyncMachines[u8Machine].s8SyncIntervalLog;
            }
            else
            {
                /* In case of invalid sync machine ID, report error */
                eError = GPTP_ERR_API_ILLEGAL_MACHINE_ID;
                GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_API_ILLEGAL_MACHINE_ID, GPTP_ERR_SEQ_ID_NOT_SPECIF);
            }
        }
        else
        {
            /* In case the domain number not found, report error */
            eError = GPTP_ERR_API_UNKNOWN_DOMAIN;
            GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_API_UNKNOWN_DOMAIN, GPTP_ERR_SEQ_ID_NOT_SPECIF);
        }
    }

    else
    {
        /* Stack not initialized */
        eError = GPTP_ERR_I_NOT_INITIALIZED;
        GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_I_NOT_INITIALIZED, GPTP_ERR_SEQ_ID_NOT_SPECIF);
    }

    /* Return error */
    return eError;
}

/*!
 *
 * @brief     This function sets sync interval for the specified domain/sync machine
 *
 * @details   For the Master sync machine, the API sets new sync interval. For Slave
 *            sync machine, the API transmits signaling message with sync interval update
 *            request.
 *
 * @param[in] u8Domain Domain number on which the sync interval is updated.
 * @param[in] u8Machine Sync machine index on which the sync interval is updated.
 * @param[in] s8SyncInterval New sync interval.
 *
 * @return    gPTP error code.
 *
 * @ requirements 529199
 * @ requirements 529208
 */
gptp_err_type_t GPTP_SyncIntervalSet(uint8_t u8Domain,
                                     uint8_t u8Machine,
                                     int8_t s8SyncInterval)
{
    const gptp_def_domain_t     *prDomain;
    gptp_err_type_t             eError;
    uint8_t                     u8DomainNum;
    uint8_t                     u8DomainSeek;
    bool                        bDomainFound;

    eError = GPTP_ERR_OK;

    /* If the gPTP is initialized */
    if (true == rGptpDataStruct.bGptpInitialized)
    {
        bDomainFound = false;

        /* Search for domain number */
        for (u8DomainSeek = 0u; u8DomainSeek < rGptpDataStruct.rPerDeviceParams.u8NumberOfDomains; u8DomainSeek++)
        {
            prDomain = &rGptpDataStruct.prPerDomainParams[u8DomainSeek];
            if (u8Domain == prDomain->u8DomainNumber)
            {
                bDomainFound = true;
                u8DomainNum = u8Domain;
            }
        }

        /* If domain number found */
        if (true == bDomainFound)
        {
            prDomain = &rGptpDataStruct.prPerDomainParams[u8DomainNum];
            /* Check if machine ID is valid (lower than max number of sync machines under the domain) */
            if (u8Machine < prDomain->u8NumberOfSyncsPerDom)
            {
                /* Update operating sync interval for the defined domain / machine */
                prDomain->prSyncMachines[u8Machine].s8OperSyncIntervalLog = s8SyncInterval;

                /* Master machine */
                if (true == prDomain->prSyncMachines[u8Machine].bSyncRole)
                {
                    /* Update sync interval for the defined domain / machine */
                    prDomain->prSyncMachines[u8Machine].s8SyncIntervalLog = s8SyncInterval;
                }
                /* Slave machine */
                else
                {
                    /* Transmit signaling message to the upstream Master port, requesting sync interval update */
                    GPTP_SIGNALING_SendSignaling(&rGptpDataStruct, u8DomainNum, s8SyncInterval);
                }
            }
            else
            {
                /* In case of invalid sync machine ID, report error */
                eError = GPTP_ERR_API_ILLEGAL_MACHINE_ID;
                GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_API_ILLEGAL_MACHINE_ID, GPTP_ERR_SEQ_ID_NOT_SPECIF);
            }
        }
        else
        {
            /* In case the domain number not found, report error */
            eError = GPTP_ERR_API_UNKNOWN_DOMAIN;
            GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_API_UNKNOWN_DOMAIN, GPTP_ERR_SEQ_ID_NOT_SPECIF);
        }
    }

    else
    {
        /* Stack not initialized */
        eError = GPTP_ERR_I_NOT_INITIALIZED;
        GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_I_NOT_INITIALIZED, GPTP_ERR_SEQ_ID_NOT_SPECIF);
    }

    /* Return error */
    return eError;
}

/*!
 *
 * @brief       This function provides current offset to GM
 *
 * @details     The function provides current offset of the local clock to the Grand Master.
 *              If the local clock is not updated after startup, the retur value is 0x0x7FFFFFFF.
 *
 * @param[Out]  rOffset Pointer to the Timestamp value of current GM offset.
 *
 * @return      gPTP error code.
 * 
 * @ requirements 529199
 * @ requirements 529209
 */
gptp_err_type_t GPTP_CurrentOffsetGet(gptp_def_timestamp_sig_t *rOffset)
{
    gptp_err_type_t             eError;

    eError = GPTP_ERR_OK;

    /* If the gPTP is initialized */
    if (true == rGptpDataStruct.bGptpInitialized)
    {
        if ((true == rGptpDataStruct.rPerDeviceParams.bEverUpdated) && 
            (true == rGptpDataStruct.rPerDeviceParams.bEstimGmOffsetCalculated))
        {
            *rOffset = rGptpDataStruct.rPerDeviceParams.rEstimGmOffset;
        }
        else
        {
            rOffset->s64TimeStampS = 0x7FFFFFFFFFFFFFFF;
            rOffset->s32TimeStampNs = 0x7FFFFFFF;
        }
    }

    else
    {
        /* Stack not initialized */
        eError = GPTP_ERR_I_NOT_INITIALIZED;
        GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_I_NOT_INITIALIZED, GPTP_ERR_SEQ_ID_NOT_SPECIF);
    }

    /* Return error */
    return eError;
}

/*!
 *
 * @brief      Get pointer to the main structure holding gPTP states and configuration.
 *
 * @return      Pointer to the main gPTP structure.
 *
 */
gptp_def_data_t* GPTP_GetMainStrPtr(void)
{
    return &rGptpDataStruct;
}

#ifdef GPTP_COUNTERS
/*!
 *
 * @brief       This function gets value of the gPTP counter.
 *
 * @details     The function reads the value of respective gPTP counter.
 *
 * @param[in]   u16Domain Domain number (uint16). Value 0xFFFF reads port counter value.
 * @param[in]   u8Port Port number.
 * @param[in]   rCntr gPTP Counter.
 * @param[out]  u32Value Pointer to the uint32_t value providing counter value.
 *
 * @return      gPTP error code.
 *
 * @ requirements 529199
 */
gptp_err_type_t GPTP_GetStatsValue(uint16_t u16Domain,
                                   uint8_t u8Port,
                                   gptp_def_counters_t rCntr,
                                   uint32_t *u32Value)
{
    gptp_err_type_t eErr;

    if (true == rGptpDataStruct.bGptpInitialized)
    {
        /* If the stack is initialized */
        eErr = GPTP_INTERNAL_GetStatsValue(&rGptpDataStruct, u16Domain, u8Port, rCntr, u32Value);
    }

    else
    {
        /* Stack not initialized */
        eErr = GPTP_ERR_I_NOT_INITIALIZED;
    }
    return eErr;
}

/*!
 *
 * @brief       This function clears all gPTP counters
 *
 * @details     The function clears gPTP counters. Domain and Port counters.
 *
 * @return      gPTP error code.
 *
 * @ requirements 529199
 */
gptp_err_type_t GPTP_ClearStats(void)
{
    gptp_err_type_t     eErr;

    if (true == rGptpDataStruct.bGptpInitialized)
    {
        /* If the stack is initialized */
        eErr = GPTP_ERR_OK;
        GPTP_INTERNAL_ClearStats(&rGptpDataStruct);

        GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_INFO, ("gptp: gPTP stats (counters) cleared. \n"));
    }

    else
    {
        /* Stack not initialized */
        eErr = GPTP_ERR_I_NOT_INITIALIZED;
    }

    return eErr;
}

#endif /* GPTP_COUNTERS */
/*******************************************************************************
 * EOF
 ******************************************************************************/
