/*
 * Copyright 2023 NXP
 *
 * NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be
 * used strictly in accordance with the applicable license terms. By expressly
 * accepting such terms or by downloading, installing, activating and/or otherwise
 * using the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms.  If you do not agree to be
 * bound by the applicable license terms, then you may not retain, install,
 * activate or otherwise use the software.
 */

#ifndef GPTP_PI
    #define GPTP_PI

#include "gptp_def.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*!
 * @brief GPTP PI Controller context structore.
 */
typedef struct
{
    /*! Current error value (input value of PI Controller) - int32_t */
    int32_t s32error;
    /*! Current calculated proportional value - int32_t */
    int32_t s32proportional;
    /*! Current calculated integrational value - int32_t */
    int32_t s32integrational;
    /*! Current calculated output value of PI Controller - int32_t */
    int32_t s32calculated;
    /*! Current calculated proportional gain of PI Controller - float64_t */
    float64_t f64Kp;
    /*! Current calculated integrational gain of PI Controller - float64_t */
    float64_t f64Ki;
    /*! PI Controller configuration structure - const pointer to struct */
    const gptp_def_pi_controller_conf_t *prConfiguration;
}gptp_pi_controller_t;

/*******************************************************************************
 * API
 ******************************************************************************/

/*!
 *
 * @brief   This function initializes PI Controller.
 *
 * @details This function initializes PI Controller and cleans all variables.
 *
 * @param[in] prConfiguration Pointer to the PI controller configuration structure.
 */
void GPTP_PI_Init(const gptp_def_pi_controller_conf_t *prConfiguration);

/*!
 *
 * @brief   This function cleans the integrational part of PI Controller.
 * 
 * @details This function cleans the integrational part of PI Controller, which also contains historical data.
 * 
 */
void GPTP_PI_Clear(void);

/*!
 *
 * @brief   This function calculates correction value as PPB to reach the setpoint.
 *
 * @details This function calculates an error value as the difference between setpoint and time offset
 *          then correction based on proportional and integral terms of controller is applied.
 *          
 * 
 * @param[in] s8SyncInterval Current Sync interval.
 * @param[in] s32ErrorNs Current error value (difference between setpoint and measurement) [ns].
 * @return New correction value [PPB].
 */
int32_t GPTP_PI_Update(int8_t s8SyncInterval,
                       int32_t s32ErrorNs);

#endif /* GPTP_PI */
/*******************************************************************************
 * EOF
 ******************************************************************************/
