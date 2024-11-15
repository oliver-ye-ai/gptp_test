/*
 *   Copyright 2023-2024 NXP
 *
 *   NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be
 *   used strictly in accordance with the applicable license terms. By expressly
 *   accepting such terms or by downloading, installing, activating and/or otherwise
 *   using the software, you are agreeing that you have read, and that you agree to
 *   comply with and are bound by, such license terms. If you do not agree to be
 *   bound by the applicable license terms, then you may not retain, install,
 *   activate or otherwise use the software.
 *
 *   This file contains sample code only. It is not part of the production code deliverables.
 */

/*******************************************************************************
 * Guard Begin
 ******************************************************************************/

#ifndef DEVICE_H
#define DEVICE_H

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "BasicTypes.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Define software breakpoint instruction. */
#define SOFTWARE_BREAKPOINT_INSTRUCTION "BKPT"

/*! @brief PIT frequency in Hz. */
#define PIT_FREQUENCY                   (40000000U)

/*! @brief PIT instance used. */
#define PIT_INST                        (0U)

/*******************************************************************************
 * Data Types
 ******************************************************************************/
/* None */

/*******************************************************************************
 * Externs
 ******************************************************************************/
/* None */

/*******************************************************************************
 * API
 ******************************************************************************/

/*!
 * @brief           This function initializes all drivers.
*/
void device_init(void);

/*!
 * @brief           This function return PHY link fail status.
*/
bool get_device_link_status(void);

/*******************************************************************************
 * Guard End
 ******************************************************************************/

#endif /* DEVICE_H */

/*******************************************************************************
 * EOF
 ******************************************************************************/
