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

/*!
 * @file gptp_timer.h
 *
 */

#ifndef GPTP_TIMER
    #define GPTP_TIMER

/*******************************************************************************
 * API
 ******************************************************************************/

/*!
 * @brief The function registers last received sync message.
 *
 * @param[in] Pointer to general gPTP data structure.
 * @param[in] Pointer to the respective domain data structure.
 */
void GPTP_TIMER_LastValidSyncReg(gptp_def_domain_t *prDomain);

/*!
 * @brief The function registers last sync sent.
 *
 * @param[in] Pointer to the respective sync data structure.
 */
void GPTP_TIMER_LastSyncSendRegister(gptp_def_sync_t *prSync);

/*!
 * @brief The function updates nex Pdelay request time.
 *
 * @param[in] Pointer to the respective Pdelay data structure.
 */
void GPTP_TIMER_PdelayDueUpdate(gptp_def_pdelay_t *prPdelay);

/*!
 * @brief The function schedules syncs for non GM domain.
 *
 * @param[in] Pointer to general gPTP data structure.
 * @param[in] Pointer to the respective domain structure.
 * @param[in] Call type
 */
void GPTP_TIMER_SyncsSendNonGm(gptp_def_data_t *prGptp,
                               gptp_def_domain_t *prDomain,
                               const gptp_def_sm_call_type_t rCallType);

/*!
 * @brief The function checks internal timers periodically.
 *
 * @param[in] Pointer to general gPTP data structure.
 */
void GPTP_TIMER_TimerPeriodic(gptp_def_data_t *prGptp);

/*!
 * @brief The function initializes internal timers.
 *
 * @param[in] Pointer to general gPTP data structure.
 */
void GPTP_TIMER_TimerInit(gptp_def_data_t *prGptp);

#endif /* GPTP_TIMER */
/*******************************************************************************
 * EOF
 ******************************************************************************/
