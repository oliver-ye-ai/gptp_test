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

#include "gptp.h"
#include "gptp_pi.h"
#include "gptp_err.h"
#include "gptp_internal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Threshold of narrow band */
#define PI_NARROW_BAND_THRESHOLD              100.0
/* PI ratio */
#define PI_PI_RATIO                           3.141592

/*******************************************************************************
 * Variables
 ******************************************************************************/
static gptp_pi_controller_t rGptpPi =
{
    .s32proportional = 0,
    .s32integrational = 0,
    .s32calculated = 0,
    .prConfiguration = NULL,
};

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 *
 * @brief   This function initializes PI Controller.
 *
 * @details This function initializes PI Controller and cleans all variables.
 *
 * @param[in] prConfiguration Pointer to the PI controller configuration structure.
 *
 * @ requirements 529199
 * @ requirements 539904
 */
void GPTP_PI_Init(const gptp_def_pi_controller_conf_t *prConfiguration)
{
    rGptpPi.prConfiguration = prConfiguration;
    GPTP_PI_Clear();
}

/*!
 *
 * @brief   This function initializes PI Controller.
 *
 * @details This function initializes PI Controller and cleans all variables.
 *
 * @ requirements 529199
 * @ requirements 539905
 */
void GPTP_PI_Clear(void)
{
    rGptpPi.s32proportional = 0;
    rGptpPi.s32integrational = 0;
    rGptpPi.s32calculated = 0;
    rGptpPi.s32error = 0;

    /* Reset sync lock and report to the app. */
    GPTP_INTERNAL_ResetSyncLock(GPTP_GetMainStrPtr());
}

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
 *
 * @ requirements 529199
 * @ requirements 539906
 */
int32_t GPTP_PI_Update(int8_t s8SyncInterval,
                       int32_t s32ErrorNs)
{
    /* Period of the synchronization interval [s] */
    float64_t      f64Dt;
    /* Natural frequency of the system [rad*s-1] */
    float64_t      f64W = 1.0;
    /* Absolute error value */
    const uint32_t u32absError = (s32ErrorNs < 0) ? (uint32_t)-s32ErrorNs : (uint32_t)s32ErrorNs;

    if (NULL != rGptpPi.prConfiguration)
    {
        /* Calculate sampling period from the current synchronization interval index */
        f64Dt = (float64_t)GPTP_INTERNAL_Log2Ns(s8SyncInterval) / 1000000000.0;

        if (((0.0 > f64Dt) || (0.0 < f64Dt)) && ((0.0 > rGptpPi.prConfiguration->f64NatFreqRatio) || (0.0 < rGptpPi.prConfiguration->f64NatFreqRatio)))
        {
            /* Calculate natural frequency from sampling frequency(converted to angle frequency) divided by frequency ratio */
            f64W = ((2.0 * PI_PI_RATIO) / f64Dt) / rGptpPi.prConfiguration->f64NatFreqRatio;

            /* Narrow band for decreasing the oscillation in steady state of the PI controller */
            if ((bool)(u32absError < (uint32_t)PI_NARROW_BAND_THRESHOLD) && (bool)(f64W > 6.0))
            {
                /* Decreasing natural frequency also brings decreasing the gain of P and I terms */
                f64W /= 2.0;
            }
        }
        else
        {
            /* Register error -  can't divide by 0 */
            GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_A_DIV_BY_0_ATTEMPT, GPTP_ERR_SEQ_ID_NOT_SPECIF);
        }

        /* Calculate proportional gain of controller */
        rGptpPi.f64Kp = (2.0 * rGptpPi.prConfiguration->f64DampingRatio) * f64W;

        /* Calculate integrational gain of controller */
        rGptpPi.f64Ki = f64W * f64W;
        
        /* Calculate action intervention as PI controller output */
        rGptpPi.s32proportional = GPTP_INTERNAL_Float64ToInt32((float64_t)s32ErrorNs * rGptpPi.f64Kp);
        rGptpPi.s32integrational += GPTP_INTERNAL_Float64ToInt32((float64_t)s32ErrorNs * (rGptpPi.f64Ki * f64Dt));

        /* If windup limit is configured (avoids overshooting) */
        if (0U != rGptpPi.prConfiguration->u32IntegralWindupLimit)
        {
            if (rGptpPi.s32integrational > (int32_t)rGptpPi.prConfiguration->u32IntegralWindupLimit)
            {
                rGptpPi.s32integrational = (int32_t)rGptpPi.prConfiguration->u32IntegralWindupLimit;
            }
            else if (rGptpPi.s32integrational < (-((int32_t)rGptpPi.prConfiguration->u32IntegralWindupLimit)))
            {
                rGptpPi.s32integrational = (-((int32_t)rGptpPi.prConfiguration->u32IntegralWindupLimit));
            }
        }

        rGptpPi.s32calculated = rGptpPi.s32proportional + rGptpPi.s32integrational;

        /* Store current error */
        rGptpPi.s32error = s32ErrorNs;
    }
    else
    {
        /* Register error -  missing configuration */
        GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_PI_INCORRECT_CFG, GPTP_ERR_SEQ_ID_NOT_SPECIF);
    }

    /* Return calculated value in PPB format */
    return rGptpPi.s32calculated;
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
