/*==================================================================================================
*   Project              : RTD AUTOSAR 4.7
*   Platform             : CORTEXM
*   Peripheral           : GMAC
*   Dependencies         : none
*
*   Autosar Version      : 4.7.0
*   Autosar Revision     : ASR_REL_4_7_REV_0000
*   Autosar Conf.Variant :
*   SW Version           : 4.0.0
*   Build Version        : S32K3_RTD_4_0_0_P14_D2403_ASR_REL_4_7_REV_0000_20240328
*
*   Copyright 2020 - 2024 NXP
*
*   NXP Confidential. This software is owned or controlled by NXP and may only be
*   used strictly in accordance with the applicable license terms. By expressly
*   accepting such terms or by downloading, installing, activating and/or otherwise
*   using the software, you are agreeing that you have read, and that you agree to
*   comply with and are bound by, such license terms. If you do not agree to be
*   bound by the applicable license terms, then you may not retain, install,
*   activate or otherwise use the software.
==================================================================================================*/

#ifndef ETH_43_GMAC_PBCFG_H
#define ETH_43_GMAC_PBCFG_H

/**
*   @file
*
*   @addtogroup ETH_43_GMAC_DRIVER_CONFIGURATION Ethernet Driver Configuration
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
#define ETH_43_GMAC_PBCFG_VENDOR_ID                     43
#define ETH_43_GMAC_PBCFG_AR_RELEASE_MAJOR_VERSION      4
#define ETH_43_GMAC_PBCFG_AR_RELEASE_MINOR_VERSION      7
#define ETH_43_GMAC_PBCFG_AR_RELEASE_REVISION_VERSION   0
#define ETH_43_GMAC_PBCFG_SW_MAJOR_VERSION              4
#define ETH_43_GMAC_PBCFG_SW_MINOR_VERSION              0
#define ETH_43_GMAC_PBCFG_SW_PATCH_VERSION              0

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
*                                      DEFINES AND MACROS
==================================================================================================*/






#ifndef EthConf_EthCtrlConfig_EthCtrlConfig_0
    /*! @brief Controller symbolic name to be passed to API functions that require CtrlIdx */
    #define EthConf_EthCtrlConfig_EthCtrlConfig_0    (0U)
#elif (EthConf_EthCtrlConfig_EthCtrlConfig_0 != 0)
    #error "[TPS_ECUC_06074] Invalid configuration due to symbolic name values"
#endif

#ifndef EthConf_EthCtrlConfigIngressFifo_EthCtrlConfigIngressFifo_0
    /*! @brief Controller symbolic name to be passed to API functions that require FifoIdx */
    #define EthConf_EthCtrlConfigIngressFifo_EthCtrlConfigIngressFifo_0        (0U)
#elif (EthConf_EthCtrlConfigIngressFifo_EthCtrlConfigIngressFifo_0 != 0)
    #error "[TPS_ECUC_06074] Invalid configuration due to symbolic name values"
#endif

/* Maximum number of configured Tx FIFOs */
#ifndef ETH_43_GMAC_MAX_TXFIFO_SUPPORTED
    /*! @brief Maximum number of configured Tx FIFOs */
    #define ETH_43_GMAC_MAX_TXFIFO_SUPPORTED        (1U)
#elif (ETH_43_GMAC_MAX_TXFIFO_SUPPORTED < 1)
    #undef ETH_43_GMAC_MAX_TXFIFO_SUPPORTED
    #define ETH_43_GMAC_MAX_TXFIFO_SUPPORTED        (1U) 
#endif

/* Maximum number of configured Rx FIFOs */
#ifndef ETH_43_GMAC_MAX_RXFIFO_SUPPORTED
    #define ETH_43_GMAC_MAX_RXFIFO_SUPPORTED        (1U)
#elif (ETH_43_GMAC_MAX_RXFIFO_SUPPORTED < 1)
    #undef ETH_43_GMAC_MAX_RXFIFO_SUPPORTED
    #define ETH_43_GMAC_MAX_RXFIFO_SUPPORTED        (1U) 
#endif

/* Maximum number of configured buffers per Tx FIFO */
#ifndef ETH_43_GMAC_MAX_TXBUFF_SUPPORTED
    #define ETH_43_GMAC_MAX_TXBUFF_SUPPORTED        (6U)
#elif (ETH_43_GMAC_MAX_TXBUFF_SUPPORTED < 6)
    #undef ETH_43_GMAC_MAX_TXBUFF_SUPPORTED
    #define ETH_43_GMAC_MAX_TXBUFF_SUPPORTED        (6U) 
#endif

/* Maximum number of configured buffers per Rx FIFO */
#ifndef ETH_43_GMAC_MAX_RXBUFF_SUPPORTED
    #define ETH_43_GMAC_MAX_RXBUFF_SUPPORTED        (6U)
#elif (ETH_43_GMAC_MAX_RXBUFF_SUPPORTED < 6)
    #undef ETH_43_GMAC_MAX_RXBUFF_SUPPORTED
    #define ETH_43_GMAC_MAX_RXBUFF_SUPPORTED        (6U) 
#endif

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

#endif /* ETH_43_GMAC_PBCFG_H */

