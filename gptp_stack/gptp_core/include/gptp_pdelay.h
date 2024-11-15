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
 * @file gptp_pdelay.h
 *
 */
#ifndef GPTP_PDELAY
    #define GPTP_PDELAY

#include "gptp_def.h"

/*******************************************************************************
 * API
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
 */
void GPTP_PDELAY_PdelayMachine(const gptp_def_data_t *prGptp,
                               gptp_def_pdelay_t *prPdelMachine,
                               const gptp_def_sm_call_type_t eCallType);

#endif /* GPTP_PDELAY */
/*******************************************************************************
 * EOF
 ******************************************************************************/
