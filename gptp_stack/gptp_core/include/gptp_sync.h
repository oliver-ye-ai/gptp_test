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
 * @file gptp_sync.h
 *
 */
#ifndef GPTP_SYNC
    #define GPTP_SYNC

#include "gptp_def.h"

/*******************************************************************************
 * API
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
 */
void GPTP_SYNC_SyncMachine(gptp_def_data_t *prGptp,
                           const uint8_t u8Domain,
                           const uint8_t u8Machine,
                           const gptp_def_sm_call_type_t eCallType);

#endif /* GPTP_SYNC */
/*******************************************************************************
 * EOF
 ******************************************************************************/
