/*
 * Copyright 2019-2023 NXP
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
 * @file gptp_timer.c
 *
 */

#include "gptp_def.h"
#include "gptp_err.h"
#include "gptp_timer.h"
#include "gptp_sync.h"
#include "gptp_pdelay.h"
#include "gptp_signaling.h"
#include "gptp_port.h"
#include "gptp_internal.h"

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief The function registers last received sync message.
 *
 * @param[in] Pointer to general gPTP data structure.
 * @param[in] Pointer to the respective domain data structure.
 *
 * @ requirements 529199
 * @ requirements 529244
 */
void GPTP_TIMER_LastValidSyncReg(gptp_def_domain_t *prDomain)
{
    gptp_def_timestamp_t        rCurrentTimeTs;
    uint64_t                    u64CurrentTimeNs;

    rCurrentTimeTs = GPTP_PORT_CurrentTimeGet(GPTP_DEF_TS_FREERUNNING);
    u64CurrentTimeNs = GPTP_INTERNAL_TsToU64(rCurrentTimeTs);

    /* Last valid Sync message received at TS time*/
    prDomain->u64TimerLastSyncRcvd = u64CurrentTimeNs;

    prDomain->u64TimerSyncReceiptTimeoutDue = u64CurrentTimeNs + (GPTP_INTERNAL_Log2Ns(prDomain->prSyncMachines[prDomain->u8SlaveMachineId].s8SyncIntervalLog) * (uint64_t)prDomain->u8SyncReceiptTimeoutCnt);

    /* Consider sync message valid */
    prDomain->bValidSyncReceived = true;
}

/*!
 * @brief The function registers last sync sent.
 *
 * @param[in] Pointer to the respective sync data structure.
 *
 * @ requirements 529199
 * @ requirements 529245
 */
void GPTP_TIMER_LastSyncSendRegister(gptp_def_sync_t *prSync)
{
    gptp_def_timestamp_t        rCurrentTimeTs;
    uint64_t                    u64CurrentTimeNs;

    rCurrentTimeTs = GPTP_PORT_CurrentTimeGet(GPTP_DEF_TS_FREERUNNING);
    u64CurrentTimeNs = GPTP_INTERNAL_TsToU64(rCurrentTimeTs);

    prSync->u64TimerLastSyncSent = u64CurrentTimeNs;
}

/*!
 * @brief The function updates nex Pdelay request time.
 *
 * @param[in] Pointer to the respective Pdelay data structure.
 *
 * @ requirements 529199
 * @ requirements 529246
 */
void GPTP_TIMER_PdelayDueUpdate(gptp_def_pdelay_t *prPdelay)
{
    gptp_def_timestamp_t        rCurrentTimeTs;
    uint64_t                    u64CurrentTimeNs;

    rCurrentTimeTs = GPTP_PORT_CurrentTimeGet(GPTP_DEF_TS_FREERUNNING);
    u64CurrentTimeNs = GPTP_INTERNAL_TsToU64(rCurrentTimeTs);

    /* Update Pdelay request due */
    prPdelay->u64TimerPdelaySendDue = u64CurrentTimeNs + GPTP_INTERNAL_Log2Ns( prPdelay->s8PdelayIntervalLog);
}

/*!
 * @brief The function schedules syncs for non GM domain.
 *
 * @param[in] Pointer to general gPTP data structure.
 * @param[in] Pointer to the respective domain structure.
 * @param[in] Call type
 *
 * @ requirements 529199
 * @ requirements 529247
 */
void GPTP_TIMER_SyncsSendNonGm(gptp_def_data_t *prGptp,
                               gptp_def_domain_t *prDomain,
                               const gptp_def_sm_call_type_t rCallType)
{
    gptp_def_sync_t             *prSync;
    uint8_t                     u8Machine;
    int8_t                      s8SlaveIntLog;
    int8_t                      s8IntervLogDiff;
    uint8_t                     u8Gap;
    uint64_t                    u64MasterIntervalNs;
    gptp_def_timestamp_t        rCurrentTimeTs;
    uint64_t                    u64CurrentTimeNs;

#ifdef GPTP_USE_PRINTF
    static bool                 bActingGmReported = false;
#endif /* GPTP_USE_PRINTF */

    rCurrentTimeTs = GPTP_PORT_CurrentTimeGet(GPTP_DEF_TS_FREERUNNING);
    u64CurrentTimeNs = GPTP_INTERNAL_TsToU64(rCurrentTimeTs);

    /* Function call type - from periodic function and True acting GM - sync never received*/
    if ((GPTP_DEF_CALL_INITIATE == rCallType) && (false == prDomain->bValidSyncReceived))
    {
        /* For all sync machines */
        for (u8Machine = 0u; u8Machine < prDomain->u8NumberOfSyncsPerDom; u8Machine++)
        {
            prSync = &prDomain->prSyncMachines[u8Machine];

            /* Except of the slave */
            if (u8Machine != prDomain->u8SlaveMachineId)
            {
                if (prSync->u64TimerSyncSendDue <= u64CurrentTimeNs)
                {
                    if (true == prSync->bTimerSyncEnabled)
                    {
                        prDomain->bSyncActingGm = true;
                        GPTP_SYNC_SyncMachine(prGptp, prDomain->u8DomainIndex, u8Machine, GPTP_DEF_CALL_INITIATE);
#ifdef GPTP_USE_PRINTF
                        if (false == bActingGmReported)
                        {
                            GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_STATE, ("gptp: Domain %u acting Grand Master. \n", prDomain->u8DomainNumber));
                            bActingGmReported = true;
                        }
#endif /* GPTP_USE_PRINTF */
                    }
                    prSync->u64TimerSyncSendDue = u64CurrentTimeNs + GPTP_INTERNAL_Log2Ns(prSync->s8SyncIntervalLog);
                }
            }
        }
    }

    /* Function call type - from the FUP reception and valid sync received */
    else if ((GPTP_DEF_CALL_PTP_RECEIVED == rCallType) && (true == prDomain->bValidSyncReceived))
    {
        /* Load the sync interval of slave machine */
        s8SlaveIntLog = prDomain->prSyncMachines[prDomain->u8SlaveMachineId].s8SyncIntervalLog;

        /* Check due time for each machine per non GM domain */
        for (u8Machine = 0u; u8Machine <  prDomain->u8NumberOfSyncsPerDom; u8Machine++)
        {
            prSync = &prDomain->prSyncMachines[u8Machine];

            /* Each machine but slave */
            if (u8Machine != prDomain->u8SlaveMachineId)
            {
                /* If sync interval of the master is faster or equal to slave interval */
                if (s8SlaveIntLog >= prSync->s8SyncIntervalLog)
                {
                    prDomain->bSyncActingGm = false;
                    GPTP_SYNC_SyncMachine(prGptp, prDomain->u8DomainIndex, u8Machine, GPTP_DEF_CALL_INITIATE);
                }

                /* If sync interval of the master is slower as slave interval */
                else
                {
                    /* Calculate gap - number of received syncs which are not forwarded to master (per machine) */
                    s8IntervLogDiff = prSync->s8SyncIntervalLog - s8SlaveIntLog;
                    u8Gap = 1u << (uint8_t)s8IntervLogDiff;
                    u8Gap = u8Gap - 1u;

                    if (u8Gap <= prSync->u8SkippedSyncs)
                    {
                        GPTP_SYNC_SyncMachine(prGptp, prDomain->u8DomainIndex, u8Machine, GPTP_DEF_CALL_INITIATE);
                        prSync->u8SkippedSyncs = 0u;
                    }

                    else
                    {
                        prSync->u8SkippedSyncs++;
                    }
                }
            }
        }
    }

    /* Call from periodic function */
    else
    {
        for (u8Machine = 0u; u8Machine < prDomain->u8NumberOfSyncsPerDom; u8Machine++)
        {
            /* But the slave */
            if (u8Machine != prDomain->u8SlaveMachineId)
            {
                prSync = &prDomain->prSyncMachines[u8Machine];
                /* Sync interval for the respective master sync machine */
                u64MasterIntervalNs = GPTP_INTERNAL_Log2Ns(prSync->s8SyncIntervalLog);
                /* If the current time is > 125% sync interval */
                if (u64CurrentTimeNs > (prDomain->u64TimerLastSyncRcvd + (u64MasterIntervalNs + (u64MasterIntervalNs / 4u))))
                {
                    if (prSync->u64TimerSyncSendDue <= u64CurrentTimeNs)
                    {
                        if (true == prSync->bTimerSyncEnabled)
                        {
                            prDomain->bSyncActingGm = false;
                            GPTP_SYNC_SyncMachine(prGptp, prDomain->u8DomainIndex, u8Machine, GPTP_DEF_CALL_INITIATE);
                        }
                        prSync->u64TimerSyncSendDue = u64CurrentTimeNs + GPTP_INTERNAL_Log2Ns(prSync->s8SyncIntervalLog);
                    }
                }
            }
        }
    }
}

/*!
 * @brief The function initializes internal timers.
 *
 * @param[in] Pointer to general gPTP data structure.
 *
 * @ requirements 529199
 * @ requirements 529248
 */
void GPTP_TIMER_TimerInit(gptp_def_data_t *prGptp)
{
    uint8_t                     u8Port;
    uint8_t                     u8Domain;
    uint8_t                     u8Machine;
    gptp_def_domain_t           *prDomain;
    gptp_def_sync_t             *prSync;
    gptp_def_pdelay_t           *prPdelay;
    gptp_def_timestamp_t        rCurrentTimeTs;
    uint64_t                    u64CurrentTimeNs;

    rCurrentTimeTs = GPTP_PORT_CurrentTimeGet(GPTP_DEF_TS_FREERUNNING);
    u64CurrentTimeNs = GPTP_INTERNAL_TsToU64(rCurrentTimeTs);

    /* Init Pdelay timers for each port */
    for (u8Port = 0u; u8Port < prGptp->rPerDeviceParams.u8PortsCount; u8Port++)
    {
        prPdelay = &prGptp->prPdelayMachines[u8Port];

        /* Schedule the first Pdelay due */
        prPdelay->u64TimerPdelaySendDue = u64CurrentTimeNs;

        /* If the pdelay value is not 127 */
        if (GPTP_DEF_SIG_TLV_STOP_SENDING != prPdelay->s8PdelayIntervalLog)
        {
            /* For the port, which has enabled pdelay initiator */
            if (true == prPdelay->bPdelayInitiatorEnabled)
            {
                /* Enable Pdelay measurement timer */
                prPdelay->bTimerPdelayTimerEnabled = true;
            }
        }
        else
        {
            /* Don't enable Pdelay measurement timer */
            prPdelay->bTimerPdelayTimerEnabled = false;
        }
    }

    /* Domain / Sync related timers */
    for (u8Domain = 0u; u8Domain < prGptp->rPerDeviceParams.u8NumberOfDomains; u8Domain++)
    {
        prDomain = &prGptp->prPerDomainParams[u8Domain];

        /* If domain is not GM (Bridge) */
        if (false == prDomain->bDomainIsGm)
        {
            /* Schedule sync receipt timeout error */
            prDomain->u64TimerSyncReceiptTimeoutDue = u64CurrentTimeNs + (GPTP_INTERNAL_Log2Ns(prDomain->prSyncMachines[prDomain->\
                                                      u8SlaveMachineId].s8SyncIntervalLog) * (uint64_t)prDomain->u8SyncReceiptTimeoutCnt);

            /* Check sync period for signaling */
            prDomain->u64TimerSyncCheckDue = u64CurrentTimeNs;

            /* Last sync received */
            prDomain->u64TimerLastSyncRcvd = 0u;

            /* For all sync machines */
            for (u8Machine = 0u; u8Machine < prDomain->u8NumberOfSyncsPerDom; u8Machine++)
            {
                prSync = &prDomain->prSyncMachines[u8Machine];

                /* Except of the slave */
                if (u8Machine != prDomain->u8SlaveMachineId)
                {
                    /* Sync send due is after the startup timeout (if no sync received) */
                    /* Syncs when acting GM */
                    prSync->u64TimerSyncSendDue = u64CurrentTimeNs + ((uint64_t)prDomain->u8StartupTimeoutS * (uint64_t)GPTP_DEF_NS_IN_SECONDS);

                    prSync->u8SkippedSyncs = 0u;

                    /* Enable sync timer only if the sync interval is not equal 127 */
                    if (GPTP_DEF_SIG_TLV_STOP_SENDING != prSync->s8SyncIntervalLog)
                    {
                        /* SW timer enabled */
                        prSync->bTimerSyncEnabled = true;
                    }

                    else
                    {
                        /* SW timer not enabled */
                        prSync->bTimerSyncEnabled = false;
                    }
                }
            }
        }

        /* If the domain is GM */
        else
        {
            /* For all sync machines */
            for (u8Machine = 0u; u8Machine < prDomain->u8NumberOfSyncsPerDom; u8Machine++)
            {
                prSync = &prDomain->prSyncMachines[u8Machine];

                prSync->u64TimerSyncSendDue = u64CurrentTimeNs;

                /* Enable sync timer only if the sync interval is not equal 127 */
                if (GPTP_DEF_SIG_TLV_STOP_SENDING != prSync->s8SyncIntervalLog)
                {
                    /* SW timer enabled */
                    prSync->bTimerSyncEnabled = true;
                }

                else
                {
                    /* SW timer not enabled */
                    prSync->bTimerSyncEnabled = false;
                }
            }
        }
    }

    prGptp->u64SyncStatCalcDue = u64CurrentTimeNs + GPTP_DEF_CALC_PERIOD_100_MS;
}

/*!
 * @brief The function checks internal timers periodically.
 *
 * @param[in] Pointer to general gPTP data structure.
 *
 * @ requirements 529199
 * @ requirements 529249
 * @ requirements 152453
 * @ requirements 162143
 * @ requirements 162146
 * @ requirements 120461
 * @ requirements 120463
 */
void GPTP_TIMER_TimerPeriodic(gptp_def_data_t *prGptp)
{
    static gptp_def_timestamp_t         rFreeRunningClock;
    static gptp_def_timestamp_t         rCorrectedClock;
    gptp_def_domain_t                   *prDomain;
    gptp_def_sync_t                     *prSync;
    gptp_def_pdelay_t                   *prPdelay;
    gptp_def_timestamp_t                rCurrentTimeTs;
    gptp_def_timestamp_sig_t            rFreeRunningClockSig;
    gptp_def_timestamp_sig_t            rCorrectedClockSig;
    gptp_def_timestamp_sig_t            rClockOffsetSig;
    uint8_t                             u8Port;
    uint8_t                             u8Machine;
    uint8_t                             u8Domain;
    int8_t                              s8SyncIntervalMinimum;
    uint64_t                            u64CurrentTimeNs;
    uint64_t                            u64Elapsed;
    int32_t                             s32Compensation;
    gptp_err_type_t                     eErr;
    bool                                bMorePorts;
    bool                                bSendSigMsg;

    /* Update current time */
    rCurrentTimeTs = GPTP_PORT_CurrentTimeGet(GPTP_DEF_TS_FREERUNNING);
    u64CurrentTimeNs = GPTP_INTERNAL_TsToU64(rCurrentTimeTs);

    /* Pdelay scheduling */
    for (u8Port = 0u; u8Port < prGptp->rPerDeviceParams.u8NumberOfPdelayMachines; u8Port++)
    {
        prPdelay = &prGptp->prPdelayMachines[u8Port];

        if (true == prPdelay->bPdelayInitiatorEnabled)
        {
            if (prPdelay->u64TimerPdelaySendDue <= u64CurrentTimeNs)
            {
                if (true ==  prPdelay->bTimerPdelayTimerEnabled)
                {
                    GPTP_PDELAY_PdelayMachine(prGptp, prPdelay, GPTP_DEF_CALL_INITIATE);
                }

                 prPdelay->u64TimerPdelaySendDue = u64CurrentTimeNs + GPTP_INTERNAL_Log2Ns( prPdelay->s8PdelayIntervalLog);
            }
        }
    }

    rCurrentTimeTs = GPTP_PORT_CurrentTimeGet(GPTP_DEF_TS_FREERUNNING);
    u64CurrentTimeNs = GPTP_INTERNAL_TsToU64(rCurrentTimeTs);

    /* Sync scheduling */
    for (u8Domain = 0u; u8Domain < prGptp->rPerDeviceParams.u8NumberOfDomains; u8Domain++)
    {
        prDomain = &prGptp->prPerDomainParams[u8Domain];

        /* For GM domain */
        if (true == prDomain->bDomainIsGm)
        {
            /* Check for each machine */
            for (u8Machine = 0u; u8Machine < prDomain->u8NumberOfSyncsPerDom; u8Machine++)
            {
                prSync = &prDomain->prSyncMachines[u8Machine];

                /* If current time equals to the sync send due */
                if (prSync->u64TimerSyncSendDue <= u64CurrentTimeNs)
                {
                    if (true == prSync->bTimerSyncEnabled)
                    {
                        GPTP_SYNC_SyncMachine(prGptp, prDomain->u8DomainIndex, u8Machine, GPTP_DEF_CALL_INITIATE);
                    }
                    prSync->u64TimerSyncSendDue = u64CurrentTimeNs + GPTP_INTERNAL_Log2Ns(prSync->s8SyncIntervalLog);
                }
            }
        }

        /* For non GM domain (Bridge) */
        else
        {
            GPTP_TIMER_SyncsSendNonGm(prGptp, prDomain, GPTP_DEF_CALL_INITIATE);
        }
    }

    if (true == prGptp->rPerDeviceParams.bSignalingEnabled)
    {
        /* Update current time */
        rCurrentTimeTs = GPTP_PORT_CurrentTimeGet(GPTP_DEF_TS_FREERUNNING);
        u64CurrentTimeNs = GPTP_INTERNAL_TsToU64(rCurrentTimeTs);

        /* Regular period sync intervals check */
        for (u8Domain = 0u; u8Domain < prGptp->rPerDeviceParams.u8NumberOfDomains; u8Domain++)
        {
            prDomain = &prGptp->prPerDomainParams[u8Domain];

            /* Check only for non GM domains */
            if (false == prDomain->bDomainIsGm)
            {
                /* Sync interval check - once per second (signalling to the upstream) */
                if (prDomain->u64TimerSyncCheckDue <= u64CurrentTimeNs)
                {
                    s8SyncIntervalMinimum = GPTP_DEF_LOG_SYNC_INT_MAX;
                    bMorePorts = false;
                    bSendSigMsg = false;

                    /* Endpoint role - one port */
                    if (1u == prDomain->u8NumberOfSyncsPerDom)
                    {
                        prSync = &prDomain->prSyncMachines[0u];
                        /* If the operating sync interval is not equal to the current ingress sync interval */
                        if (prSync->s8OperSyncIntervalLog != prSync->s8SyncIntervalLog)
                        {
                            s8SyncIntervalMinimum = prSync->s8OperSyncIntervalLog;
                            bSendSigMsg = true;
                        }
                    }
                    /* Bridge role - more master ports */
                    else {
                        /* Find the shortest sync interval */
                        for (u8Machine = 0u; u8Machine < prDomain->u8NumberOfSyncsPerDom; u8Machine++)
                        {
                            prSync = &prDomain->prSyncMachines[u8Machine];
                            /* Each enabled port but the slave port */
                            if ((u8Machine != prDomain->u8SlaveMachineId) && (true == *(prSync->pbPortEnabled)))
                            {
                                bMorePorts = true;
                                if (s8SyncIntervalMinimum > prSync->s8SyncIntervalLog)
                                {
                                    s8SyncIntervalMinimum = prSync->s8SyncIntervalLog;
                                }
                            }
                        }

                        /* If the found minimum is not equal to the current ingress sync interval */
                        if ((s8SyncIntervalMinimum != prDomain->prSyncMachines[prGptp->prPerDomainParams[u8Domain].u8SlaveMachineId].s8SyncIntervalLog) && (true == bMorePorts))
                        {
                            bSendSigMsg = true;
                        }
                    }

                    if (true == bSendSigMsg)
                    {
                        /* Send signaling message */
                        GPTP_SIGNALING_SendSignaling(prGptp, u8Domain, s8SyncIntervalMinimum);
                    }

                    /* schedule the next due in 1.5s */
                    prDomain->u64TimerSyncCheckDue = u64CurrentTimeNs + (uint64_t)GPTP_DEF_1S5_IN_NS;
                }
            }
        }
    }

    /* Update current time */
    rCurrentTimeTs = GPTP_PORT_CurrentTimeGet(GPTP_DEF_TS_FREERUNNING);
    u64CurrentTimeNs = GPTP_INTERNAL_TsToU64(rCurrentTimeTs);

    /* Pdelay turnaround time check */
    for (u8Port = 0u; u8Port < prGptp->rPerDeviceParams.u8NumberOfPdelayMachines; u8Port++)
    {
        prPdelay = &prGptp->prPdelayMachines[u8Port];

        /* check for each pdelay initiator */
        if (true == prPdelay->bPdelayInitiatorEnabled)
        {
            if (0u != prPdelay->u64TurnaroundMeasStartPtpNsInit)
            {
                /* If turnaround time is too long, register error */
                if (GPTP_DEF_10_MS_IN_NS < (u64CurrentTimeNs - prPdelay->u64TurnaroundMeasStartPtpNsInit))
                {
                    GPTP_ERR_Register(prPdelay->u8GptpPort, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_L_TOO_LONG_TURN_INIT, GPTP_ERR_SEQ_ID_NOT_SPECIF);
                    /* Don't measure next time */
                    prPdelay->u64TurnaroundMeasStartPtpNsInit = 0u;
                }
            }
        }

        /* check for each pdelay responder */
        if (true == prPdelay->bPdelayResponderEnabled)
        {
            if (0u != prPdelay->u64TurnaroundMeasStartPtpNsResp)
            {
                /* If turnaround time is too long, register error */
                if (GPTP_DEF_10_MS_IN_NS < (u64CurrentTimeNs - prPdelay->u64TurnaroundMeasStartPtpNsResp))
                {
                    /* Report turnaround time error */
                    GPTP_ERR_Register(prPdelay->u8GptpPort, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_L_TOO_LONG_TURN_RESP, GPTP_ERR_SEQ_ID_NOT_SPECIF);
                    /* Don't measure next time */
                    prPdelay->u64TurnaroundMeasStartPtpNsResp = 0u;
                }
            }
        }
    }

    /* Sync residence time check for each non GM domain */
    for (u8Domain = 0u; u8Domain < prGptp->rPerDeviceParams.u8NumberOfDomains; u8Domain++)
    {
        prDomain = &prGptp->prPerDomainParams[u8Domain];

        /* Check only non GM domains */
        if (false == prDomain->bDomainIsGm)
        {
            /* Check all the sync machines under the domain */
            for (u8Machine = 0u; u8Machine < prDomain->u8NumberOfSyncsPerDom; u8Machine++)
            {
                prSync = &prDomain->prSyncMachines[u8Machine];

                /* Check only for master sync machines */
                if (GPTP_DEF_PTP_MASTER == prSync->bSyncRole)
                {
                    /* If residence time start value is not 0, the measurement is in progress (not sync forward finished */
                    if (0u != prSync->u64ResidenceMeasStartPtpNs)
                    {
                        /* If residence time above 10ms, raise error */
                        if (GPTP_DEF_10_MS_IN_NS < (u64CurrentTimeNs - prSync->u64ResidenceMeasStartPtpNs))
                        {
                            if (0u == prSync->u8SkippedSyncs)
                            {
                                GPTP_ERR_Register(u8Machine, u8Domain, GPTP_ERR_L_TOO_LONG_RESIDENCE, GPTP_ERR_SEQ_ID_NOT_SPECIF);
                                /* While the residence time is already reported too long, no need to check again */
                                prSync->u64ResidenceMeasStartPtpNs = 0u;
                            }
                        }
                    }
                }
            }
        }
    }

    /* Sync receipt timeout check */
    for (u8Domain = 0u; u8Domain < prGptp->rPerDeviceParams.u8NumberOfDomains; u8Domain++)
    {
        prDomain = &prGptp->prPerDomainParams[u8Domain];

        /* Check non GM (Bridge) domains */
        if (false == prDomain->bDomainIsGm)
        {
            if (0u != prDomain->u64TimerSyncReceiptTimeoutDue)
            {
                /* If sync receipt timeout due, report error */
                if (prDomain->u64TimerSyncReceiptTimeoutDue <= u64CurrentTimeNs)
                {
                    GPTP_ERR_Register(prDomain->u8SlaveMachineId, u8Domain, GPTP_ERR_S_RECEIPT_TIMEOUT, GPTP_ERR_SEQ_ID_NOT_SPECIF);
                    GPTP_INTERNAL_LosHandle(prGptp, prDomain, prDomain->u8SlaveMachineId, GPTP_ERR_SEQ_ID_NOT_SPECIF);
                    prDomain->u64TimerSyncReceiptTimeoutDue = 0u;

#ifdef GPTP_COUNTERS
                    GPTP_INTERNAL_IncrementPortStats(prGptp, prDomain->u8SlaveMachineId, ieee8021AsPortStatRxSyncReceiptTimeouts);
                    GPTP_INTERNAL_IncrementDomainStats(prGptp, u8Domain, prDomain->u8SlaveMachineId, ieee8021AsPortStatRxSyncReceiptTimeouts);
#endif /* GPTP_COUNTERS */
                }
            }
        }
    }

    if (true == prGptp->bLocClkToUpdate)
    {
        /* Print synchronization status (Offset to GM) */
        if (prGptp->u64SyncStatCalcDue <= u64CurrentTimeNs)
        {
            if (true == prGptp->rPerDeviceParams.bEverUpdated)
            {
                eErr = GPTP_PORT_GetSwitchTimes(&rFreeRunningClock, &rCorrectedClock);
                if (GPTP_ERR_OK != eErr)
                {
                    rFreeRunningClock.u32TimeStampNs = 0u;
                    rFreeRunningClock.u64TimeStampS = 0u;
                    rCorrectedClock.u32TimeStampNs = 0u;
                    rCorrectedClock.u64TimeStampS = 0u;
                }

                u64Elapsed = GPTP_INTERNAL_TsToU64(GPTP_INTERNAL_TsMinTs(rFreeRunningClock, prGptp->rPerDeviceParams.rLastFreeRunRef));

                s32Compensation = (int32_t)(((int64_t)(((float64_t)u64Elapsed) * prGptp->rPerDeviceParams.f64CorrClockRateRatio)) - (int64_t)u64Elapsed);

                rFreeRunningClockSig.s32TimeStampNs = (int32_t)rFreeRunningClock.u32TimeStampNs;
                rFreeRunningClockSig.s64TimeStampS = (int64_t)rFreeRunningClock.u64TimeStampS;
                rCorrectedClockSig.s32TimeStampNs = (int32_t)rCorrectedClock.u32TimeStampNs;
                rCorrectedClockSig.s64TimeStampS = (int64_t)rCorrectedClock.u64TimeStampS;

                if (rCorrectedClockSig.s32TimeStampNs >= s32Compensation)
                {
                    rCorrectedClockSig.s32TimeStampNs -= s32Compensation;
                }
                else
                {
                    rCorrectedClockSig.s32TimeStampNs -= s32Compensation;
                    rCorrectedClockSig.s32TimeStampNs *= -1;
                    rCorrectedClockSig.s32TimeStampNs = ((int32_t)GPTP_DEF_NS_IN_SECONDS) - rCorrectedClockSig.s32TimeStampNs;
                    rCorrectedClockSig.s64TimeStampS -= 1;
                }

                rClockOffsetSig = GPTP_INTERNAL_TsMinTsSig(rFreeRunningClockSig, rCorrectedClockSig);
                if (0 > rClockOffsetSig.s32TimeStampNs)
                {
                    rClockOffsetSig.s32TimeStampNs *= -1;
                    rClockOffsetSig.s64TimeStampS *= -1;
                }

                prGptp->rPerDeviceParams.rEstimGmOffset = GPTP_INTERNAL_TsMinTsSig(rClockOffsetSig, prGptp->rPerDeviceParams.rReportedOffset);
                prGptp->rPerDeviceParams.bEstimGmOffsetCalculated = true;

                GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_SYNC, ("gptp: Estimated offset to GM:"));
                GPTP_PRINTF_TIME_SIG(GPTP_DEBUG_MSGTYPE_SYNC, prGptp->rPerDeviceParams.rEstimGmOffset);
            }

            else
            {
                GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_SYNC, ("gptp: Local clock not updated yet \n"));
            }

            /* Report Synchronization status periodically with given period */
            prGptp->u64SyncStatCalcDue = u64CurrentTimeNs + GPTP_DEF_CALC_PERIOD_100_MS;
        }
    }

    /* Call platfofm specific part of the Periodic functionality */
    GPTP_PORT_TimerPeriodic(prGptp);
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
