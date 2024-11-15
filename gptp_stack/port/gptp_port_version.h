/*
 * Copyright 2023-2024 NXP
 *
 * NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be
 * used strictly in accordance with the applicable license terms. By expressly
 * accepting such terms or by downloading, installing, activating and/or otherwise
 * using the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms.  If you do not agree to be
 * bound by the applicable license terms, then you may not retain, install,
 * activate or otherwise use the software.
 */

/*******************************************************************************
 * Guard Begin
 ******************************************************************************/

#ifndef GPTP_PORT_VERSION_H
#define GPTP_PORT_VERSION_H

/*******************************************************************************
 * Includes
 ******************************************************************************/

#ifdef GPTP_USE_PRINTF
    #include <stdio.h>
    #define GPTP_PORT_PRINTF(x) (void)printf x
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Integer to string converter. */
#define GPTP_DEF_STR_HELPER(x)          #x
/* Integer to string converter. */
#define GPTP_DEF_STR(x)                 GPTP_DEF_STR_HELPER(x)

/* Manufacturer name. */
#define GPTP_DEF_MANUFACTURER_NAME      "NXP"
/* Manufacturer identity. */
#define GPTP_DEF_MANUFACTURER_IDENTITY  {0x00,0x60,0x37}

/* Product details. */
#ifndef GPTP_PORT_MODEL_NUMBER
#define GPTP_PORT_MODEL_NUMBER          "S32K3XX"
#endif /* GPTP_PORT_MODEL_NUMBER */

#ifndef GPTP_PORT_HW_VERSION
#define GPTP_PORT_HW_VERSION            "S32K3XX"
#endif /* GPTP_PORT_HW_VERSION */

#ifndef GPTP_PORT_FW_VERSION
#define GPTP_PORT_FW_VERSION            "Undefined"
#endif /* GPTP_PORT_FW_VERSION */

#ifndef GPTP_BASIC_VERSION
#define GPTP_PORT_PRODUCT_DESCRIPTION   GPTP_DEF_MANUFACTURER_NAME";"GPTP_PORT_MODEL_NUMBER" gPTP Stack Premium Version;"
#else /* GPTP_BASIC_VERSION */
#define GPTP_PORT_PRODUCT_DESCRIPTION   GPTP_DEF_MANUFACTURER_NAME";"GPTP_PORT_MODEL_NUMBER" gPTP Stack Basic Version;"
#endif /* GPTP_BASIC_VERSION */

/* Major. */
#define GPTP_DEF_REV_MAJOR              0
/* Minor. */
#define GPTP_DEF_REV_MINOR              9
/* Patch. */
#define GPTP_DEF_REV_PATCH              0
/* gPTP revision string. */
#define GPTP_DEF_GPTP_REV               GPTP_DEF_STR(GPTP_DEF_REV_MAJOR)"."GPTP_DEF_STR(GPTP_DEF_REV_MINOR)"."GPTP_DEF_STR(GPTP_DEF_REV_PATCH)

/* gPTP product version string. */
#define GPTP_PORT_PRODUCT_REVISION      GPTP_PORT_HW_VERSION";"GPTP_PORT_FW_VERSION";"GPTP_DEF_GPTP_REV

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
/* None */

/*******************************************************************************
 * Guard End
 ******************************************************************************/

#endif /* GPTP_PORT_VERSION_H */

/*******************************************************************************
 * EOF
 ******************************************************************************/
