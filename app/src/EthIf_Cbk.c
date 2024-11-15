/*==================================================================================================
*   Project              : RTD AUTOSAR 4.7
*   Platform             : CORTEXM
*   Peripheral           : 
*   Dependencies         : 
*
*   Autosar Version      : 4.7.0
*   Autosar Revision     : ASR_REL_4_7_REV_0000
*   Autosar Conf.Variant :
*   SW Version           : 4.0.0
*   Build Version        : S32K3_RTD_4_0_0_P14_D2403_ASR_REL_4_7_REV_0000_20240328
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
*   @file
*
*   @addtogroup ETHIF Ethernet Interface
*   @{
*/

/*==================================================================================================
*                                        INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
#include "ComStackTypes.h"
#include "EthIf_Cbk.h"
#include "gptp_port_platform.h"
#include "gptp_frame.h"

/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/

#define ETHIF_VENDOR_ID_C                      43
#define ETHIF_AR_RELEASE_MAJOR_VERSION_C       4
#define ETHIF_AR_RELEASE_MINOR_VERSION_C       7
#define ETHIF_AR_RELEASE_REVISION_VERSION_C    0
#define ETHIF_SW_MAJOR_VERSION_C               4
#define ETHIF_SW_MINOR_VERSION_C               0
#define ETHIF_SW_PATCH_VERSION_C               0

/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/
/* Check if current file and ETHIF header file are of the same vendor */
#if (ETHIF_VENDOR_ID_C != ETHIF_VENDOR_ID)
    #error "EthIf_Cbk.c and EthIf_Cbk.h have different vendor ids"
#endif
/* Check if current file and ETHIF header file are of the same Autosar version */
#if ((ETHIF_AR_RELEASE_MAJOR_VERSION_C    != ETHIF_AR_RELEASE_MAJOR_VERSION) || \
     (ETHIF_AR_RELEASE_MINOR_VERSION_C    != ETHIF_AR_RELEASE_MINOR_VERSION) || \
     (ETHIF_AR_RELEASE_REVISION_VERSION_C != ETHIF_AR_RELEASE_REVISION_VERSION))
    #error "AutoSar Version Numbers of EthIf_Cbk.c and EthIf_Cbk.h are different"
#endif
/* Check if current file and ETHIF header file are of the same Software version */
#if ((ETHIF_SW_MAJOR_VERSION_C != ETHIF_SW_MAJOR_VERSION) || \
     (ETHIF_SW_MINOR_VERSION_C != ETHIF_SW_MINOR_VERSION) || \
     (ETHIF_SW_PATCH_VERSION_C != ETHIF_SW_PATCH_VERSION))
    #error "Software Version Numbers of EthIf_Cbk.c and EthIf_Cbk.h are different"
#endif
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

/*================================================================================================*/
/**
* @brief          This function handles the received Ethernet frame.
* @details        Function should parse the received frame and pass the gathered
*                 information to the appropriate upper layer module.
* @note           The passed data buffer is no longer valid after the function
*                 is exited.
* @warning        This is only an empty stub function provided only to be able
*                 to compile and link the Eth module.
* @param[in]      CtrlIdx Index of the controller which received the frame.
* @param[in]      FrameType The received frame Ethertype (from the frame header)
* @param[in]      IsBroadcast The value TRUE indicates that the received frame
*                 was sent to broadcast address (ff-ff-ff-ff-ff-ff)
* @param[in]      PhysAddrPtr Pointer to received frame source MAC address
*                 (6 bytes).
* @param[in]      DataPtr Data buffer containing the received Ethernet frame 
*                 payload.
* @param[in]      LenByte Length of the data in the buffer DataPtr.
*
*/
void EthIf_RxIndication(uint8 CtrlIdx,
                        Eth_FrameType FrameType,
                        boolean IsBroadcast,
                        const uint8* PhysAddrPtr,
                        const Eth_DataType* DataPtr,
                        uint16 LenByte)
{
    /* If gPTP frame is received, pass it to the gPTP stack. */
    if (GPTP_FR_ETH_TYPE_PTP == FrameType) 
    {
        GPTP_PORT_RxIndication(CtrlIdx, FrameType, IsBroadcast, PhysAddrPtr,
                               DataPtr, LenByte);
    }

    /* User can process other non-gPTP frames here. */
}

/*================================================================================================*/
/**
* @brief          This function confirms that transmission of an Ethernet frame
*                 was finished.
* @details        Function should notify the appropriate upper layer module that
*                 the data transmission was successfully finished.
* @warning        This is only an empty stub function provided only to be able
*                 to compile and link the Eth module.
* @param[in]      CtrlIdx Index of the controller which transmitted the frame.
* @param[in]      BufIdx Index of the transmitted data buffer.

*/
void EthIf_TxConfirmation(uint8 CtrlIdx,
                          Eth_BufIdxType BufIdx,
                          Std_ReturnType Result)
{
    GPTP_PORT_TxConfirmation(CtrlIdx, BufIdx, Result);
}

/*================================================================================================*/
/**
* @brief          This function indicate that driver mode has been changed
* @details        Called asynchronously when mode has been read out. Triggered by previous 
*                 Eth_SetControllerMode call. Can directly be called within the trigger functions.
* @warning        This is only an empty stub function provided only to be able
*                 to compile and link the Eth module.
* @param[in]      CtrlIdx Index of the controller which mode has been changed.
* @param[in]      CtrlMode New mode of correspond Eth driver.

*/
void EthIf_CtrlModeIndication(uint8 CtrlIdx,
                              Eth_ModeType CtrlMode)
{
    /* This is an empty stub function. */ 
    (void)CtrlIdx;
    (void)CtrlMode;
}


/*================================================================================================*/
/**
* @brief          This function indicates that an ingress timestamp was captured
* @details        Returns an ingress timestamp value out of the Switch.
*                 If the HW resolution is lower than the Eth_TimeStampType resolution resp. range,
*                 than the remaining bits will be filled with 0.
* @warning        This is only an empty stub function provided only to be able
*                 to compile and link the EthSwt module.
* @param[in]      CtrlIdx       Index of the controller which mode has been changed.
* @param[in]      MgmtInfoPtr   Management information
* @param[in]      timeStampPtr  Current timestamp
* @param[out]     DataPtr       Ethernet data pointer
*/
void EthIf_SwitchIngressTimeStampIndication(uint8 CtrlIdx,
                                            Eth_DataType* DataPtr,
                                            EthSwt_MgmtInfoType* MgmtInfoPtr,
                                            Eth_TimeStampType* timeStampPtr)
{
    /* This is an empty stub function. */
    (void)CtrlIdx;
    (void)DataPtr;
    (void)MgmtInfoPtr;
    (void)timeStampPtr;
}

/**
* @brief          This function indicates that an egress timestamp was captured
* @details        Returns an egress timestamp value out of the Switch.
*                 If the HW resolution is lower than the Eth_TimeStampType resolution resp. range,
*                 than the remaining bits will be filled with 0.
* @warning        This is only an empty stub function provided only to be able
*                 to compile and link the EthSwt module.
* @param[in]      CtrlIdx       Index of the controller which mode has been changed.
* @param[intout]  DataPtr       Ethernet data pointer
* @param[out]     MgmtInfoPtr   Management information
* @param[out]     timeStampPtr  Current timestamp
*/
void EthIf_SwitchEgressTimeStampIndication(uint8 CtrlIdx,
                                           Eth_DataType* DataPtr,
                                           EthSwt_MgmtInfoType* MgmtInfoPtr,
                                           Eth_TimeStampType* timeStampPtr)
{
    /* This is an empty stub function. */
    (void)CtrlIdx;
    (void)DataPtr;
    (void)MgmtInfoPtr;
    (void)timeStampPtr;
}

/**
* @brief          This function indicates that mgmt information was received
* @details        Ingress Switch management info indication redirected call to upper layers who registered for the call.
* @warning        This is only an empty stub function provided only to be able
*                 to compile and link the EthSwt module.
* @param[in]      CtrlIdx       Index of the controller which mode has been changed.
* @param[intout]  DataPtr       Ethernet data pointer
* @param[out]     MgmtInfoPtr   Management information
*/
void EthIf_SwitchMgmtInfoIndication(uint8 CtrlIdx,
                                    Eth_DataType* DataPtr,
                                    EthSwt_MgmtInfoType* MgmtInfoPtr)
{
    /* This is an empty stub function. */ 
    (void)CtrlIdx;
    (void)DataPtr;
    (void)MgmtInfoPtr;
}

/**
* @brief          This function indicates that a transceiver's mode was changed
* @details        Called asynchronously when a mode change has been read out. If the function is triggered by previous call of
*                 EthTrcv_SetTransceiverMode it can directly be called within the trigger function.
* @warning        This is only an empty stub function provided only to be able
*                 to compile and link the EthSwt module.
* @param[in]      TrcvIdx       Index of the Ethernet transceiver within the context of the Ethernet Interface
* @param[in]      TrcvMode      Notified Ethernet transceiver mode
*/
void EthIf_TrcvModeIndication(uint8 TrcvIdx,
                              EthTrcv_ModeType TrcvMode)
{
    /* This is an empty stub function. */ 
    (void)TrcvIdx;
    (void)TrcvMode;
}

/**
* @brief          This function indicates that a switch port mode was changed
* @details        The EthIf shall determine the expected notifications based on the EthSwtPort configuration.
*                 In case the EthSwtPort references an EthTrcv the EthIf expects a notification from the EthTrcv via API EthIf_TrcvModeIndication().
*                 Otherwise the EthIf expects a notification from the EthSwt via API EthIf_SwitchPortModeIndication().
* @warning        This is only an empty stub function provided only to be able
*                 to compile and link the EthSwt module.
* @param[in]      SwitchIdx     Index of the switch within the context of the Ethernet Switch Driver
* @param[in]      SwitchPortIdx Index of the port at the addressed switch.
* @param[in]      PortMode      Notified Ethernet Switch port mode.
*/
void EthIf_SwitchPortModeIndication(uint8 SwitchIdx,
                                    uint8 SwitchPortIdx,
                                    EthTrcv_ModeType PortMode)
{
    /* This is an empty stub function. */
    (void)SwitchIdx;
    (void)SwitchPortIdx;
    (void)PortMode;
}

/**
* @brief          This function indicates that a transceiver's link state was changed.
* @details        Function should notify the appropriate upper layer module that
*                 the link state of transceiver module has been changed.
* @param[in]      u8CtrlIdx       Index of the controller which link state has been changed.
* @param[in]      eTrcvLinkState Notified transceiver link state.
*/
void EthIf_TrcvLinkStateChg(uint8 u8CtrlIdx,
                            EthTrcv_LinkStateType eTrcvLinkState)
{
    if (ETHTRCV_LINK_STATE_ACTIVE == eTrcvLinkState)
    {
        /* Link is up, notify GPTP stack. */
        GPTP_LinkUpNotify(0u, u8CtrlIdx);
    }
    else
    {
        /* Link is down, notify GPTP stack. */
        GPTP_LinkDownNotify(0u, u8CtrlIdx);
    }
}

/** @} */
