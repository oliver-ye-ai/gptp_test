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
 * @file gptp_signaling.c
 *
 */

#include "gptp_def.h"
#include "gptp_err.h"
#include "gptp_timer.h"
#include "gptp_frame.h"
#include "gptp_port.h"
#include "gptp.h"
#include "gptp_internal.h"
#include "gptp_signaling.h"

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Receive signaling message
 *
 * This function processes received Signaling message.
 *
 * @param[in] prGptp Pointer to global gPTP structure.
 * @param[in] u8Domain Domain ID.
 * @param[in] u8Machine Machine ID.
 *
 * @ requirements 529199
 * @ requirements 529238
 * @ requirements 152454
 * @ requirements 152452
 */
void GPTP_SIGNALING_RxSignaling(const gptp_def_data_t *prGptp,
                                const uint8_t u8Domain,
                                const uint8_t u8Machine)
{
    gptp_def_domain_t           *prDomain;
    gptp_def_sync_t             *rSync;

    prDomain = &prGptp->prPerDomainParams[u8Domain];
    rSync = &prDomain->prSyncMachines[u8Machine];

    if (true == prDomain->bSignalingIntervalRcvd)
    {
        /* If the port is master, set Sync message period according to the Signaling TLV */
        if (GPTP_DEF_PTP_MASTER == rSync->bSyncRole)
        {
            if (GPTP_DEF_SIG_TLV_STOP_SENDING == prDomain->s8SyncIntervalLogRcvd)
            {
                /* Disable Sync timer for the specific port */
                rSync->bTimerSyncEnabled = false;

                GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_STATE, ("gptp: Signaling Message for Domain %u, Machine %u received. Stop sending Sync and Follow Up messages. \n", \
                            prDomain->u8DomainNumber, u8Machine));
            }

            else if (GPTP_DEF_SIG_TLV_SET_TO_INIT == prDomain->s8SyncIntervalLogRcvd)
            {
                /* Set period to initial */
                rSync->s8SyncIntervalLog = rSync->s8InitSyncIntervalLog;
                /* Enable Sync timer for the specific port */
                rSync->bTimerSyncEnabled = true;

                GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_STATE, ("gptp: Signaling Message for Domain %u, Machine %u received. Sync interval changed to initial value (%d [log]). \n", \
                            prDomain->u8DomainNumber, u8Machine, rSync->s8InitSyncIntervalLog));
            }

            else if (GPTP_DEF_SIG_TLV_NOT_CHANGE == prDomain->s8SyncIntervalLogRcvd)
            {
                /* Do not change period */
            }

            else
            {
                if ((prDomain->s8SyncIntervalLogRcvd >= GPTP_DEF_LOG_SYNC_INT_MIN) && \
                   (prDomain->s8SyncIntervalLogRcvd <= GPTP_DEF_LOG_SYNC_INT_MAX))
                {
                    /* Enable Sync timer for the specific port */
                    rSync->bTimerSyncEnabled = true;
                    /* Set period as signaling message requires */
                    rSync->s8SyncIntervalLog = prDomain->s8SyncIntervalLogRcvd;

                    GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_STATE, ("gptp: Signaling Message for Domain %u, Machine %u received. Sync interval changed value to %d [log]. \n", \
                                prDomain->u8DomainNumber, u8Machine, prDomain->s8SyncIntervalLogRcvd));
                }

                else
                {
                    /* Interval outside of limits */
                    GPTP_ERR_Register(u8Machine, u8Domain, GPTP_ERR_I_SYNC_INTERVAL_OOR, GPTP_ERR_SEQ_ID_NOT_SPECIF);
                }
            }
        }
        /* If the port is slave, ignore signaling message - don't change pdelay interval */
    }

    /* Clear signaling message received flag */
    prDomain->bSignalingIntervalRcvd = false;
}

/*!
 * @brief Send signaling message
 *
 * This function Sends Signaling message.
 *
 * @param[in] prGptp Pointer to global gPTP structure.
 * @param[in] u8Domain Domain ID.
 * @param[in] s8Interval Required Sync interval.
 *
 * @ requirements 529199
 * @ requirements 529239
 * @ requirements 162158
 * @ requirements 162161
 * @ requirements 162160
 */
void GPTP_SIGNALING_SendSignaling(const gptp_def_data_t *prGptp,
                                  const uint8_t u8Domain,
                                  const int8_t s8Interval)
{
    gptp_def_domain_t           *prDomain;

    prDomain = &prGptp->prPerDomainParams[u8Domain];

    /* Increment the sequence ID */
    prDomain->u16SigSequenceId++;
    /* Set sequence Id */
    prDomain->rTxDataSig.u16SequenceId = prDomain->u16SigSequenceId;
    /* Message Id - signaling */
    prDomain->rTxDataSig.eMsgId = GPTP_DEF_MSG_TYPE_SIGNALING;
    prDomain->rTxDataSig.s8LogMessagePeriod = s8Interval;

    prDomain->rTxDataSig.pau8TxBuffPtr = prDomain->au8SendBuffSig;
    prDomain->rTxDataSig.bTsRequested = false;
    prDomain->rTxDataSig.u16VlanTci = prDomain->u16VlanTci;

    /* Use clockID and portID from device configuration */
    prDomain->rTxDataSig.rSrcPortID.u64ClockId = prGptp->prPerPortParams[prDomain->prSyncMachines[prDomain->u8SlaveMachineId].u8GptpPort].u64ClockId;
    prDomain->rTxDataSig.rSrcPortID.u16PortId = (uint16_t)prDomain->prSyncMachines[prDomain->u8SlaveMachineId].u8GptpPort + 1u;

    /* Set correct destination MAC address. */
    prDomain->rTxDataSig.u64DestMac = (uint64_t)GPTP_DEF_ETH_PTP_MAC_MULTICAST;

    /* Transmit message, Timestamp not required */
    GPTP_MsgTransmit(&prDomain->rTxDataSig,\
                     prDomain->prSyncMachines[prDomain->u8SlaveMachineId].u8GptpPort,\
                     prDomain->u8SlaveMachineId, u8Domain);
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
