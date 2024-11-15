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
 * @file gptp_pdelay.c
 *
 */

#include "gptp_def.h"
#include "gptp_err.h"
#include "gptp_timer.h"
#include "gptp_frame.h"
#include "gptp_port.h"
#include "gptp.h"
#include "gptp_internal.h"
#include "gptp_pdelay.h"

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Pdelay state machine main function
 *
 * This function services Pdelay state machine. It receives and sends Pdelay messages
 * to measure Path delay.
 *
 * @param[in] prGptp Pointer to global gPTP structure.
 * @param[in] prPdelMachine Pointer to Pdelay state machine structure.
 * @param[in] eCallType Call type - specifying upon which event the function is called.
 *
 * @ requirements 529199
 * @ requirements 529212
 * @ requirements 152449
 * @ requirements 162144
 * @ requirements 120455
 * @ requirements 120465
 * @ requirements 162152
 * @ requirements 162154
 * @ requirements 162155
 * @ requirements 162153
 * @ requirements 162157
 * @ requirements 162159
 * @ requirements 120472
 * @ requirements 162162
 */
void GPTP_PDELAY_PdelayMachine(const gptp_def_data_t *prGptp,
                               gptp_def_pdelay_t *prPdelMachine,
                               const gptp_def_sm_call_type_t eCallType)
{
    float64_t                    f64CurrentDelay;
    float64_t                    f64TempDelay;
    float64_t                    f64TempRatio;
    uint64_t                     u64TempT1;
    int64_t                      s64TempT1;
    uint64_t                     u64TempT2;
    gptp_def_timestamp_t         rTempTs1;
    gptp_def_timestamp_sig_t     rTempTs1Sig;
    gptp_def_timestamp_sig_t     rTempTs2Sig;
    gptp_def_timestamp_sig_t     rTmpSigTs1;
    gptp_def_timestamp_sig_t     rTmpSigTs2;
    gptp_def_timestamp_t         rTempTs2;
    uint64_t                     u64Turnaroud;
    bool                         bTempResult;

#ifdef GPTP_TIME_VALIDATION
    gptp_def_pdelay_init_meas_t  rInitiatorNotifData;
    gptp_def_pdelay_resp_meas_t  rResponderNotifData;
    gptp_err_type_t              eErr;
#endif /* GPTP_TIME_VALIDATION */

    /* Distinguish among call types - event caused the function call */
    switch (eCallType)
    {
        /* The function has been called from the periodic timer */
        case GPTP_DEF_CALL_INITIATE:
            /* PDELAY INITIATOR */
            /* If the port is enabled and pdelay initiator role is enabled for this port */
            if ((true == *(prPdelMachine->pbPortEnabled)) && (true == prPdelMachine->bPdelayInitiatorEnabled))
            {
                /* If the pdelay state machine is waiting for the pdelay response or pdelay response follow up */
                if ((GPTP_DEF_WAITING_FOR_PD_RESP == prPdelMachine->ePdelayInitiatorState) || \
                    (GPTP_DEF_WAITING_FOR_PD_RESP_F == prPdelMachine->ePdelayInitiatorState))
                {
                    /* Error, the pdelay state stucked waiting for the message (did not receive message) */

                    /* Reset the state machine */
                    prPdelMachine->ePdelayInitiatorState = GPTP_DEF_PDELAY_RESET;

                    /* Increment number of lost responses */
                    prPdelMachine->u16LostResponses++;

                    /* If the limit for lost responses is exceeded by 1 */
                    if (prPdelMachine->u16LostResponses == (prPdelMachine->u16PdelLostReponsesAllowedCnt + 1u))
                    {
                        /* Clear the averager initialized flag */
                        prPdelMachine->bPdelAveragerInitialized = false;
                        /* Clear the averager initialized flag */
                        prPdelMachine->bRatioAveragerInitialized = false;
                        /* Count of pdelay responses before slowing down the pdelay interval */
                        prPdelMachine->u16PdelayResponsesCnt = 0u;

                        /* set initial pdelay period */
                        prPdelMachine->s8PdelayIntervalLog = prPdelMachine->s8InitPdelayIntervalLog;

                        /* Register error - out of allowed lost responses limit */
                        GPTP_ERR_Register(prPdelMachine->u8GptpPort, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_P_TOO_MANY_LOST_RESPS, prPdelMachine->u16SequenceId);

#ifdef GPTP_COUNTERS
                        GPTP_INTERNAL_IncrementPortStats(prGptp, prPdelMachine->u8GptpPort, ieee8021AsPortStatPdelayAllowedLostResponsesExceeded);
#endif /* GPTP_COUNTERS */
                    }
#ifdef GPTP_COUNTERS
                    GPTP_INTERNAL_IncrementPortStats(prGptp, prPdelMachine->u8GptpPort, ieee8021AsPortStatRxPTPPacketDiscard);
#endif /* GPTP_COUNTERS */

                    /* Set the pdelay initiator state to send pdelay request */
                    prPdelMachine->ePdelayInitiatorState = GPTP_DEF_SEND_PDELAY_REQ;
                }

                /* Pdelay initiator state switch */
                switch (prPdelMachine->ePdelayInitiatorState)
                {
                        /* Pdelay measurement is not enabled */
                    case GPTP_DEF_INITIATOR_NOT_ENABLED:
                        /* Enable it and set the state to send pdelay request */
                        prPdelMachine->ePdelayInitiatorState = GPTP_DEF_INIIAL_SEND_PD_REQ;

                        /* Initial pdelay request send */
                        /* fallthrough */
                    case GPTP_DEF_INIIAL_SEND_PD_REQ:

                        /* Initialize the measurement */
                        /* Reset number of lost responses */
                        prPdelMachine->u16LostResponses = 0u;
                        /* Clear averager initialized flag */
                        prPdelMachine->bPdelAveragerInitialized = false;
                        /* Clear the averager initialized flag */
                        prPdelMachine->bRatioAveragerInitialized = false;
                        /* Count of pdelay responses before slowing down the pdelay interval */
                        prPdelMachine->u16PdelayResponsesCnt = 0u;
                        /* Set the initial period value for the pdelay measurement */
                        prPdelMachine->s8PdelayIntervalLog = prPdelMachine->s8InitPdelayIntervalLog;

                        /* Waiting for prelay interval */
                        /* fallthrough */
                    case GPTP_DEF_WAITING_FOR_PD_INTERV:

                        /* Set the pdelay initiator state to send pdelay request */
                        prPdelMachine->ePdelayInitiatorState = GPTP_DEF_SEND_PDELAY_REQ;
                        /* fallthrough */
                    case GPTP_DEF_SEND_PDELAY_REQ:

                        /* Increment the sequence ID */
                        prPdelMachine->u16SequenceId++;

                        /* Clear pdelay response received flag */
                        prPdelMachine->bPdelayRespReceived = false;
                        /* Clear pdelay response follow up received flag */
                        prPdelMachine->bPdelayRespFupReceived = false;
                        /* Clear T1 time stamp registred flag */
                        prPdelMachine->bT1TsRegistred = false;

                        /* Copy the Pdelay data into the TX structure */
                        prPdelMachine->rTxDataReq.eMsgId = GPTP_DEF_MSG_TYPE_PD_REQ;
                        prPdelMachine->rTxDataReq.u16SequenceId = prPdelMachine->u16SequenceId;
                        prPdelMachine->rTxDataReq.s8LogMessagePeriod = prPdelMachine->s8PdelayIntervalLog;
                        prPdelMachine->rTxDataReq.pau8TxBuffPtr = prPdelMachine->au8SendBuffPdelReq;
                        prPdelMachine->rTxDataReq.bTsRequested = true;
                        prPdelMachine->rTxDataReq.u16VlanTci = prGptp->rPerDeviceParams.u16VlanTci;

                        /* Use clockID and portID from device configuration */
                        prPdelMachine->rTxDataReq.rSrcPortID.u64ClockId = prGptp->prPerPortParams[prPdelMachine->u8GptpPort].u64ClockId;
                        prPdelMachine->rTxDataReq.rSrcPortID.u16PortId = (uint16_t)prPdelMachine->u8GptpPort + 1u;

                        /* Set correct destination MAC address. */
                        prPdelMachine->rTxDataReq.u64DestMac = (uint64_t)GPTP_DEF_ETH_PTP_MAC_MULTICAST;

#ifdef GPTP_TIME_VALIDATION
                        eErr = GPTP_PORT_GetSwitchTimes(&(prPdelMachine->rReferenceLocalTimestampInit), &(prPdelMachine->rReferenceGlobalTimestampInit));
                        if (GPTP_ERR_OK != eErr)
                        {
                            prPdelMachine->rReferenceLocalTimestampInit.u32TimeStampNs = 0u;
                            prPdelMachine->rReferenceLocalTimestampInit.u64TimeStampS = 0u;
                            prPdelMachine->rReferenceGlobalTimestampInit.u32TimeStampNs = 0u;
                            prPdelMachine->rReferenceGlobalTimestampInit.u64TimeStampS = 0u;
                            GPTP_ERR_Register(prPdelMachine->u8GptpPort, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_F_TIME_READ, prPdelMachine->u16SequenceId);
                        }
#endif /* GPTP_TIME_VALIDATION */

                        /* Transmit message, Timestamp required */
                        GPTP_MsgTransmit(&prPdelMachine->rTxDataReq, prPdelMachine->u8GptpPort, prPdelMachine->u8GptpPort, 0u);
                        prPdelMachine->u64TurnaroundMeasStartPtpNsInit = GPTP_INTERNAL_TsToU64(GPTP_PORT_CurrentTimeGet(GPTP_DEF_TS_FREERUNNING));
                        /* The next pdelay initiator state is Waiting for pdelay response */
                        prPdelMachine->ePdelayInitiatorState = GPTP_DEF_WAITING_FOR_PD_RESP;
                    break;

                    default:
                    break;
                }
            }
        break;

        /* The function has been called after the message receive */
        case GPTP_DEF_CALL_PTP_RECEIVED:
            /* PDELAY INITIATOR */
            /* Switch the pdelay initiator state */
            switch (prPdelMachine->ePdelayInitiatorState)
            {
                /* Waiting for the pdelay response */
                case GPTP_DEF_WAITING_FOR_PD_RESP:
                    /* If pdelay response received */
                    if (true == prPdelMachine->bPdelayRespReceived)
                    {
                        /* If the sequence Id of transmitted request matches with the received sequence id from the pdelay response and
                           clock Id of this instance matches with received requesting clock Id from the pdelay response and
                           Id of this port matches with the received port Id from the pdelay response */
                        if ((prPdelMachine->u16SequenceId == prPdelMachine->rPdRespMsgRx.rHeader.u16SequenceId)&&
                           (*(prPdelMachine->pu64ClockId) == prPdelMachine->rPdRespMsgRx.rRequestingId.u64ClockId)&&
                           (prPdelMachine->u8GptpPort == prPdelMachine->rPdRespMsgRx.rRequestingId.u16PortId))
                        {
                            /* Clear Pdelay response received flag */
                            prPdelMachine->bPdelayRespReceived = false;
                            /* Set the new pdelay initiator state - waiting for the response follow up */
                            prPdelMachine->ePdelayInitiatorState = GPTP_DEF_WAITING_FOR_PD_RESP_F;
                        }

                        else
                        {
                            GPTP_ERR_Register(prPdelMachine->u8GptpPort, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_M_IDENTIFIERS_MISMATCH, prPdelMachine->rPdRespMsgRx.rHeader.u16SequenceId);
#ifdef GPTP_COUNTERS
                            GPTP_INTERNAL_IncrementPortStats(prGptp, prPdelMachine->u8GptpPort, ieee8021AsPortStatRxPTPPacketDiscard);
#endif /* GPTP_COUNTERS */
                        }
                    }

                break;

                /* Waiting for the pdelay response follow up */
                case GPTP_DEF_WAITING_FOR_PD_RESP_F:
                    /* If pdelay response follow up message received and if the T1 timestamp from previously transmitted pdelay response message is registered */
                    if ((true == prPdelMachine->bPdelayRespFupReceived) && (true == prPdelMachine->bT1TsRegistred))
                    {
                        /* Clear the T1 timestam registered flag */
                        prPdelMachine->bT1TsRegistred = false;

                        /* Clear pdelay response follow up received flag */
                        prPdelMachine->bPdelayRespFupReceived = false;

                        /* If the sequence Id of transmitted request matches with the received sequence id from the pdelay response and
                           Id of this port matches with the received requesting port Id from the pdelay response and
                           and sequence Id matches with the sequence Id of transmitted message (gained in the timestamp handler from the
                           frame ID */
                        if ((prPdelMachine->rPdRespMsgRx.rHeader.u16SequenceId == prPdelMachine->rPdRespFupMsgRx.rHeader.u16SequenceId) &&
                           (prPdelMachine->rPdRespMsgRx.rRequestingId.u16PortId == prPdelMachine->rPdRespFupMsgRx.rRequestingId.u16PortId) &&
                           (prPdelMachine->rPdRespMsgRx.rHeader.u16SourcePortId == prPdelMachine->rPdRespFupMsgRx.rHeader.u16SourcePortId) &&
                           (prPdelMachine->u16T1TsSeqId == prPdelMachine->u16SequenceId))
                        {
                            /* If the calculation of the Neighbor rate ratio is required */
                            if (true == prPdelMachine->bComputeNeighborRateRatio)
                            {
                                /* Neighbor ratio calculation */

                                /* Check if the actual timestamps T1 and T2 have higher value than timestamps from previous calculation */
                                bTempResult = GPTP_INTERNAL_TsAboveTs(prPdelMachine->rPdRespMsgRx.rT2Ts, prPdelMachine->rNeighborCalcPrevT2);
                                if ((true == GPTP_INTERNAL_TsAboveTs(prPdelMachine->rT1Ts, prPdelMachine->rNeighborCalcPrevT1)) && (true == bTempResult))
                                {
                                    /* Calculate subtract of current T2 timestamp and previous T2 timestamp */
                                    rTempTs2 = GPTP_INTERNAL_TsMinTs(prPdelMachine->rPdRespMsgRx.rT2Ts, prPdelMachine->rNeighborCalcPrevT2);
                                    /* Calculate subtract of current T1 timestamp and previous T1 timestamp */
                                    rTempTs1 = GPTP_INTERNAL_TsMinTs(prPdelMachine->rT1Ts, prPdelMachine->rNeighborCalcPrevT1);

                                    /* Calculate T2 nanosecond value from the timestamp information containing seconds and nanoseconds (small enough to not loose information) */
                                    u64TempT2 = GPTP_INTERNAL_TsToU64(rTempTs2);
                                    /* Calculate T1 nanosecond value from the timestamp information containing seconds and nanoseconds (small enough to not loose information) */
                                    u64TempT1 = GPTP_INTERNAL_TsToU64(rTempTs1);

                                    /* Calculate temprary ratio value in double from T2 and T1 timestamps */
                                    if (0u == u64TempT1)
                                    {
                                        /* Register error - result of previous calculation is 0, can't divide by 0 */
                                        GPTP_ERR_Register(prPdelMachine->u8GptpPort, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_A_DIV_BY_0_ATTEMPT, prPdelMachine->rPdRespMsgRx.rHeader.u16SequenceId);
                                    }

                                    else
                                    {
                                        prPdelMachine->f64TempRateRatioCurrent = (float64_t)u64TempT2 / (float64_t)u64TempT1;
                                    }

                                    /* Check if the rate ratio is inside of the limit */
                                    prPdelMachine->bNeighborRateRatioValidCurrent = GPTP_INTERNAL_RateRratioCheck(prGptp->rPerDeviceParams.f64RratioMaxDev, &prPdelMachine->f64TempRateRatioCurrent);

                                    if (true == prPdelMachine->bNeighborRateRatioValidCurrent)
                                    {
                                        /* If the averager is initialized */
                                        if (true == prPdelMachine->bRatioAveragerInitialized)
                                        {
                                            /* Store the neighbor propagation delay from previous measurement into temp variable */
                                            f64TempRatio = prPdelMachine->f64NeighborRateRatioCurrent;
                                            /* Apply exponential averaging filter */
                                            prPdelMachine->f64NeighborRateRatioCurrent = ((prGptp->rPerDeviceParams.f64RratioAvgWeight * f64TempRatio) + \
                                                                                       ((1.0 - prGptp->rPerDeviceParams.f64RratioAvgWeight) * prPdelMachine->f64TempRateRatioCurrent));
                                        }

                                        /* If averager is not initialized */
                                        else
                                        {
                                            /* Save the calculated ratio into the global data structure for the future use */
                                            prPdelMachine->f64NeighborRateRatioCurrent = prPdelMachine->f64TempRateRatioCurrent;
                                            /* Set averager initialized flag*/
                                            prPdelMachine->bRatioAveragerInitialized = true;
                                        }

                                        if (false == prPdelMachine->bNeighborRateRatioValid)
                                        {
                                            prPdelMachine->f64NeighborRateRatio = prPdelMachine->f64NeighborRateRatioCurrent;
                                        }
                                    }
                                }

                                /* If the previous timestamp t1 and t2 are higher than the current */
                                else
                                {
                                    /* Something is wrong */
                                    /* Clear neighbor rate ratio valid flag */
                                    prPdelMachine->bNeighborRateRatioValidCurrent = false;
                                }

                                /* Save current T1 and T2 timestamps as previous for the next calculation */
                                prPdelMachine->rNeighborCalcPrevT1 = prPdelMachine->rT1Ts;
                                prPdelMachine->rNeighborCalcPrevT2 = prPdelMachine->rPdRespMsgRx.rT2Ts;
                            }

                            /* If the calculation of the neighbor propagation delay is required  */
                            if (true == prPdelMachine->bComputeNeighborPropDelay)
                            {
                                /* PDELAY calculation */
                                rTmpSigTs1.s32TimeStampNs = (int32_t)prPdelMachine->rPdRespMsgRx.rT4Ts.u32TimeStampNs;
                                rTmpSigTs1.s64TimeStampS = (int64_t)prPdelMachine->rPdRespMsgRx.rT4Ts.u64TimeStampS;
                                rTmpSigTs2.s32TimeStampNs = (int32_t)prPdelMachine->rT1Ts.u32TimeStampNs;
                                rTmpSigTs2.s64TimeStampS = (int64_t)prPdelMachine->rT1Ts.u64TimeStampS;
                                rTempTs1Sig = GPTP_INTERNAL_TsMinTsSig(rTmpSigTs1, rTmpSigTs2);
                                u64Turnaroud = (uint64_t)GPTP_INTERNAL_TsToS64(rTempTs1Sig);

                                /* Calculate subtract of T3 and T2 timestamps */
                                rTmpSigTs1.s32TimeStampNs = (int32_t)prPdelMachine->rPdRespFupMsgRx.rT3Ts.u32TimeStampNs;
                                rTmpSigTs1.s64TimeStampS = (int64_t)prPdelMachine->rPdRespFupMsgRx.rT3Ts.u64TimeStampS;
                                rTmpSigTs2.s32TimeStampNs = (int32_t)prPdelMachine->rPdRespMsgRx.rT2Ts.u32TimeStampNs;
                                rTmpSigTs2.s64TimeStampS = (int64_t)prPdelMachine->rPdRespMsgRx.rT2Ts.u64TimeStampS;
                                rTempTs2Sig = GPTP_INTERNAL_TsMinTsSig(rTmpSigTs1, rTmpSigTs2);

                                /* Calculate subtract of previously caltulated results and transfer it into the nanosecond (small
                                   enough to not loose information)*/
                                s64TempT1 = GPTP_INTERNAL_TsToS64(GPTP_INTERNAL_TsMinTsSig(rTempTs1Sig, rTempTs2Sig));

                                /* Calculate current Pdelay using neighbor rate ratio and previous result */
                                f64CurrentDelay = (prPdelMachine->f64NeighborRateRatio * (float64_t)s64TempT1) / 2.0;

                                /* If measuring turnaround (measurement not ended in the periodic function */
                                if (0u != prPdelMachine->u64TurnaroundMeasStartPtpNsInit)
                                {
                                    /* If turnaround time is too long, register error */
                                    if (GPTP_DEF_10_MS_IN_NS < u64Turnaroud)
                                    {
                                        GPTP_ERR_Register(prPdelMachine->u8GptpPort, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_L_TOO_LONG_TURN_INIT, \
                                                          prPdelMachine->rPdRespFupMsgRx.rHeader.u16SequenceId);
                                    }
                                }
                                /* Follow Up received and turnaround already check. Not check in the periodic func */
                                prPdelMachine->u64TurnaroundMeasStartPtpNsInit = 0u;

                                /* If the current delay is below the threshold, neighbor rate ratio is valid and
                                   requesting clock Id from pdelay response matches with this clock Id of this instance */
                                if ((true == GPTP_INTERNAL_PdelayCheck(prPdelMachine->u8GptpPort, (float64_t)prPdelMachine->u64NeighborPropDelayThreshNs, f64CurrentDelay)) && \
                                    (*(prPdelMachine->pu64ClockId) == prPdelMachine->rPdRespMsgRx.rRequestingId.u64ClockId))
                                {
                                    if (true == prPdelMachine->bPdelAveragerInitialized)
                                    {
                                        /* Store the neighbor propagation delay from previous measurement into temp variable */
                                        f64TempDelay = prPdelMachine->f64NeighborPropDelayCurrent;
                                        /* Apply exponential averaging filter */
                                        prPdelMachine->f64NeighborPropDelayCurrent = ((prGptp->rPerDeviceParams.f64PdelAvgWeight * f64TempDelay) + \
                                                                                   ((1.0 - prGptp->rPerDeviceParams.f64PdelAvgWeight) * f64CurrentDelay));
                                    }

                                    else
                                    {
                                        /* Store current Pdelay into the global data structure */
                                        prPdelMachine->f64NeighborPropDelayCurrent = f64CurrentDelay;
                                        /* Set averager initialized flag*/
                                        prPdelMachine->bPdelAveragerInitialized = true;
                                    }

                                    if (false == prPdelMachine->bPdelayValueValid)
                                    {
                                        prPdelMachine->f64NeighborPropDelay = prPdelMachine->f64NeighborPropDelayCurrent;
                                    }

                                    /* Current Pdelay measurement is valid */
                                    prPdelMachine->bPdelayValueValidCurrent = true;
                                    /* Clear lost responses counter */
                                    prPdelMachine->u16LostResponses = 0u;

                                    if (prPdelMachine->u16PdelayResponsesCnt < prPdelMachine->u16MsmtsTillSlowDown)
                                    {
                                        prPdelMachine->u16PdelayResponsesCnt++;
                                        GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_INFO, ("gptp: Pdelay measurement no. %u, result %f [ns]. Filtered value: %f [ns]\n", \
                                                    prPdelMachine->u16PdelayResponsesCnt, f64CurrentDelay, prPdelMachine->f64NeighborPropDelayCurrent));
                                    }

                                    if (prPdelMachine->u16PdelayResponsesCnt > prPdelMachine->u16MsmtsTillSlowDown)
                                    {
                                        /* Update Rate ratio value from the current measurement */
                                        prPdelMachine->f64NeighborRateRatio = prPdelMachine->f64NeighborRateRatioCurrent;
                                        /* Update Pdelay value from the current measurement */
                                        prPdelMachine->f64NeighborPropDelay = prPdelMachine->f64NeighborPropDelayCurrent;
                                    }

                                    /* If count of pdelay responses reach the limit to slow down - the pdelay measurement is stabilized */
                                    if (prPdelMachine->u16PdelayResponsesCnt == prPdelMachine->u16MsmtsTillSlowDown)
                                    {
                                        /* Increment one more time to not return here next time */
                                        prPdelMachine->u16PdelayResponsesCnt++;
                                        /* Update the pdelay interval to operational (usualy slow down) */
                                        prPdelMachine->s8PdelayIntervalLog = prPdelMachine->s8OperPdelayIntervalLog;
                                        /* Update next pdelay due */
                                        GPTP_TIMER_PdelayDueUpdate(prPdelMachine);

                                        /* Read rate ratio from the NVM */
                                        GPTP_PORT_ValueNvmRead(prPdelMachine, GPTP_DEF_NVM_RRATIO, prGptp);
                                        /* If write threshold is not exceeded */
                                        if (((prPdelMachine->f64NeighborRateRatioCurrent + prGptp->rPerDeviceParams.f64RratioNvmWriteThr) > prPdelMachine->f64NeighborRateRatio) && \
                                            ((prPdelMachine->f64NeighborRateRatioCurrent - prGptp->rPerDeviceParams.f64RratioNvmWriteThr) < prPdelMachine->f64NeighborRateRatio))
                                        {
                                            /* Nothing to do */
                                        }
                                        /* Threshold exceeded, NVM update */
                                        else
                                        {
                                            prPdelMachine->f64RratioToWrite = prPdelMachine->f64NeighborRateRatioCurrent;
                                            prPdelMachine->rRratioNvmWriteStat = GPTP_DEF_MEM_WRITE_INIT;
                                        }
                                        /* Update Rate ratio value from the current measurement */
                                        prPdelMachine->f64NeighborRateRatio = prPdelMachine->f64NeighborRateRatioCurrent;
                                        prPdelMachine->bNeighborRateRatioValid = true;

                                        /* Read Pdelay value from the NVM */
                                        GPTP_PORT_ValueNvmRead(prPdelMachine, GPTP_DEF_NVM_PDELAY, prGptp);
                                        /* If write threshold is not exceeded */
                                        if (((prPdelMachine->f64NeighborPropDelayCurrent + prGptp->rPerDeviceParams.f64PdelayNvmWriteThr) > prPdelMachine->f64NeighborPropDelay) && \
                                            ((prPdelMachine->f64NeighborPropDelayCurrent - prGptp->rPerDeviceParams.f64PdelayNvmWriteThr) < prPdelMachine->f64NeighborPropDelay))
                                        {
                                            /* Nothing to do */
                                        }
                                        /* Threshold exceeded, NVM update */
                                        else
                                        {
                                            prPdelMachine->f64PdelayToWrite = prPdelMachine->f64NeighborPropDelayCurrent;
                                            prPdelMachine->rPdelayNvmWriteStat = GPTP_DEF_MEM_WRITE_INIT;
                                        }
                                        /* Update Pdelay value from the current measurement */
                                        prPdelMachine->f64NeighborPropDelay = prPdelMachine->f64NeighborPropDelayCurrent;
                                        prPdelMachine->bPdelayValueValid = true;

                                        GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_INFO, ("gptp: Filtered Pdelay value after %u measurement(s) is %f [ns]. \n", \
                                                    prPdelMachine->u16MsmtsTillSlowDown, prPdelMachine->f64NeighborPropDelay));
                                    }

                                    GPTP_PORT_ValueNvmWrite(prPdelMachine, GPTP_DEF_NVM_PDELAY, prGptp);
                                    GPTP_PORT_ValueNvmWrite(prPdelMachine, GPTP_DEF_NVM_RRATIO, prGptp);
                                }

                                /* If calculated delay is above the threshold or neighbor rate ratio is not valid or clock ID from the
                                   Pdelay response does not match witch clock Id of this instance */
                                else
                                {
                                    /* Clear number of received responses */
                                    prPdelMachine->u16PdelayResponsesCnt = 0u;
                                    /* Current Pdelay measurement is NOT valid */
                                    prPdelMachine->bPdelayValueValidCurrent = false;
                                }
                            }

                            /* Change state to waiting for Pdelay interval */
                            prPdelMachine->ePdelayInitiatorState = GPTP_DEF_WAITING_FOR_PD_INTERV;
                        }

                        /* If Port Id or sequence Id does not match */
                        else
                        {
                            GPTP_ERR_Register(prPdelMachine->u8GptpPort, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_M_IDENTIFIERS_MISMATCH, \
                                              prPdelMachine->rPdRespMsgRx.rHeader.u16SequenceId);
#ifdef GPTP_COUNTERS
                            GPTP_INTERNAL_IncrementPortStats(prGptp, prPdelMachine->u8GptpPort, ieee8021AsPortStatRxPTPPacketDiscard);
#endif /* GPTP_COUNTERS */
                        }

#ifdef GPTP_TIME_VALIDATION
                        if (NULL != prGptp->rPtpStackCallBacks.pfSetPdelayInitiatorData)
                        {
                            rInitiatorNotifData.u16SequenceId = prPdelMachine->u16SequenceId;
                            rInitiatorNotifData.rRequestPortId.u64ClockId = prGptp->prPerPortParams[(prPdelMachine->u8GptpPort)].u64ClockId;
                            rInitiatorNotifData.rRequestPortId.u16PortId = (((uint16_t)prPdelMachine->u8GptpPort) + 1u);
                            rInitiatorNotifData.rResponsePortId.u64ClockId = prPdelMachine->rPdRespFupMsgRx.rHeader.u64SourceClockId;
                            rInitiatorNotifData.rResponsePortId.u16PortId = prPdelMachine->rPdRespFupMsgRx.rHeader.u16SourcePortId + 1u;
                            rInitiatorNotifData.rRequestOriginTimestamp = prPdelMachine->rT1Ts;
                            rInitiatorNotifData.rResponseReceiptTimestamp = prPdelMachine->rPdRespMsgRx.rT4Ts;
                            rInitiatorNotifData.rRequestReceiptTimestamp = prPdelMachine->rPdRespMsgRx.rT2Ts;
                            rInitiatorNotifData.rResponseOriginTimestamp = prPdelMachine->rPdRespFupMsgRx.rT3Ts;
                            rInitiatorNotifData.rReferenceLocalTimestampInit = prPdelMachine->rReferenceLocalTimestampInit;
                            rInitiatorNotifData.rReferenceGlobalTimestampInit = prPdelMachine->rReferenceGlobalTimestampInit;
                            /* If the Pdelay value is negative, use 0 instead the negative value */
                            if ( 0.0 > prPdelMachine->f64NeighborPropDelayCurrent)
                            {
                                rInitiatorNotifData.u32Pdelay = 0u;
                            }

                            else
                            {
                                rInitiatorNotifData.u32Pdelay = (uint32_t) prPdelMachine->f64NeighborPropDelayCurrent;
                            }

                            prGptp->rPtpStackCallBacks.pfSetPdelayInitiatorData(rInitiatorNotifData);
                        }
                        else
                        {
                            GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_F_NULL_PTR, GPTP_ERR_SEQ_ID_NOT_SPECIF);
                        }
#endif /* GPTP_TIME_VALIDATION */
                    }

                break;

                default:
                break;
            }

            /* PDELAY RESPONDER */
            /* If the Pdelay responder is enabled, if Pdelay request message is received */
            if (((true == *(prPdelMachine->pbPortEnabled)) && (true == prPdelMachine->bPdelayResponderEnabled)) && (true == prPdelMachine->bPdelayReqReceived))
            {
                /* Switch Pdelay responder state */
                switch (prPdelMachine->ePdelayResponderState)
                {
                    /* If the Pdelay responder state is waiting for timestamp and another request is received  */
                    /* RESTART state machine */
                    case GPTP_DEF_SENT_PD_RESP_WTING_4_T:
                        /* Register error, not received timestamp */
                        GPTP_ERR_Register(prPdelMachine->u8GptpPort, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_T_TS_NOT_REGISTERED, \
                                          prPdelMachine->rPdRespMsgRx.rHeader.u16SequenceId);
                        /* If Pdelay responder is not enabled yet */
                        /* fallthrough */
                    case GPTP_DEF_RESPONDER_NOT_ENABLED:
                        /* Change state tp waiting for the Pdelay request */
                        prPdelMachine->ePdelayResponderState = GPTP_DEF_INIT_WAITING_FOR_PD_R;
                        /* If Pdelay responder is in waitning for pdelay request state */
                        /* fallthrough */
                    case GPTP_DEF_INIT_WAITING_FOR_PD_R:
                        /* If Pdelay responder is in waiting for Pdelay request state */
                        /* fallthrough */
                    case GPTP_DEF_WAITING_FOR_PDELAY_REQ:
                        /* Clear Pdelay message request received flag */
                        prPdelMachine->bPdelayReqReceived = false;

                        /* Prepare the data for the Pdelay response */
                        /* Message type - Pdelay response */
                        prPdelMachine->rTxDataResp.eMsgId = GPTP_DEF_MSG_TYPE_PD_RESP;
                        /* Copy the Sequence Id from the received Pdelay request message into the Pdelay response */
                        prPdelMachine->rTxDataResp.u16SequenceId = prPdelMachine->rPdReqMsgRx.rHeader.u16SequenceId;
                        /* Save sqeuence ID for later check, after timestamp received */
                        prPdelMachine->u16RespSeqId = prPdelMachine->rTxDataResp.u16SequenceId;
#ifdef GPTP_TIME_VALIDATION
                        eErr = GPTP_PORT_GetSwitchTimes(&(prPdelMachine->rReferenceLocalTimestampResp), &(prPdelMachine->rReferenceGlobalTimestampResp));
                        if (GPTP_ERR_OK != eErr)
                        {
                            prPdelMachine->rReferenceLocalTimestampResp.u32TimeStampNs = 0u;
                            prPdelMachine->rReferenceLocalTimestampResp.u64TimeStampS = 0u;
                            prPdelMachine->rReferenceGlobalTimestampResp.u32TimeStampNs = 0u;
                            prPdelMachine->rReferenceGlobalTimestampResp.u64TimeStampS = 0u;
                            GPTP_ERR_Register(prPdelMachine->u8GptpPort, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_F_TIME_READ, prPdelMachine->u16RespSeqId);
                        }
#endif /* GPTP_TIME_VALIDATION */
                        /* Copy the T2 timestamp (reception of the Pdelay request message) to the Pdelay response */
                        prPdelMachine->rTxDataResp.rTs.u64TimeStampS = prPdelMachine->rPdReqMsgRx.rT2Ts.u64TimeStampS;
                        prPdelMachine->rTxDataResp.rTs.u32TimeStampNs = prPdelMachine->rPdReqMsgRx.rT2Ts.u32TimeStampNs;
                        /* Start turnaround time measurement */ 
                        prPdelMachine->u64TurnaroundMeasStartPtpNsResp = GPTP_INTERNAL_TsToU64(GPTP_PORT_CurrentTimeGet(GPTP_DEF_TS_FREERUNNING));
                        /* Copy requesting clock Id and requesting port Id from received Pdelay request message into the Pdelay response message */
                        prPdelMachine->rTxDataResp.rRequesting.u64ClockId = prPdelMachine->rPdReqMsgRx.rHeader.u64SourceClockId;
                        prPdelMachine->rTxDataResp.rRequesting.u16PortId = prPdelMachine->rPdReqMsgRx.rHeader.u16SourcePortId;
                        /* Respond with subdomain number, as in the pdelay request */
                        prPdelMachine->rTxDataResp.u8SubdomainNumber = prPdelMachine->rPdReqMsgRx.u8SubdomainNum;
                        /* TX data buffer provide */
                        prPdelMachine->rTxDataResp.pau8TxBuffPtr = prPdelMachine->au8SendBuffPdelResp;
                        prPdelMachine->rTxDataResp.bTsRequested = true;
                        prPdelMachine->rTxDataResp.u16VlanTci = prPdelMachine->rPdReqMsgRx.u16VlanTci;
                        /* Use clockID and portID from device configuration */
                        prPdelMachine->rTxDataResp.rSrcPortID.u64ClockId = prGptp->prPerPortParams[prPdelMachine->u8GptpPort].u64ClockId;
                        prPdelMachine->rTxDataResp.rSrcPortID.u16PortId = (uint16_t)prPdelMachine->u8GptpPort + 1u;
                        /* Set correct destination MAC address. */
                        if (prPdelMachine->bPdelayUnicastResp)
                        {
                            prPdelMachine->rTxDataResp.u64DestMac = prPdelMachine->u64SourceMac;
                        }
                        else
                        {
                            prPdelMachine->rTxDataResp.u64DestMac = (uint64_t)GPTP_DEF_ETH_PTP_MAC_MULTICAST;
                        }

                        /* Transmit message, Timestamp required */
                        GPTP_MsgTransmit(&prPdelMachine->rTxDataResp, prPdelMachine->u8GptpPort, prPdelMachine->u8GptpPort, 0u);

                        /* Change Pdelay responder state to waiting for timestamp */
                        prPdelMachine->ePdelayResponderState = GPTP_DEF_SENT_PD_RESP_WTING_4_T;
                    break;

                    default:
                    break;
                }
            }
        break;

        /* The function has been called upon the TX timestamp generation */
        case GPTP_DEF_CALL_ON_TIMESTAMP:
            /* Switch Pdelay response state */
            switch (prPdelMachine->ePdelayResponderState)
            {
                /* If the Pdelay responder state is waiting for timestamp */
                case GPTP_DEF_SENT_PD_RESP_WTING_4_T:
                    /* If the T3 timestamp is registred */
                    if (true == prPdelMachine->bT3TsRegistred)
                    {
                        /* If the sequence Id from the frame Id matches with sequence Id of the message previously sent */
                        if (prPdelMachine->u16T3TsSeqId == prPdelMachine->u16RespSeqId)
                        {
                            /* Clear T3 timestamp registred flag */
                            prPdelMachine->bT3TsRegistred = false;

                            /* Message type - Pdelay response follow up */
                            prPdelMachine->rTxDataReFu.eMsgId = GPTP_DEF_MSG_TYPE_PD_RESP_FUP;
                            /* Copy the Sequence Id from the received Pdelay request message into the Pdelay response follow up */
                            prPdelMachine->rTxDataReFu.u16SequenceId = prPdelMachine->rPdReqMsgRx.rHeader.u16SequenceId;

                            /* Copy the T3 timestamp (transmition of the Pdelay response message) to the Pdelay response follow up */
                            prPdelMachine->rTxDataReFu.rTs.u64TimeStampS = prPdelMachine->rT3Ts.u64TimeStampS;
                            prPdelMachine->rTxDataReFu.rTs.u32TimeStampNs = prPdelMachine->rT3Ts.u32TimeStampNs;

                            /* Calculate responder turnaround time */
                            u64Turnaroud = GPTP_INTERNAL_TsToU64(prPdelMachine->rT3Ts) - GPTP_INTERNAL_TsToU64(prPdelMachine->rPdReqMsgRx.rT2Ts);

                            /* Check if turneround time measurement is still active */
                            if (0u != prPdelMachine->u64TurnaroundMeasStartPtpNsResp)
                            {
                                /* Check the turnaround time of the pdelay responder */
                                if (GPTP_DEF_10_MS_IN_NS < u64Turnaroud)
                                {
                                    /* Report turnaround time error */
                                    GPTP_ERR_Register(prPdelMachine->u8GptpPort, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_L_TOO_LONG_TURN_RESP, \
                                    prPdelMachine->rPdRespFupMsgRx.rHeader.u16SequenceId);
                                }
                            }
                            /* Cancel turnaround time measurement */
                            prPdelMachine->u64TurnaroundMeasStartPtpNsResp = 0u;

                            /* Copy requesting clock Id and requesting port Id from received Pdelay request message into the Pdelay response follow up message */
                            prPdelMachine->rTxDataReFu.rRequesting.u64ClockId = prPdelMachine->rPdReqMsgRx.rHeader.u64SourceClockId;
                            prPdelMachine->rTxDataReFu.rRequesting.u16PortId = prPdelMachine->rPdReqMsgRx.rHeader.u16SourcePortId;
                            /* Respond with subdomain number, as in the pdelay request */
                            prPdelMachine->rTxDataReFu.u8SubdomainNumber = prPdelMachine->rPdReqMsgRx.u8SubdomainNum;
                            /* Provide TX buffer pointer */
                            prPdelMachine->rTxDataReFu.pau8TxBuffPtr = prPdelMachine->au8SendBuffPdelRespFup;
                            prPdelMachine->rTxDataReFu.bTsRequested = false;
                            prPdelMachine->rTxDataReFu.u16VlanTci = prPdelMachine->rPdReqMsgRx.u16VlanTci;
                            /* Copy clockID and portID from last PD response */
                            prPdelMachine->rTxDataReFu.rSrcPortID.u64ClockId = prPdelMachine->rTxDataResp.rSrcPortID.u64ClockId;
                            prPdelMachine->rTxDataReFu.rSrcPortID.u16PortId = prPdelMachine->rTxDataResp.rSrcPortID.u16PortId;
                            /* Set correct destination MAC address. */
                            if (prPdelMachine->bPdelayUnicastResp)
                            {
                                prPdelMachine->rTxDataReFu.u64DestMac = prPdelMachine->u64SourceMac;
                            }
                            else
                            {
                                prPdelMachine->rTxDataReFu.u64DestMac = (uint64_t)GPTP_DEF_ETH_PTP_MAC_MULTICAST;
                            }

                            /* Transmit message, Timestamp not required */
                            GPTP_MsgTransmit(&prPdelMachine->rTxDataReFu, prPdelMachine->u8GptpPort, prPdelMachine->u8GptpPort, 0u);

#ifdef GPTP_TIME_VALIDATION
                            if (NULL != prGptp->rPtpStackCallBacks.pfSetPdelayResponderData)
                            {
                                rResponderNotifData.u16SequenceId = prPdelMachine->rPdReqMsgRx.rHeader.u16SequenceId;
                                rResponderNotifData.rRequestPortId.u64ClockId = prPdelMachine->rPdReqMsgRx.rHeader.u64SourceClockId;
                                rResponderNotifData.rRequestPortId.u16PortId = prPdelMachine->rPdReqMsgRx.rHeader.u16SourcePortId + 1u;
                                rResponderNotifData.rResponsePortId.u64ClockId = prGptp->prPerPortParams[(prPdelMachine->u8GptpPort)].u64ClockId;
                                rResponderNotifData.rResponsePortId.u16PortId = (((uint16_t)prPdelMachine->u8GptpPort) + 1u);
                                rResponderNotifData.rRequestReceiptTimestamp = prPdelMachine->rPdReqMsgRx.rT2Ts;
                                rResponderNotifData.rResponseOriginTimestamp = prPdelMachine->rT3Ts;
                                rResponderNotifData.rReferenceLocalTimestampInit = prPdelMachine->rReferenceLocalTimestampResp;
                                rResponderNotifData.rReferenceGlobalTimestampInit = prPdelMachine->rReferenceGlobalTimestampResp;

                                prGptp->rPtpStackCallBacks.pfSetPdelayResponderData(rResponderNotifData);
                            }
                            else
                            {
                                GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_F_NULL_PTR, GPTP_ERR_SEQ_ID_NOT_SPECIF);
                            }
#endif /* GPTP_TIME_VALIDATION */

                            /* Change Pdelay responder state to waiting for the Pdelay request */
                            prPdelMachine->ePdelayResponderState = GPTP_DEF_WAITING_FOR_PDELAY_REQ;
                        }

                        /* If the sequence Id from the frame Id does not match with sequence Id of the message previously sent */
                        else
                        {
                            GPTP_ERR_Register(prPdelMachine->u8GptpPort, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_M_IDENTIFIERS_MISMATCH, \
                                              prPdelMachine->rPdReqMsgRx.rHeader.u16SequenceId);
                        }
                    }

                    /* If the T3 timestamp is not registred */
                    else
                    {
                        GPTP_ERR_Register(prPdelMachine->u8GptpPort, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_T_TS_NOT_REGISTERED, \
                                          prPdelMachine->rPdReqMsgRx.rHeader.u16SequenceId);
                    }
                break;

                default:
                break;
            }

        break;

        default:
        break;
    }
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
