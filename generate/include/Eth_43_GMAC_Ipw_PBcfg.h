/*==================================================================================================
*   Project              : RTD AUTOSAR 4.7
*   Platform             : CORTEXM
*   Peripheral           : GMAC
*   Dependencies         : none
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

#ifndef ETH_43_GMAC_IPW_PBCFG_H
#define ETH_43_GMAC_IPW_PBCFG_H

/**
*   @file
*
*   @addtogroup ETH_43_GMAC_DRIVER_CONFIGURATION Ethernet Driver Configurations
*   @{
*/

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                         INCLUDE FILES
 1) system and project includes
 2) needed interfaces from external units
 3) internal and external interfaces from this unit
==================================================================================================*/

/*==================================================================================================
                                SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define ETH_43_GMAC_IPW_PBCFG_VENDOR_ID                     43
#define ETH_43_GMAC_IPW_PBCFG_AR_RELEASE_MAJOR_VERSION      4
#define ETH_43_GMAC_IPW_PBCFG_AR_RELEASE_MINOR_VERSION      7
#define ETH_43_GMAC_IPW_PBCFG_AR_RELEASE_REVISION_VERSION   0
#define ETH_43_GMAC_IPW_PBCFG_SW_MAJOR_VERSION              5
#define ETH_43_GMAC_IPW_PBCFG_SW_MINOR_VERSION              0
#define ETH_43_GMAC_IPW_PBCFG_SW_PATCH_VERSION              0

/*==================================================================================================
                                      FILE VERSION CHECKS
==================================================================================================*/

/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/
#define ETH_43_GMAC_IPW_CONFIG_PB \
    extern const Eth_43_GMAC_Ipw_CtrlConfigType Eth_43_GMAC_Ipw_aCtrlConfigPB[];

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
                                       GLOBAL FUNCTIONS
==================================================================================================*/

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* ETH_43_GMAC_IPW_PBCFG_H */

