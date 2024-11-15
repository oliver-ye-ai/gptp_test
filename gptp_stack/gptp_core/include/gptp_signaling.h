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
 * @file gptp_signaling.h
 *
 */
#ifndef GPTP_SIGNALING
    #define GPTP_SIGNALING

#include "gptp_def.h"

/*******************************************************************************
 * API
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
 */
void GPTP_SIGNALING_RxSignaling(const gptp_def_data_t *prGptp,
                                const uint8_t u8Domain,
                                const uint8_t u8Machine);

/*!
 * @brief Send signaling message
 *
 * This function Sends Signaling message.
 *
 * @param[in] prGptp Pointer to global gPTP structure.
 * @param[in] u8Domain Domain ID.
 * @param[in] s8Interval Required Sync interval.
 *
 */
void GPTP_SIGNALING_SendSignaling(const gptp_def_data_t *prGptp,
                                  const uint8_t u8Domain,
                                  const int8_t s8Interval);

#endif /* GPTP_SIGNALING */
/*******************************************************************************
 * EOF
 ******************************************************************************/
