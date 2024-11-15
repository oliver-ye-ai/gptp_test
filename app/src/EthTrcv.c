/** @} */
/*==================================================================================================
*   Project            : RTD AUTOSAR 4.7
*   Platform           : M4_SRC_ETHTRCV_FAMILY
*   Peripheral         : STUB
*   Dependencies       : 
*
*   AutosarVersion     : 4.7.0
*   AutosarRevision    : ASR_REL_4_7_REV_0000
*   AutosarConfVariant :
*   SW Version         : 4.0.0
*   BuildVersion       : S32K3_RTD_4_0_0_P14_D2403_ASR_REL_4_7_REV_0000_20240328
*
*   Copyright 2020-2024 NXP
*
*   NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be
*   used strictly in accordance with the applicable license terms. By expressly
*   accepting such terms or by downloading, installing, activating and/or otherwise
*   using the software, you are agreeing that you have read, and that you agree to
*   comply with and are bound by, such license terms.  If you do not agree to be
*   bound by the applicable license terms, then you may not retain, install,
*   activate or otherwise use the software.
==================================================================================================*/

/**
*   @file             EthTrcv.c
*
*   @addtogroup       ETHTRCV_DRIVER Ethernet Transceiver Driver
*   @{
*/

/*==================================================================================================
*                                        INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
#include "EthTrcv.h"
#include "device.h"

/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/

#define ETHTRCV_VENDOR_ID_C                      43
#define ETHTRCV_AR_RELEASE_MAJOR_VERSION_C       4
#define ETHTRCV_AR_RELEASE_MINOR_VERSION_C       7
#define ETHTRCV_AR_RELEASE_REVISION_VERSION_C    0
#define ETHTRCV_SW_MAJOR_VERSION_C               4
#define ETHTRCV_SW_MINOR_VERSION_C               0
#define ETHTRCV_SW_PATCH_VERSION_C               0

/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/

/*==================================================================================================
*                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
*                                       LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
*                                      LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                      LOCAL VARIABLES
==================================================================================================*/

/*==================================================================================================
*                                      GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                      GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
*                                       LOCAL FUNCTIONS
==================================================================================================*/



/*==================================================================================================
*                                       GLOBAL FUNCTIONS
==================================================================================================*/ 
/**
* @brief          This function handles the received Ethernet frame.
* @details        Function should parse the received frame and pass the gathered
*                 information to the appropriate upper layer module.
* @param[in]      CtrlIdx Index of the controller which read Mii.
* @param[in]      TrcvIdx The Id of transceiver was read Mii
* @param[in]      RegIdx The Id or Address of Register was read
* @param[in]      RegVal The value of register.
*
* @implements EthTrcv_ReadMiiIndication_Activity
*
* @violates @ref EthTrcv_c_REF_3 Violates MISRA 2004 Required Rule 8.10 This is an interrupt handler
*/
void EthTrcv_ReadMiiIndication(uint8 CtrlIdx,
                               uint8 TrcvIdx,
                               uint8 RegIdx,
                               uint16 RegVal)
{
    /* This is an empty stub function. */
    (void)CtrlIdx;
    (void)TrcvIdx;
    (void)RegIdx;
    (void)RegVal;
}

/*================================================================================================*/
/**
* @brief          This function handles the received Ethernet frame.
* @details        Function should parse the received frame and pass the gathered
*                 information to the appropriate upper layer module.
* @param[in]      CtrlIdx Index of the controller which write Mii.
* @param[in]      TrcvIdx The Id of transceiver was write Mii
* @param[in]      RegIdx The Id or Address of Register was write
*
* @implements EthTrcv_WriteMiiIndication_Activity
*
* @violates @ref EthTrcv_c_REF_3 Violates MISRA 2004 Required Rule 8.10 This is an interrupt handler
*/
void EthTrcv_WriteMiiIndication(uint8 CtrlIdx,
                                uint8 TrcvIdx,
                                uint8 RegIdx)
{
    /* This is an empty stub function. */
    (void)CtrlIdx;
    (void)TrcvIdx;
    (void)RegIdx;
}

/**
* @brief          This function obtains link state of Ethernet transceiver.
* @details        Function should read the current transceiver link state.
* @param[in]      TrcvIdx The Id of transceiver to get link.
* @param[out]     LinkStatePtr Pointer to variable to store link state.
*/
Std_ReturnType EthTrcv_GetLinkState(uint8 TrcvIdx,
                                    EthTrcv_LinkStateType *LinkStatePtr)
{
    (void)TrcvIdx;
    Std_ReturnType eRetVal;

    /* Check for LinkStatePtr pointer validity. */
    if (NULL != LinkStatePtr)
    {
        /* Check PHY link state obtained by MDIO. */
        if (get_device_link_status())
        {
            /* Link is up. */
            *LinkStatePtr = ETHTRCV_LINK_STATE_ACTIVE;
        }
        else
        {
            /* Link is down. */
            *LinkStatePtr = ETHTRCV_LINK_STATE_DOWN;
        }
        eRetVal = (Std_ReturnType)E_OK;
    }
    else
    {
        /* Invalid LinkStatePtr. */
        eRetVal = (Std_ReturnType)E_NOT_OK;
    }

    return eRetVal;
}

/** @} */
