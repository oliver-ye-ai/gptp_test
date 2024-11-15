/*==================================================================================================
*   Project              : RTD AUTOSAR 4.7
*   Platform             : CORTEXM
*   Peripheral           : none
*   Dependencies         : MemAcc
*
*   Autosar Version      : 4.7.0
*   Autosar Revision     : ASR_REL_4_7_REV_0000
*   Autosar Conf.Variant :
*   SW Version           : 5.0.0
*   Build Version        : S32K3_RTD_5_0_0_D2408_ASR_REL_4_7_REV_0000_20241002
*
*   Copyright 2020 - 2024 NXP
*
*   NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be
*   used strictly in accordance with the applicable license terms. By expressly
*   accepting such terms or by downloading, installing, activating and/or otherwise
*   using the software, you are agreeing that you have read, and that you agree to
*   comply with and are bound by, such license terms. If you do not agree to be
*   bound by the applicable license terms, then you may not retain, install,
*   activate or otherwise use the software.
==================================================================================================*/

/**
*   @file MemAcc_Cfg.c
*
*   @addtogroup MEMACC
*   @{
*/

#ifdef __cplusplus
extern "C"{
#endif


/*==================================================================================================
*                                          INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
#include "MemAcc_Cfg.h"

/*==================================================================================================
*                                 SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define MEMACC_VENDOR_ID_CFG_C                      43
#define MEMACC_AR_RELEASE_MAJOR_VERSION_CFG_C       4
#define MEMACC_AR_RELEASE_MINOR_VERSION_CFG_C       7
#define MEMACC_AR_RELEASE_REVISION_VERSION_CFG_C    0
#define MEMACC_SW_MAJOR_VERSION_CFG_C               5
#define MEMACC_SW_MINOR_VERSION_CFG_C               0
#define MEMACC_SW_PATCH_VERSION_CFG_C               0

/*==================================================================================================
*                                       FILE VERSION CHECKS
==================================================================================================*/

/* Check if current file and MemAcc_Cfg.h file are of the same vendor */
#if (MEMACC_VENDOR_ID_CFG_C != MEMACC_VENDOR_ID_CFG)
    #error "MemAcc_Cfg.c and MemAcc_Cfg.h have different vendor ids"
#endif
/* Check if current file and MemAcc_Cfg.h file are of the same Autosar version */
#if ((MEMACC_AR_RELEASE_MAJOR_VERSION_CFG_C    != MEMACC_AR_RELEASE_MAJOR_VERSION_CFG) || \
     (MEMACC_AR_RELEASE_MINOR_VERSION_CFG_C    != MEMACC_AR_RELEASE_MINOR_VERSION_CFG) || \
     (MEMACC_AR_RELEASE_REVISION_VERSION_CFG_C != MEMACC_AR_RELEASE_REVISION_VERSION_CFG) \
    )
    #error "AutoSar Version Numbers of MemAcc_Cfg.c and MemAcc_Cfg.h are different"
#endif
/* Check if current file and MemAcc_Cfg.h file are of the same software version */
#if ((MEMACC_SW_MAJOR_VERSION_CFG_C != MEMACC_SW_MAJOR_VERSION_CFG) || \
     (MEMACC_SW_MINOR_VERSION_CFG_C != MEMACC_SW_MINOR_VERSION_CFG) || \
     (MEMACC_SW_PATCH_VERSION_CFG_C != MEMACC_SW_PATCH_VERSION_CFG) \
    )
    #error "Software Version Numbers of MemAcc_Cfg.c and MemAcc_Cfg.h are different"
#endif

/*==================================================================================================
*                           LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
*                                          LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
*                                         LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                         LOCAL VARIABLES
==================================================================================================*/

/*==================================================================================================
*                                        GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                        GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
*                                    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
*                                         LOCAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
*                                        GLOBAL FUNCTIONS
==================================================================================================*/


#ifdef __cplusplus
}
#endif

/** @} */