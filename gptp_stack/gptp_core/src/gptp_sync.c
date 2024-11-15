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
 * @file gptp_sync.c
 *
 */

#include "gptp_def.h"
#include "gptp_err.h"
#include "gptp_timer.h"
#include "gptp_frame.h"
#include "gptp_port.h"
#include "gptp.h"
#include "gptp_internal.h"
#include "gptp_sync.h"

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void GPTP_SYNC_CalculateOffset(const gptp_def_timestamp_t *rUpstreamTime,
                                      const gptp_def_timestamp_t *rGmTime,
                                      gptp_def_timestamp_sig_t *rOffset,
                                      bool *bNegative);

static void GPTP_SYNC_SyncedGmCalculate(gptp_def_sync_t *prMachine);

/*******************************************************************************
 * Local Functions
 ******************************************************************************/
/**
 * @brief The function calculates offset between local and GM clock.
 *
 * @ requirements 529199
 * @ requirements 529240
 */
static void GPTP_SYNC_CalculateOffset(const gptp_def_timestamp_t *rUpstreamTime,
                                      const gptp_def_timestamp_t *rGmTime,
                                      gptp_def_timestamp_sig_t *rOffset,
                                      bool *bNegative)
{
    int32_t             s32UpstreamNs;
    int64_t             s64UpstreamS;
    int32_t             s32GmNs;
    int64_t             s64GmS;

    int32_t             s32OffsetNs;
    int64_t             s64OffsetS;

    s32UpstreamNs = (int32_t)rUpstreamTime->u32TimeStampNs;
    s64UpstreamS = (int64_t)rUpstreamTime->u64TimeStampS;
    s32GmNs = (int32_t)rGmTime->u32TimeStampNs;
    s64GmS = (int64_t)rGmTime->u64TimeStampS;

    /* GM time overlap after the correction is added to GM time */
    if (((int32_t)GPTP_DEF_NS_IN_SECONDS) <= s32GmNs)
    {
        s32GmNs = s32GmNs - (int32_t)GPTP_DEF_NS_IN_SECONDS;
        s64GmS = s64GmS + 1;
    }

    s32OffsetNs = s32GmNs - s32UpstreamNs;

    if (s64GmS != s64UpstreamS)
    {
        s64OffsetS = s64GmS - s64UpstreamS;

        if ((s32OffsetNs < 0) && (s64OffsetS >= 0 ))
        {
            s32OffsetNs += (int32_t)GPTP_DEF_NS_IN_SECONDS;
            s64OffsetS -= 1;
        }

        else if ((s32OffsetNs >= 0) && (s64OffsetS < 0 ))
        {
            s32OffsetNs -= (int32_t)GPTP_DEF_NS_IN_SECONDS;
            s64OffsetS += 1;
        }

        else
        {
            /* Nothing to do */
        }

        if ((s32OffsetNs < 0) && (s64OffsetS < 0 ))
        {
            /* The offset is negative */
            s32OffsetNs *= -1;
            s64OffsetS *= -1;
            *bNegative = true;
        }

        else if ((s32OffsetNs < 0) && (s64OffsetS == 0 ))
        {
            /* The offset is negative */
            s32OffsetNs *= -1;
            *bNegative = true;
        }

        else
        {
            /* The offset is positive */
            *bNegative = false;
        }
    }

    else
    {
        s64OffsetS = 0;
        if (s32OffsetNs < 0)
        {
            /* The offset is negative */
            s32OffsetNs *= -1;
            *bNegative = true;
        }

        else
        {
            /* The offset is positive */
            *bNegative = false;
        }
    }
    rOffset->s64TimeStampS = s64OffsetS;
    rOffset->s32TimeStampNs = s32OffsetNs;
}

/**
 * @brief The function calculates time-stamp for synced GM.
 *
 * @ requirements 529199
 * @ requirements 529242
 */

static void GPTP_SYNC_SyncedGmCalculate(gptp_def_sync_t *prMachine)
{
    static gptp_def_timestamp_t rFreeRunClk;
    static gptp_def_timestamp_t rCorrectedClk;
    uint64_t                    u64FreeRunClk;
    uint64_t                    u64CorrClk;
    uint64_t                    u64TsTs;
    uint64_t                    u64TsDelay;
    uint64_t                    u64CorrTsTime;

    /* Set correction field to 0 sub ns */
    prMachine->u64CorrectionSubNs = 0u;
    /* Copy the correction field value into the transmit structure */
    prMachine->rTxDataFup.u64CorrectionSubNs = prMachine->u64CorrectionSubNs;

    /* As synced GM, use time from PTP clock */
    if (GPTP_ERR_OK == GPTP_PORT_GetSwitchTimes(&rFreeRunClk, &rCorrectedClk))
    {
        u64FreeRunClk = GPTP_INTERNAL_TsToU64(rFreeRunClk);
        u64CorrClk = GPTP_INTERNAL_TsToU64(rCorrectedClk);
        u64TsTs = GPTP_INTERNAL_TsToU64(prMachine->rTsTs);
        u64TsDelay = u64FreeRunClk - u64TsTs;
        u64CorrTsTime = u64CorrClk - u64TsDelay;
        prMachine->rTxDataFup.rTs.u64TimeStampS = (u64CorrTsTime / GPTP_DEF_NS_IN_SECONDS);
        prMachine->rTxDataFup.rTs.u32TimeStampNs = (uint32_t)(u64CorrTsTime % GPTP_DEF_NS_IN_SECONDS);
    }

    else
    {
        prMachine->rTxDataFup.rTs.u64TimeStampS = 0u;
        prMachine->rTxDataFup.rTs.u32TimeStampNs = 0u;
    }
}

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Sync state machine main function
 *
 * This function services Sync state machine. It receives and sends Sync messages
 * to synchronize time.
 *
 * @param[in] prGptp Pointer to global gPTP structure.
 * @param[in] u8Domain Domain ID.
 * @param[in] u8Machine Machine ID.
 * @param[in] eCallType Call type - specifying upon which event the function is called.
 *
 * @ requirements 529199
 * @ requirements 529243
 * @ requirements 120451
 * @ requirements 162144
 * @ requirements 162145
 * @ requirements 162146
 * @ requirements 120458
 * @ requirements 162148
 * @ requirements 162150
 * @ requirements 162151
 * @ requirements 162149
 * @ requirements 122552
 * @ requirements 120488
 * @ requirements 120471
 * @ requirements 162162
 */
void GPTP_SYNC_SyncMachine(gptp_def_data_t *prGptp,
                           const uint8_t u8Domain,
                           const uint8_t u8Machine,
                           const gptp_def_sm_call_type_t eCallType)
{
    static gptp_def_timestamp_sig_t    rSyncOffset;
    static gptp_def_timestamp_t        rGmTimePlusCorr;
    gptp_def_timestamp_t               rGmTimePlusCorrExpected;
    gptp_def_timestamp_t               rSinceLastValidElapsed;

    gptp_def_timestamp_sig_t           rGmTimePlusCorrExpectedSig;
    gptp_def_timestamp_sig_t           rGmTimePlusCorrSig;

    int64_t                            s64SyncDeviation;

    static bool                        bNegativeOffset;
    const gptp_def_domain_t            *prSyncGmRefDomain;
    const gptp_def_sync_t              *prSlaveMachine;

    float64_t                          f64Temp;
    uint32_t                           u32CorrectionNs;
    uint32_t                           u32CorrectionS;
    uint64_t                           u64CorrectionNs;

    gptp_def_domain_t                  *prDomain;
    gptp_def_sync_t                    *prMachine;
    gptp_def_sync_t                    *prMasterMachine;
    uint8_t                            u8SlaveId;
    float64_t                          f64MeanPropDelay;
    gptp_err_type_t                    eErr;
    uint8_t                            u8MasterMachines;

#ifdef GPTP_TIME_VALIDATION
    gptp_def_time_slave_measure_t      rSlaveValidationData;
    gptp_def_time_master_measure_t     rMasterValidationData;
#endif /* GPTP_TIME_VALIDATION */
#ifdef GPTP_USE_PRINTF
    gptp_def_timestamp_sig_t           rTempTsSig;
    bool                               bUpdateStateBeforeUpdate;
#endif  /* GPTP_USE_PRINTF */

    prDomain = &prGptp->prPerDomainParams[u8Domain];

    prMachine = &prDomain->prSyncMachines[u8Machine];

    u8SlaveId = prDomain->u8SlaveMachineId;

    prSlaveMachine = &prDomain->prSyncMachines[u8SlaveId];

    /* Distinguish among call types - event caused the function call */
    switch (eCallType)
    {
        /* Periodical function call */
        case GPTP_DEF_CALL_INITIATE:
            /* If the respective gPTP port enabled */
            if (true == *(prMachine->pbPortEnabled))
            {
                /* Switch Sync master state */
                switch (prMachine->eSyncMasterState)
                {
                    /* Sync master not enabled yet */
                    case GPTP_DEF_SYNC_MASTER_NOT_EN:
                        /* Change Sync master state to initializing */
                        prMachine->eSyncMasterState = GPTP_DEF_SYNC_MASTER_INITING;
                        /* Sync master state initializing */
                        /* fallthrough */
                    case GPTP_DEF_SYNC_MASTER_INITING:
                        /* Clear Ts timestamp registred flag */
                        prMachine->bTsTsRegistred = false;
                        /* Set Sync master state to send Sync message */
                        prMachine->eSyncMasterState = GPTP_DEF_SYNC_MASTER_SEND_SYNC;
                        /* Sync master state send Sync */
                        /* fallthrough */
                    case GPTP_DEF_SYNC_MASTER_SEND_SYNC:
                        /* Sync master state send Sync follow up */
                        /* fallthrough */
                    case GPTP_DEF_SYNC_MASTER_SEND_FUP:
                        /* If the current domain is non-GM domain */
                        if (false == prDomain->bDomainIsGm)
                        {
                            /* Use sequence Id from slave machine */
                            prMachine->u16SequenceId = prDomain->prSyncMachines[prDomain->u8SlaveMachineId].u16SequenceId;

                            if (true == prDomain->bSyncValidEverReceived)
                            {
                                /* Copy clockID and portID from received sync ptp frame */
                                prMachine->rTxDataSyn.rSrcPortID.u64ClockId = prDomain->prSyncMachines[prDomain->u8SlaveMachineId].rFupMsgRx.rHeader.u64SourceClockId;
                                prMachine->rTxDataSyn.rSrcPortID.u16PortId = 0u;
                            }
                            else
                            {
                                /* Use clockID and portID from device configuration */
                                prMachine->rTxDataSyn.rSrcPortID.u64ClockId = prGptp->prPerPortParams[prMachine->u8GptpPort].u64ClockId;
                                prMachine->rTxDataSyn.rSrcPortID.u16PortId = (uint16_t)prMachine->u8GptpPort + 1u;
                            }
                        }
                        else
                        {
                            /* Increment sequence Id */
                            prMachine->u16SequenceId++;

                            /* Use clockID and portID from device configuration */
                            prMachine->rTxDataSyn.rSrcPortID.u64ClockId = prGptp->prPerPortParams[prMachine->u8GptpPort].u64ClockId;
                            prMachine->rTxDataSyn.rSrcPortID.u16PortId = (uint16_t)prMachine->u8GptpPort + 1u;
                        }
                       
                        /* Message type - Sync */
                        prMachine->rTxDataSyn.eMsgId = GPTP_DEF_MSG_TYPE_SYNC;
                        /* Sequence ID */
                        prMachine->rTxDataSyn.u16SequenceId = prMachine->u16SequenceId;
                        /* Log message period */
                        prMachine->rTxDataSyn.s8LogMessagePeriod = prMachine->s8SyncIntervalLog;
                        /* Provide TX buffer pointer */
                        prMachine->rTxDataSyn.pau8TxBuffPtr = prMachine->au8SendBuffSync;
                        prMachine->rTxDataSyn.bTsRequested = true;
                        prMachine->rTxDataSyn.bSyncActingGm = prDomain->bSyncActingGm;
                        prMachine->rTxDataSyn.u16VlanTci = prDomain->u16VlanTci;
                        /* Set correct destination MAC address. */
                        prMachine->rTxDataSyn.u64DestMac = (uint64_t)GPTP_DEF_ETH_PTP_MAC_MULTICAST;

                        /* Transmit message, Timestamp required */
                        GPTP_MsgTransmit(&prMachine->rTxDataSyn, prMachine->u8GptpPort, u8Machine, u8Domain);

                        GPTP_TIMER_LastSyncSendRegister(prMachine);

                        /* Set new Sync master state to send Sync follow up */
                        prMachine->eSyncMasterState = GPTP_DEF_SYNC_MASTER_SEND_FUP;
                    break;

                    default:
                    break;
                }
            }
        break;

        /* The function has been called after the message receive */
        case GPTP_DEF_CALL_PTP_RECEIVED:

            /* Switch Sync slave state */
            switch (prMachine->eSyncSlaveState)
            {
                /* Sync slave not enabled yet */
                case GPTP_DEF_SYNC_SLAVE_NOT_EN:
                    /* Set new Sync slave state discard */
                    prMachine->eSyncSlaveState = GPTP_DEF_SYNC_SLAVE_DISCARD;
                    /* Sync slave discard */
                    /* fallthrough */
                    /* no break at the end of case */
                case GPTP_DEF_SYNC_SLAVE_DISCARD:
                    /* Clear Sync follow up message received flag */
                    prMachine->bFupMsgReceived = false;
                    /* Clear Sync message received flag */
                    prMachine->bSyncMsgReceived = false;
                    /* New Sync slave state - waiting for Sync message */
                    prMachine->eSyncSlaveState = GPTP_DEF_SYNC_SLAVE_WT_4_SYNC;
                    /* Sync slave waiting for Sync message */
                    /* fallthrough */
                    /* no break at the end of case */
                case GPTP_DEF_SYNC_SLAVE_WT_4_SYNC:
                    /* If Sync message received */
                    if (true == prMachine->bSyncMsgReceived)
                    {
                        /* Clear Sync message received flag */
                        prMachine->bSyncMsgReceived = false;
                        /* Save TR timestamp for the further calculation */
                        prMachine->rTrTs = prMachine->rSyncMsgRx.rSyncRxTs;
                        /* Set new Sync slave state waiting for Sync follow up */
                        prMachine->eSyncSlaveState = GPTP_DEF_SYNC_SLAVE_WT_4_FUP;
                        /* Save the sync interval of the ingress sync message */
                        prMachine->s8SyncIntervalLog = prMachine->rSyncMsgRx.rHeader.s8MessagePeriodLog;
                        /* Save the actual sequence ID */
                        prMachine->u16SequenceId = prMachine->rSyncMsgRx.rHeader.u16SequenceId;
                        /* Update the domain TCI (VLAN) with received data */
                        prDomain->u16VlanTci = prMachine->rSyncMsgRx.u16VlanTci;

                        /* Register the last valid receivd Sync in timer module */
                        GPTP_TIMER_LastValidSyncReg(prDomain);

                        /* Store Start residence time measurement value, into each master machine under the domain */
                        for (u8MasterMachines = 0u; u8MasterMachines < prDomain->u8NumberOfSyncsPerDom; u8MasterMachines++)
                        {
                            prMasterMachine = &prDomain->prSyncMachines[u8MasterMachines];
                            if (GPTP_DEF_PTP_MASTER == prMasterMachine->bSyncRole)
                            {
                                if (true == *(prMasterMachine->pbPortEnabled))
                                {
                                    /* Get current time */
                                    prMasterMachine->u64ResidenceMeasStartPtpNs = GPTP_INTERNAL_TsToU64(GPTP_PORT_CurrentTimeGet(GPTP_DEF_TS_FREERUNNING));
                                }
                            }
                        }
                    }
                break;

                /* Waiting for Sync follow up */
                case GPTP_DEF_SYNC_SLAVE_WT_4_FUP:
                    /* If Sync follow up message has been received */
                    if (true == prMachine->bFupMsgReceived)
                    {
                        if (prMachine->rFupMsgRx.rHeader.u16SequenceId == prMachine->u16SequenceId)
                        {
                            /* Clear Sync follow up message received flag */
                            prMachine->bFupMsgReceived = false;
                            /* Save received rate ratio for the further calculation */
                            prMachine->f64RateRatio = prMachine->rFupMsgRx.f64RateRatio;
                            /* Calculate own rate ratio */
                            prMachine->f64RateRatio += ((*prMachine->pf64NeighborRateRatio) - 1.0);
                            /* Save Correction field */
                            prMachine->u64CorrectionSubNs = prMachine->rFupMsgRx.rHeader.u64CorrectionSubNs;
                            /* Confirm that valid sync message has been received */
                            prDomain->bSyncValidEverReceived = true;

                            /* If dNeighborRateRation and f64RateRatio is not equal to 0.0 */
                            if (((0.0 > (*prMachine->pf64NeighborRateRatio)) || (0.0 < (*prMachine->pf64NeighborRateRatio))) &&
                                ((0.0 > prMachine->f64RateRatio) || (0.0 < prMachine->f64RateRatio)))
                            {
                                /* Calculate mean propagation delay */
                                /* Delay asymetry is 0 */
                                f64MeanPropDelay = ((*prMachine->pf64NeighborPropDelay + ((float64_t)*prMachine->ps16DelayAsymmetry)) / (*prMachine->pf64NeighborRateRatio)) + \
                                                   (0.0 / prMachine->f64RateRatio);
                                /* Convert mean propagation delay into uint32 type */
                                prMachine->u32MeanPropDelay = (uint32_t)f64MeanPropDelay;
                            }

                            /* If dNeighborRateRation or f64RateRatio is equal to 0.0 */
                            else
                            {
                                /* Do not change u32MeanPropDelay and use value from previous calculation */
                                /* Register error -  can't divide by 0 */
                                GPTP_ERR_Register(u8Machine, u8Domain, GPTP_ERR_A_DIV_BY_0_ATTEMPT, prMachine->u16SequenceId);
                            }

                            /* Calculate upstream Tx time */
                            prMachine->rUpstreamTxTime = GPTP_INTERNAL_TsMinU32(prMachine->rTrTs, prMachine->u32MeanPropDelay);

                            if (false == prDomain->bDomainIsGm)
                            {
                                if (GPTP_DEF_PTP_SLAVE == prMachine->bSyncRole)
                                {
                                    /* Update Operation Sync interval */
                                    prMachine->s8SyncIntervalLog = prMachine->rSyncMsgRx.rHeader.s8MessagePeriodLog;

                                    /* Calculate correction field value in s and ns */
                                    u32CorrectionNs = (uint32_t)((prMachine->u64CorrectionSubNs >> 16u) % (uint64_t)GPTP_DEF_NS_IN_SECONDS);
                                    u32CorrectionS = (uint32_t)((prMachine->u64CorrectionSubNs >> 16u) / (uint64_t)GPTP_DEF_NS_IN_SECONDS);
                                    u64CorrectionNs = (prMachine->u64CorrectionSubNs >> 16u);

                                    rGmTimePlusCorr = prMachine->rFupMsgRx.rSyncTxTs;

                                    /* Add correction field value to the Upstream time */
                                    rGmTimePlusCorr.u32TimeStampNs = rGmTimePlusCorr.u32TimeStampNs + u32CorrectionNs;
                                    rGmTimePlusCorr.u64TimeStampS = rGmTimePlusCorr.u64TimeStampS + (uint64_t)u32CorrectionS;
                                    if (((uint32_t)GPTP_DEF_NS_IN_SECONDS) <= rGmTimePlusCorr.u32TimeStampNs)
                                    {
                                        rGmTimePlusCorr.u32TimeStampNs = rGmTimePlusCorr.u32TimeStampNs - (uint32_t)GPTP_DEF_NS_IN_SECONDS;
                                        rGmTimePlusCorr.u64TimeStampS = rGmTimePlusCorr.u64TimeStampS + 1u;
                                    }

                                    /* Store correctly received tmestamp */
                                    prMachine->rPrecOrigTsRcvd.u64TimeStampS = prMachine->rFupMsgRx.rSyncTxTs.u64TimeStampS;
                                    prMachine->rPrecOrigTsRcvd.u32TimeStampNs = prMachine->rFupMsgRx.rSyncTxTs.u32TimeStampNs;

                                    /* If the precision origin timestamp (time of Grand Master) doesn't advance */
                                    if ((prDomain->rPrevPrecOrigTs.u32TimeStampNs == prMachine->rFupMsgRx.rSyncTxTs.u32TimeStampNs) && \
                                        (prDomain->rPrevPrecOrigTs.u64TimeStampS == prMachine->rFupMsgRx.rSyncTxTs.u64TimeStampS))
                                    {
                                        /* if precision origin timestamp advancing is stopped for more than 2 maximum sync periods */
                                        if (u64CorrectionNs > (GPTP_INTERNAL_Log2Ns(GPTP_DEF_LOG_SYNC_INT_MAX) * 2u))
                                        {
                                            if (false == prDomain->bGmFailureReported)
                                            {
                                                /* Report GM Failure */
                                                GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, prDomain->u8DomainNumber, GPTP_ERR_O_DOMAIN_GM_FAILURE, GPTP_ERR_SEQ_ID_NOT_SPECIF);
                                                prDomain->bGmFailureReported = true;
                                            }
                                        }
                                        else
                                        {
                                            prDomain->bGmFailureReported = false;
                                        }
                                    }

                                    prDomain->rPrevPrecOrigTs = prMachine->rFupMsgRx.rSyncTxTs;

                                    /* Outlier check */
                                    /* Expected time */
                                    rSinceLastValidElapsed = GPTP_INTERNAL_TsMinTs(prMachine->rTrTs,prDomain->rLastValidTsOfGmPlusCor);

                                    rGmTimePlusCorrExpected.u32TimeStampNs = prDomain->rLastValidGmPlusCor.u32TimeStampNs + rSinceLastValidElapsed.u32TimeStampNs;
                                    rGmTimePlusCorrExpected.u64TimeStampS = prDomain->rLastValidGmPlusCor.u64TimeStampS + rSinceLastValidElapsed.u64TimeStampS;
                                    if (((uint32_t)GPTP_DEF_NS_IN_SECONDS) <= rGmTimePlusCorrExpected.u32TimeStampNs)
                                    {
                                        rGmTimePlusCorrExpected.u32TimeStampNs = rGmTimePlusCorrExpected.u32TimeStampNs - (uint32_t)GPTP_DEF_NS_IN_SECONDS;
                                        rGmTimePlusCorrExpected.u64TimeStampS = rGmTimePlusCorrExpected.u64TimeStampS + 1u;
                                    }

                                    rGmTimePlusCorrExpectedSig.s64TimeStampS = (int64_t)rGmTimePlusCorrExpected.u64TimeStampS;
                                    rGmTimePlusCorrExpectedSig.s32TimeStampNs = (int32_t)rGmTimePlusCorrExpected.u32TimeStampNs;
                                    rGmTimePlusCorrSig.s64TimeStampS = (int64_t)rGmTimePlusCorr.u64TimeStampS;
                                    rGmTimePlusCorrSig.s32TimeStampNs = (int32_t)rGmTimePlusCorr.u32TimeStampNs;

                                    s64SyncDeviation = GPTP_INTERNAL_TsToS64(GPTP_INTERNAL_TsMinTsSig(rGmTimePlusCorrExpectedSig,rGmTimePlusCorrSig));
                                    if (s64SyncDeviation < 0)
                                    {
                                        s64SyncDeviation = s64SyncDeviation * (-1);
                                    }

                                    if ((((uint64_t)s64SyncDeviation < prDomain->u32SyncOutlierThrNs) || (prDomain->u8OutlierIgnoreCnt <= prDomain->u8OutlierCnt)) || \
                                        (false == prDomain->bRcvdSyncUsedForLocalClk))
                                    {
#ifdef GPTP_USE_PRINTF
                                        if (prDomain->u8OutlierIgnoreCnt <= prDomain->u8OutlierCnt)
                                        {
                                            GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_INFO, ("gptp: Current GM time determined as:"));
                                            GPTP_PRINTF_TIME_UNSIG(GPTP_DEBUG_MSGTYPE_INFO, rGmTimePlusCorr);
                                        }
                                        bUpdateStateBeforeUpdate = prDomain->bRcvdSyncUsedForLocalClk;
#endif  /* GPTP_USE_PRINTF */
                                        /* Calculate offset between upstream Tx time and grandmaster time */
                                        GPTP_SYNC_CalculateOffset(&prMachine->rUpstreamTxTime, &rGmTimePlusCorr, &rSyncOffset, &bNegativeOffset);

                                        /* Update synchronization lock. */
                                        GPTP_INTERNAL_UpdateSyncLock(prGptp, rSyncOffset);

                                        /* Notify the switch with the offset and rate ratio */
                                        eErr = GPTP_PORT_UpdateLocalClock(prGptp, prDomain->u8DomainNumber, &(prMachine->f64RateRatio), rSyncOffset, \
                                                                          bNegativeOffset, prMachine->s8SyncIntervalLog, &(prDomain->bRcvdSyncUsedForLocalClk));

                                        if (GPTP_ERR_OK != eErr)
                                        {
                                            GPTP_ERR_Register(u8Machine, u8Domain, eErr, prMachine->u16SequenceId);
                                        }
#ifdef GPTP_USE_PRINTF
                                        if ((false == bUpdateStateBeforeUpdate) && (true == prDomain->bRcvdSyncUsedForLocalClk))
                                        {
                                            GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_SYNC, ("gptp: Clock synchronized for domain %u, time:", prDomain->u8DomainNumber));
                                            GPTP_PRINTF_TIME_UNSIG(GPTP_DEBUG_MSGTYPE_SYNC, rGmTimePlusCorr);
                                        }
#endif  /* GPTP_USE_PRINTF */
                                        prGptp->rPerDeviceParams.rLastFreeRunRef = prMachine->rTrTs;
                                        prDomain->rLastValidGmPlusCor.u64TimeStampS = rGmTimePlusCorr.u64TimeStampS;
                                        prDomain->rLastValidGmPlusCor.u32TimeStampNs = rGmTimePlusCorr.u32TimeStampNs;
                                        prDomain->rLastValidTsOfGmPlusCor.u64TimeStampS = prMachine->rTrTs.u64TimeStampS;
                                        prDomain->rLastValidTsOfGmPlusCor.u32TimeStampNs = prMachine->rTrTs.u32TimeStampNs;
                                        prDomain->u8OutlierCnt = 0;
                                    }

                                    else
                                    {
                                        prDomain->u8OutlierCnt++;

                                        /* Update local clock in case it has already been updated (last setpoint exists) */
                                        if (true == prGptp->rPerDeviceParams.bEverUpdated) {
                                            /* Notify the switch with the zero offset and rate ratio - to stabilize clock updating */
                                            eErr = GPTP_PORT_FixLocalClock(&prMachine->f64RateRatio);
                                            if (GPTP_ERR_OK != eErr)
                                            {
                                                GPTP_ERR_Register(u8Machine, u8Domain, eErr, prMachine->u16SequenceId);
                                            }
                                        }
#ifdef GPTP_USE_PRINTF
                                        rTempTsSig.s32TimeStampNs = (int32_t)(s64SyncDeviation % (int64_t)GPTP_DEF_NS_IN_SECONDS);
                                        rTempTsSig.s64TimeStampS = s64SyncDeviation / (int64_t)GPTP_DEF_NS_IN_SECONDS;
                                        GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_INFO, ("gptp: Sync Outlier value received, Domain Number: %u. The offset is", prDomain->u8DomainNumber));
                                        GPTP_PRINTF_TIME_SIG(GPTP_DEBUG_MSGTYPE_INFO, rTempTsSig);
#endif  /* GPTP_USE_PRINTF */
                                    }

#ifdef GPTP_TIME_VALIDATION
                                    if (NULL != prGptp->rPtpStackCallBacks.pfTimeBaseProvideNotifSlave)
                                    {
                                        rSlaveValidationData.u16SequenceId = prMachine->rSyncMsgRx.rHeader.u16SequenceId;
                                        rSlaveValidationData.rSourcePortId.u64ClockId = prMachine->rSyncMsgRx.rHeader.u64SourceClockId;
                                        rSlaveValidationData.rSourcePortId.u16PortId = (prMachine->rSyncMsgRx.rHeader.u16SourcePortId + 1u);
                                        rSlaveValidationData.rSyncIngressTimestamp.u64TimeStampS = prMachine->rSyncMsgRx.rSyncRxTs.u64TimeStampS;
                                        rSlaveValidationData.rSyncIngressTimestamp.u32TimeStampNs = prMachine->rSyncMsgRx.rSyncRxTs.u32TimeStampNs;
                                        rSlaveValidationData.rPreciseOriginTimestamp.u64TimeStampS = prMachine->rFupMsgRx.rSyncTxTs.u64TimeStampS;
                                        rSlaveValidationData.rPreciseOriginTimestamp.u32TimeStampNs = prMachine->rFupMsgRx.rSyncTxTs.u32TimeStampNs;
                                        rSlaveValidationData.s64CorrectionField = (int64_t)(prMachine->rFupMsgRx.rHeader.u64CorrectionSubNs);
                                        rSlaveValidationData.u32Pdelay = prMachine->u32MeanPropDelay;
                                        eErr = GPTP_PORT_GetSwitchTimes(&rSlaveValidationData.rReferenceLocalTimestamp, &rSlaveValidationData.rReferenceGlobalTimestamp);
                                        if (GPTP_ERR_OK == eErr)
                                        {
                                            prGptp->rPtpStackCallBacks.pfTimeBaseProvideNotifSlave(prGptp->prPerDomainParams[u8Domain].u8DomainNumber,rSlaveValidationData);
                                        }
                                    }
                                    else
                                    {
                                        GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_F_NULL_PTR, GPTP_ERR_SEQ_ID_NOT_SPECIF);
                                    }
#endif /* GPTP_TIME_VALIDATION */
                                }
                            }

                            /* Set new Sync slave state - waiting for Sync message */
                            prMachine->eSyncSlaveState = GPTP_DEF_SYNC_SLAVE_WT_4_SYNC;
                        }

                        /* If sequence id of Sync and Sync FUP does not match */
                        else
                        {
                            /* Set new Sync slave state - Sync discard */
                            prMachine->eSyncSlaveState = GPTP_DEF_SYNC_SLAVE_DISCARD;
                            
                            GPTP_INTERNAL_LosHandle(prGptp, prDomain, u8Machine, prMachine->u16SequenceId);
#ifdef GPTP_COUNTERS
                            GPTP_INTERNAL_IncrementPortStats(prGptp, prMachine->u8GptpPort, ieee8021AsPortStatRxPTPPacketDiscard);
                            GPTP_INTERNAL_IncrementDomainStats(prGptp, u8Domain, u8Machine, ieee8021AsPortStatRxPTPPacketDiscard);
#endif /* GPTP_COUNTERS */
                        }
                    }

                    /* If the next sync is received instead of the sync FUP */
                    else if (true == prMachine->bSyncMsgReceived)
                    {
                        /* Clear Sync message received flag */
                        prMachine->bSyncMsgReceived = false;
                        /* Save TR timestamp for the further calculation */
                        prMachine->rTrTs = prMachine->rSyncMsgRx.rSyncRxTs;
                        /* Set new Sync slave state waiting for Sync follow up */
                        prMachine->eSyncSlaveState = GPTP_DEF_SYNC_SLAVE_WT_4_FUP;
                        /* Save the sync interval of the ingress sync message */
                        prMachine->s8SyncIntervalLog = prMachine->rSyncMsgRx.rHeader.s8MessagePeriodLog;
                        /* Handle Loss of sync error */
                        GPTP_INTERNAL_LosHandle(prGptp, prDomain, u8Machine, prMachine->u16SequenceId);
                        /* Save the actual sequence ID */
                        prMachine->u16SequenceId = prMachine->rSyncMsgRx.rHeader.u16SequenceId;

#ifdef GPTP_COUNTERS
                        GPTP_INTERNAL_IncrementPortStats(prGptp, prMachine->u8GptpPort, ieee8021AsPortStatRxPTPPacketDiscard);
                        GPTP_INTERNAL_IncrementDomainStats(prGptp, u8Domain, u8Machine, ieee8021AsPortStatRxPTPPacketDiscard);
#endif /* GPTP_COUNTERS */
                    }

                    else
                    {
                        /* Nothing to do */
                    }
                break;

                default:
                break;
            }
        break;

        /* The function has been called upon the TX timestamp generation */
        case GPTP_DEF_CALL_ON_TIMESTAMP:
            /* If the Sync master is in the send Sync follow up state, Ts timestamp has been registred and sequence Id of Sync message matches with sequence id from the frame Id */
            if (((GPTP_DEF_SYNC_MASTER_SEND_FUP == prMachine->eSyncMasterState) && (true == prMachine->bTsTsRegistred)) &&  \
                 (prMachine->u16SequenceId == prMachine->u16TsTsSeqId))
            {
                /* Clear Ts timestamp registred flag */
                prMachine->bTsTsRegistred = false;

                /* Copy sequence Id information into the Sync follow up message content */
                prMachine->rTxDataFup.u16SequenceId = prMachine->u16SequenceId;

                /* Copy clockID and portID into the Sync follow up message content */
                prMachine->rTxDataFup.rSrcPortID.u64ClockId = prMachine->rTxDataSyn.rSrcPortID.u64ClockId;
                prMachine->rTxDataFup.rSrcPortID.u16PortId = prMachine->rTxDataSyn.rSrcPortID.u16PortId;

                /* If domain is not synced GM */
                if (false == prDomain->bSyncedGm)
                {
                    /* If the domain is NOT GM */
                    if (false == prDomain->bDomainIsGm)
                    {
                        /* Copy the Rate Ratio from the slave sync machine to the current one */
                        prMachine->f64RateRatio = prSlaveMachine->f64RateRatio;
                    }
                    /* If the domain is GM */
                    else
                    {
                        prMachine->f64RateRatio = 1.0;
                    }
                }

                /* If the domain is synced GM */
                else
                {
                    if (prDomain->u8RefDomForSyncedGIndex == prDomain->u8DomainIndex)
                    {
                        prMachine->f64RateRatio = 1.0;
                    }

                    else
                    {
                        /* Copy rate ratio from the slave sync machine - across domains, from the reference domain */
                        prSyncGmRefDomain = &prGptp->prPerDomainParams[prDomain->u8RefDomForSyncedGIndex];
                        prMachine->f64RateRatio = prSyncGmRefDomain->prSyncMachines[prSyncGmRefDomain->u8SlaveMachineId].f64RateRatio;
                    }
                }

                /* Copy the calculated rate ratio into the Sync follow up message content */
                prMachine->rTxDataFup.f64RateRatio = prMachine->f64RateRatio;
                /* Provide TX buffer pointer */
                prMachine->rTxDataFup.pau8TxBuffPtr = prMachine->au8SendBuffFup;
                /* Time stamp is not requested */
                prMachine->rTxDataFup.bTsRequested = false;

                /* If the domain is NOT GM */
                if (false == prDomain->bDomainIsGm)
                {
                    /* The Sync HAS NOT been sent as acting GM */
                    if (false == prMachine->bSyncActingGm)
                    {
                        /* Partial correction field calculation */
                        f64Temp = (float64_t)GPTP_INTERNAL_TsToU64(GPTP_INTERNAL_TsMinTs(prMachine->rTsTs, prSlaveMachine->rUpstreamTxTime));
                        f64Temp *= prMachine->f64RateRatio;
                        /* Correction field calculation */
                        prMachine->u64CorrectionSubNs = prSlaveMachine->u64CorrectionSubNs + (((uint64_t)f64Temp) << 16u);
                        /* In case of Bridge functionality */
                        /* As non GM, use received timestamp from the slave port */
                        prMachine->rTxDataFup.rTs.u64TimeStampS = prSlaveMachine->rPrecOrigTsRcvd.u64TimeStampS;
                        prMachine->rTxDataFup.rTs.u32TimeStampNs = prSlaveMachine->rPrecOrigTsRcvd.u32TimeStampNs;
                        /* Use calculated correction field for the downstream port */
                        prMachine->rTxDataFup.u64CorrectionSubNs = prMachine->u64CorrectionSubNs;
                    }

                    /* The Sync HAS been sent as acting GM */
                    else
                    {
                        /* The AED-B is advancing correction field when acting GM */
                        prMachine->u64CorrectionSubNs = GPTP_INTERNAL_TsToU64(prMachine->rTsTs) << 16u ;
                        /* Use calculated correction field for the downstream port */
                        prMachine->rTxDataFup.u64CorrectionSubNs = prMachine->u64CorrectionSubNs;
                        /* Precise origin timestamp is 0 when the AED-B is acting GM */
                        prMachine->rTxDataFup.rTs.u64TimeStampS = 0u;
                        prMachine->rTxDataFup.rTs.u32TimeStampNs = 0u;
                    }
                }

                /* If the domain is GM */
                else
                {
                    /* If domain is not synced GM */
                    if (false == prDomain->bSyncedGm)
                    {
                        /* Set correction field to 0 sub ns */
                        prMachine->u64CorrectionSubNs = 0u;
                        /* Copy the correction field value into the transmit structure */
                        prMachine->rTxDataFup.u64CorrectionSubNs = prMachine->u64CorrectionSubNs;
                        /* As GM, use Ts timestamp */
                        prMachine->rTxDataFup.rTs.u64TimeStampS = prMachine->rTsTs.u64TimeStampS;
                        prMachine->rTxDataFup.rTs.u32TimeStampNs = prMachine->rTsTs.u32TimeStampNs;
                    }

                    /* If domain is synced GM */
                    else
                    {
                        GPTP_SYNC_SyncedGmCalculate(prMachine);
                    }
                }

                /* If the domain is Grand Master */
                if (true == prDomain->bDomainIsGm)
                {
                    GPTP_PORT_TimeBaseInfoGet(&(prDomain->u16GmTimeBaseIndicator), &(prDomain->u32LastGmPhaseChangeH), \
                                              &(prDomain->u64LastGmPhaseChangeL), &(prDomain->u32ScaledLastGmFreqChange));
                }

                /* Set message type Sync follow up */
                prMachine->rTxDataFup.eMsgId = GPTP_DEF_MSG_TYPE_FOLLOW_UP;
                prMachine->rTxDataFup.s8LogMessagePeriod = prMachine->s8SyncIntervalLog;
                prMachine->rTxDataFup.u16VlanTci = prDomain->u16VlanTci;

                /* Set correct destination MAC address. */
                prMachine->rTxDataFup.u64DestMac = (uint64_t)GPTP_DEF_ETH_PTP_MAC_MULTICAST;

#ifdef GPTP_TIME_VALIDATION
                if (NULL != prGptp->rPtpStackCallBacks.pfTimeBaseProvideNotifMaster)
                {
                    rMasterValidationData.u16SequenceId = prMachine->u16SequenceId;
                    rMasterValidationData.rSourcePortId.u64ClockId = prGptp->prPerPortParams[prMachine->u8GptpPort].u64ClockId;
                    rMasterValidationData.rSourcePortId.u16PortId = ((uint16_t)(prMachine->u8GptpPort) + 1u);
                    rMasterValidationData.rSyncEgressTimestamp.u64TimeStampS = prMachine->rTsTs.u64TimeStampS;
                    rMasterValidationData.rSyncEgressTimestamp.u32TimeStampNs = prMachine->rTsTs.u32TimeStampNs;
                    rMasterValidationData.rPreciseOriginTimestamp.u64TimeStampS = prMachine->rTxDataFup.rTs.u64TimeStampS;
                    rMasterValidationData.rPreciseOriginTimestamp.u32TimeStampNs = prMachine->rTxDataFup.rTs.u32TimeStampNs;
                    rMasterValidationData.s64CorrectionField = (int64_t)(prMachine->u64CorrectionSubNs);

                    prGptp->rPtpStackCallBacks.pfTimeBaseProvideNotifMaster(prGptp->prPerDomainParams[u8Domain].u8DomainNumber, rMasterValidationData);
                }
                else
                {
                    GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_F_NULL_PTR, GPTP_ERR_SEQ_ID_NOT_SPECIF);
                }
#endif /* GPTP_TIME_VALIDATION */

                /* Transmit message, Timestamp not required */
                GPTP_MsgTransmit(&prMachine->rTxDataFup, prMachine->u8GptpPort, u8Machine, u8Domain);

                /* Set next Sync master state send Sync message */
                prMachine->eSyncMasterState = GPTP_DEF_SYNC_MASTER_SEND_SYNC;

                /* non GM (bridge) residence time measurement */
                if (false == prDomain->bDomainIsGm)
                {
                    if (0u != prMachine->u64ResidenceMeasStartPtpNs)
                    {
                        if (GPTP_DEF_10_MS_IN_NS < ((GPTP_INTERNAL_TsToU64(GPTP_PORT_CurrentTimeGet(GPTP_DEF_TS_FREERUNNING))) - prMachine->u64ResidenceMeasStartPtpNs))
                        {
                            /* Register error - residence time too long */
                            GPTP_ERR_Register(u8Machine, u8Domain, GPTP_ERR_L_TOO_LONG_RESIDENCE, prMachine->u16SequenceId);
                        }

                        /* Clear residence measurement start - not calculate again, in periodic function */
                        prMachine->u64ResidenceMeasStartPtpNs = 0u;
                    }
                }
            }
            else
            {
                /* Time stamp not registered */
                if (true != prMachine->bTsTsRegistred)
                {
                    GPTP_ERR_Register(u8Machine, u8Domain, GPTP_ERR_T_TS_NOT_REGISTERED, prMachine->u16SequenceId);
                }
                /* Error, wrong machine state */
                if (GPTP_DEF_SYNC_MASTER_SEND_FUP != prMachine->eSyncMasterState)
                {
                    GPTP_ERR_Register(u8Machine, u8Domain, GPTP_ERR_S_MACHINE_STATE, prMachine->u16SequenceId);
                }
                /* Error, sequence id of timestamp doesn't match */
                if (prMachine->u16SequenceId != prMachine->u16TsTsSeqId)
                {
                    GPTP_ERR_Register(u8Machine, u8Domain, GPTP_ERR_S_INCORRECT_TS, prMachine->u16SequenceId);
                }
            }
        break;

        default:
        break;
    }
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
