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

#ifndef ETH_43_GMAC_CFG_H
#define ETH_43_GMAC_CFG_H

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
#include "Eth_43_GMAC_PBcfg.h"

#include "Eth_GeneralTypes.h"
#include "Mcal.h"
/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define ETH_43_GMAC_CFG_VENDOR_ID                    43
#define ETH_43_GMAC_CFG_AR_RELEASE_MAJOR_VERSION     4
#define ETH_43_GMAC_CFG_AR_RELEASE_MINOR_VERSION     7
#define ETH_43_GMAC_CFG_AR_RELEASE_REVISION_VERSION  0
#define ETH_43_GMAC_CFG_SW_MAJOR_VERSION             5
#define ETH_43_GMAC_CFG_SW_MINOR_VERSION             0
#define ETH_43_GMAC_CFG_SW_PATCH_VERSION             0

/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/
/* Checks against Eth_43_GMAC_PBcfg.h */
#if (ETH_43_GMAC_CFG_VENDOR_ID != ETH_43_GMAC_PBCFG_VENDOR_ID)
    #error "Eth_43_GMAC_Cfg.h and Eth_43_GMAC_PBcfg.h have different vendor ids"
#endif
#if ((ETH_43_GMAC_CFG_AR_RELEASE_MAJOR_VERSION    != ETH_43_GMAC_PBCFG_AR_RELEASE_MAJOR_VERSION) || \
     (ETH_43_GMAC_CFG_AR_RELEASE_MINOR_VERSION    != ETH_43_GMAC_PBCFG_AR_RELEASE_MINOR_VERSION) || \
     (ETH_43_GMAC_CFG_AR_RELEASE_REVISION_VERSION != ETH_43_GMAC_PBCFG_AR_RELEASE_REVISION_VERSION) \
    )
     #error "AUTOSAR Version Numbers of Eth_43_GMAC_Cfg.h and Eth_43_GMAC_PBcfg.h are different"
#endif
#if ((ETH_43_GMAC_CFG_SW_MAJOR_VERSION != ETH_43_GMAC_PBCFG_SW_MAJOR_VERSION) || \
     (ETH_43_GMAC_CFG_SW_MINOR_VERSION != ETH_43_GMAC_PBCFG_SW_MINOR_VERSION) || \
     (ETH_43_GMAC_CFG_SW_PATCH_VERSION != ETH_43_GMAC_PBCFG_SW_PATCH_VERSION)    \
    )
    #error "Software Version Numbers of Eth_43_GMAC_Cfg.h and Eth_43_GMAC_PBcfg.h are different"
#endif

#ifndef DISABLE_MCAL_INTERMODULE_ASR_CHECK
    /* Checks against Eth_GeneralTypes.h */
    #if ((ETH_43_GMAC_CFG_AR_RELEASE_MAJOR_VERSION != ETH_GENERALTYPES_AR_RELEASE_MAJOR_VERSION) || \
         (ETH_43_GMAC_CFG_AR_RELEASE_MINOR_VERSION != ETH_GENERALTYPES_AR_RELEASE_MINOR_VERSION)    \
        )
        #error "AUTOSAR Version Numbers of Eth_43_GMAC_Cfg.h and Eth_GeneralTypes.h are different"
    #endif

    /* Checks against Mcal.h */
    #if ((ETH_43_GMAC_CFG_AR_RELEASE_MAJOR_VERSION != MCAL_AR_RELEASE_MAJOR_VERSION) || \
         (ETH_43_GMAC_CFG_AR_RELEASE_MINOR_VERSION != MCAL_AR_RELEASE_MINOR_VERSION)    \
        )
        #error "AUTOSAR Version Numbers of Eth_43_GMAC_Cfg.h and Mcal.h are different"
    #endif
#endif
/*==================================================================================================
*                                      DEFINES AND MACROS
==================================================================================================*/

/*! @brief InstanceId of this module instance. If only one instance is present it shall have the Id 0 */
#define ETH_43_GMAC_DRIVER_INSTANCE             (0U)    /* Instance number assigned to the driver */

/*! @brief Enables / Disables user mode support */
#define ETH_43_GMAC_ENABLE_USER_MODE_SUPPORT    (STD_OFF)

#ifndef MCAL_ENABLE_USER_MODE_SUPPORT
    #if (STD_ON == ETH_43_GMAC_ENABLE_USER_MODE_SUPPORT)
        #error MCAL_ENABLE_USER_MODE_SUPPORT is not enabled. For running ETH in user mode, MCAL_ENABLE_USER_MODE_SUPPORT needs to be defined.
    #endif /* (STD_ON == ETH_43_GMAC_ENABLE_USER_MODE_SUPPORT */
#endif /* ifndef MCAL_ENABLE_USER_MODE_SUPPORT */

/*! @brief Enables / Disables multi-partition support */
#define ETH_43_GMAC_MULTIPARTITION_SUPPORT           (STD_OFF)

/*! @brief Number of partitions in the system as defined in EcuC */
#define ETH_43_GMAC_MAX_PARTITIONIDX_SUPPORTED       (1U)

/*! @brief Enables / Disables PreCompile support when there is a single configuration */
#define ETH_43_GMAC_PRECOMPILE_SUPPORT          (STD_ON)

/*! @brief Enables / Disables DET development errors detection and reporting */
#define ETH_43_GMAC_DEV_ERROR_DETECT            (STD_OFF)

/*! @brief Enables / Disables DEM events detection and reporting */
#define ETH_43_GMAC_DEM_EVENT_DETECT            (STD_ON)

/*! @brief Enables / Disables hardware offloading for IPv4 checksums */
#define ETH_43_GMAC_ENABLE_OFFLOAD_CRC_IPV4     (STD_OFF)

/*! @brief Enables / Disables hardware offloading for ICMP checksums */
#define ETH_43_GMAC_ENABLE_OFFLOAD_CRC_ICMP     (STD_OFF)

/*! @brief Enables / Disables hardware offloading for TCP checksums */
#define ETH_43_GMAC_ENABLE_OFFLOAD_CRC_TCP      (STD_OFF)

/*! @brief Enables / Disables hardware offloading for UDP checksums */
#define ETH_43_GMAC_ENABLE_OFFLOAD_CRC_UDP      (STD_OFF)

/*! @brief Enables / Disables hardware offloading for checksums */
#define ETH_43_GMAC_ENABLE_OFFLOAD_CRC          (STD_OFF)

/*! @brief Enables / Disables the Switch Management APIs */
#define ETH_43_GMAC_SWT_MANAGEMENT_SUPPORT_API  (STD_OFF)

/*! @brief Enables / Disables Eth_43_GMAC_GetVersionInfo API */
#define ETH_43_GMAC_VERSION_INFO_API            (STD_OFF)

/*! @brief Enables / Disables Eth_43_GMAC_UpdatePhysAddrFilter API */
#define ETH_43_GMAC_UPDATE_PHYS_ADDR_FILTER_API (STD_ON)

/*! @brief Enables / Disables Eth_43_GMAC_GetCounterValues API */
#define ETH_43_GMAC_GET_COUNTER_API             (STD_OFF)

/*! @brief Enables / Disables Eth_43_GMAC_GetTxStats API */
#define ETH_43_GMAC_GET_TXSTATS_API             (STD_OFF)

/*! @brief Enables / Disables Eth_43_GMAC_GetRxStats API */
#define ETH_43_GMAC_GET_RXSTATS_API             (STD_ON)

/*! @brief Enables / Disables Eth_43_GMAC_GetTxErrorCounterValues API */
#define ETH_43_GMAC_GET_TXERROR_COUNTER_API     (STD_OFF)

/*! @brief Enables / Disables the GlobalTime APIs used amongst others by Global Time Synchronization over Ethernet */
#define ETH_43_GMAC_GLOBAL_TIME_API             (STD_ON)

/*! @brief Enables / Disables the Eth_43_GMAC_SendMultiBufferFrame API */
#define ETH_43_GMAC_SEND_MULTI_BUFFER_FRAME_API             (STD_OFF)

/*! @brief Enables / Disables the Eth_43_GMAC_GetSyncState API */
#define ETH_43_GMAC_GET_SYNC_STATE_API              (STD_OFF)
/*! @brief Enables / Disables EthSetSiPhysAddr API */
#define ETH_43_GMAC_SET_SI_PHYS_ADDR_API             (STD_OFF)/*! @brief The maximum number of buffers that can be used for constructing a multi buffer frame */
#define ETH_43_GMAC_MAX_BUFFS_IN_MULTI_BUFFER_FRAME       (0U)

/* PreCompile parameters residing in container EthCtrlConfig which has attribute postBuildVariantMultiplicity = true */
#ifndef ETH_43_GMAC_MDIO_CLAUSE22_API    /* Will eventually be defined in Eth_43_GMAC_<PBVName>_PBcfg.h */
    /*! @brief Enables / Disables Management Data I/O (MDIO) Clause 22for transceiver accesses */
    #define ETH_43_GMAC_MDIO_CLAUSE22_API          (STD_OFF)
#endif

#ifndef ETH_43_GMAC_MDIO_CLAUSE45_API    /* Will eventually be defined in Eth_43_GMAC_<PBVName>_PBcfg.h */
    /*! @brief Enables / Disables Management Data I/O (MDIO) Clause45 for transceiver accesses */
    #define ETH_43_GMAC_MDIO_CLAUSE45_API          (STD_OFF)
#endif

#ifndef ETH_43_GMAC_RX_IRQ_ENABLED       /* Will eventually be defined in Eth_43_GMAC_<PBVName>_PBcfg.h */
    /*! @brief Enables / Disables channel interrupts on Rx */
    #define ETH_43_GMAC_RX_IRQ_ENABLED      (STD_OFF)
#endif

#ifndef ETH_43_GMAC_TX_IRQ_ENABLED    /* Will eventually be defined in Eth_43_GMAC_<PBVName>_PBcfg.h */
  /*! @brief Enables / Disables channel interrupts on Tx */
  #define ETH_43_GMAC_TX_IRQ_ENABLED            (STD_OFF)
#endif

/*! @brief Number of controllers configured by the driver */
#define ETH_43_GMAC_MAX_CTRLIDX_SUPPORTED       (1U)     /* Maximum configured controller hardware index + 1 (might be different from the total number of hardware controllers available on the platform) */

/*! @brief Number of VLAN Priority Code Points */
#define ETH_43_GMAC_MAX_VLAN_PCP                (8U)

/*! @brief Enables / Disables the allocation of the TX data buffers. */
#define ETH_43_GMAC_HAS_EXTERNAL_TX_BUFFERS  (STD_OFF)

/*! @brief Enables / Disables the allocation of the RX data buffers. */
#define ETH_43_GMAC_HAS_EXTERNAL_RX_BUFFERS  (STD_OFF)


/*! @brief Enables / Disables Time Aware Shaper */
#define ETH_43_GMAC_TIME_AWARE_SHAPER              (STD_OFF)
/*! @brief Enables / Disables Ingress Port Filter */
#define ETH_43_GMAC_INGRESS_PORT_FILTER              (STD_OFF)
/*! @brief Enables / Disables TX/RX coalescing interrupt */
#define ETH_43_GMAC_COALESCING_INTERRUPT              (STD_OFF)
/*! @brief Enables /  Disables Mac Sec Integration. */
#define ETH_43_GMAC_MACSEC_SUPPORT              (STD_OFF)
/*! @brief Enables / Disables Pulse-per-second signal outputs */
#define ETH_43_GMAC_PPS_OUTPUT_SUPPORT              (STD_OFF)
/*! @brief Enables / Disables Low Power Idle (LPI) mode */
#define ETH_43_GMAC_LPI_ENABLE              (STD_OFF)
/*! @brief Enables / Disables the configuration of StartTSN API. */
#define ETH_43_GMAC_START_TSN_SUPPORT              (STD_OFF)
/*==================================================================================================
*                                             ENUMS
==================================================================================================*/
#if (STD_ON == ETH_43_GMAC_PPS_OUTPUT_SUPPORT)
/*!
 * @brief The PPS output commands.
 * @implements
 */
typedef enum {
    ETH_43_GMAC_NO_COMMAND                    = 0U,
    ETH_43_GMAC_START_SINGLE_PULSE            = 1U,
    ETH_43_GMAC_START_PULSE_TRAIN             = 2U,
    ETH_43_GMAC_CANCEL_START                  = 3U,
    ETH_43_GMAC_STOP_PULSE_TRAIN_AT_TIME      = 4U,
    ETH_43_GMAC_STOP_PULSE_TRAIN_IMMEDIATELY  = 5U,
    ETH_43_GMAC_CANCEL_STOP_PULSE_TRAIN       = 6U,
} Eth_43_GMAC_PPSOutputCmd;
#endif

/*==================================================================================================
*                                STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
typedef struct
{
    uint16 BufferCount;    /* Total number of buffers across all Tx FIFOs */
    uint8 FifoCount;       /* Total number of configured Tx FIFOs */
    uint8 VlanPcpToFifoIdx[ETH_43_GMAC_MAX_VLAN_PCP];   /* Map between VLAN PCPs and Tx FIFOs */

} Eth_43_GMAC_EgressCfgType;

typedef struct
{
    uint16 BufferCount;    /* Total number of buffers across all Rx FIFOs */
    uint8 FifoCount;       /* Total number of configured Rx FIFOs */

} Eth_43_GMAC_IngressCfgType;

typedef struct
{
    uint32 ModuleClock;                 /* Frequency of the application interface (in Hz) */
    uint8 SubSecondInc;                 /* Default value for SSINC register. */
    uint8 SubNanoSecondsInc;            /* Default value for SNSINC register. */
    float64 MultiplyRatioCompensation;  /* Multiply ratio for computation of new value for TSAR register. */
} Eth_43_GMAC_ClockCfgType;

#if STD_ON == ETH_43_GMAC_DEM_EVENT_DETECT
/**
 * @brief    Type for holding DEM events
 * @details  It contains the necessary events to report to the DEM module
 */
typedef struct
{
    Mcal_DemErrorType ETH_E_ACCESS_Cfg;
    Mcal_DemErrorType ETH_E_RX_FRAMES_LOST_Cfg;
    Mcal_DemErrorType ETH_E_CRC_Cfg;
    Mcal_DemErrorType ETH_E_UNDERSIZEFRAME_Cfg;
    Mcal_DemErrorType ETH_E_OVERSIZEFRAME_Cfg;
    Mcal_DemErrorType ETH_E_ALIGNMENT_Cfg;
    Mcal_DemErrorType ETH_E_SINGLECOLLISION_Cfg;
    Mcal_DemErrorType ETH_E_MULTIPLECOLLISION_Cfg;
    Mcal_DemErrorType ETH_E_LATECOLLISION_Cfg;

} Eth_43_GMAC_DemEventsType;
#endif

#if STD_ON == ETH_43_GMAC_DEM_EVENT_DETECT
/**
 * @brief    Type for enumerating DEM error IDs
 * @details  It contains a mapping between DEM events to error IDs
 */
typedef enum
{
    ETH_E_ACCESS            = 0U,
    ETH_E_RX_FRAMES_LOST    = 1U,
    ETH_E_CRC               = 2U,
    ETH_E_UNDERSIZEFRAME    = 3U,
    ETH_E_OVERSIZEFRAME     = 4U,
    ETH_E_ALIGNMENT         = 5U,
    ETH_E_SINGLECOLLISION   = 6U,
    ETH_E_MULTIPLECOLLISION = 7U,
    ETH_E_LATECOLLISION     = 8U,
    ETH_NUMBER_OF_DEM_IDS   = 9U

} Eth_43_GMAC_DemErrorIdType;
#endif


#if (STD_ON == ETH_43_GMAC_MDIO_CLAUSE22_API) || (STD_ON == ETH_43_GMAC_MDIO_CLAUSE45_API)
/**
 * @brief    Type for holding function pointers to the EthTrcv
 * @details  It contains the needed function pointers to EthTrcv functions.
 */
typedef struct
{
    void (*readMiiIndicationFunction)(uint8 CtrlIdx, uint8 TrcvIdx, uint8 RegIdx, uint16 RegValPtr);
    void (*writeMiiIndicationFunction)(uint8 CtrlIdx, uint8 TrcvIdx, uint8 RegIdx);

}Eth_43_GMAC_EthTrcvFunctionsType;
#endif


#if STD_ON == ETH_43_GMAC_SWT_MANAGEMENT_SUPPORT_API
/**
 * @brief    Type for holding function pointers to the EthSwt
 * @details  It contains the needed function pointers to EthSwt functions.
 */
typedef struct
{
    void (*TxAdaptBufferLengthFunction)(uint16* LengthPtr);
    Std_ReturnType (*TxPrepareFrameFunction)(uint8 CtrlIdx, Eth_BufIdxType BufIdx, uint8** DataPtr, uint16* LengthPtr);
    Std_ReturnType (*TxProcessFrameFunction)(uint8 CtrlIdx, Eth_BufIdxType BufIdx, uint8** DataPtr, uint16* LengthPtr);
    Std_ReturnType (*TxFinishedIndicationFunction)(uint8 CtrlIdx, Eth_BufIdxType BufIdx);
    Std_ReturnType (*RxProcessFrameFunction)(uint8 CtrlIdx, Eth_BufIdxType BufIdx, uint8** DataPtr, uint16* LengthPtr, boolean* IsMgmtFrameOnlyPtr);
    Std_ReturnType (*RxFinishedIndicationFunction)(uint8 CtrlIdx, Eth_BufIdxType BufIdx);

}Eth_43_GMAC_EthSwtFunctionsType;
#endif

/* Forward IPW structure declarations */
struct sEth_43_GMAC_Ipw_CtrlConfigType;

#if (STD_ON == ETH_43_GMAC_PPS_OUTPUT_SUPPORT)
/**
* @brief         Type used to express Pulse-per-second target time.
* @details       Variables of this type are used to express Pulse-per-second target time.
* Implements     Eth_PPSTargetTime
*/
typedef struct
{
    uint32 nanoseconds;  /**< @brief Nanoseconds part of the time. */
    uint32 seconds;      /**< @brief Seconds part of the time. */
} Eth_43_GMAC_PPSTargetTime;

/**
* @brief         Type used to express configured parameter of Pulse-per-second configuration.
* @details       Variables of this type are used to express Pulse-per-second target time.
*
*/
typedef struct
{
    Eth_43_GMAC_PPSOutputCmd PPSOutputCmd;        /*!< The PPS output commands */
    Eth_43_GMAC_PPSTargetTime PPSTargetTime;      /*!< The PPS target time */
} Eth_43_GMAC_PPSConfigType;
#endif

/**
* @brief         Type for Ethernet driver configuration values storage.
* @details       This type is used to collect and pass all configuration data
*                of one controller (except those which have only
*                precompile variant) to the Eth driver.
* implements Eth_43_GMAC_CtrlCfgType_structure
*/
typedef struct
{
    const struct sEth_43_GMAC_Ipw_CtrlConfigType *Eth_43_GMAC_pIpwCtrlConfig;
    const Eth_43_GMAC_EgressCfgType *Eth_43_GMAC_pEgressConfig;
    const Eth_43_GMAC_IngressCfgType *Eth_43_GMAC_pIngressConfig;
    const Eth_43_GMAC_ClockCfgType *Eth_43_GMAC_pClockConfig;
    const uint8 EthCtrlEthIfIdx;    /**< @brief Index of the controller in context of EthIf driver */
    const uint8 EthCtrlIdx;    /**< @brief Index of the controller */
#if (STD_ON == ETH_43_GMAC_DEM_EVENT_DETECT)
    const Eth_43_GMAC_DemEventsType Eth_43_GMAC_DemEventsList;
#endif
#if (STD_ON == ETH_43_GMAC_MDIO_CLAUSE22_API) || (STD_ON == ETH_43_GMAC_MDIO_CLAUSE45_API)
    const Eth_43_GMAC_EthTrcvFunctionsType Eth_43_GMAC_EthTrcvDriverFunctionList;  /**< @brief The structure with pointer functions to the EthTrcv driver functions */
#endif
#if STD_ON == ETH_43_GMAC_SWT_MANAGEMENT_SUPPORT_API
    const Eth_43_GMAC_EthSwtFunctionsType Eth_43_GMAC_EthSwtDriverFunctionList; /**< @brief The structure with pointer functions to the EthSwt driver functions */
#endif

} Eth_43_GMAC_CtrlCfgType;

/*!
* @brief This type is used to group all multiple configurations.
* implements Eth_ConfigType_structure

*/
typedef struct
{
#if (STD_ON == ETH_43_GMAC_MULTIPARTITION_SUPPORT)
    uint32 PartitionId;
#endif
    const Eth_43_GMAC_CtrlCfgType *Eth_43_GMAC_apCtrlConfig[ETH_43_GMAC_MAX_CTRLIDX_SUPPORTED];

} Eth_43_GMAC_ConfigType;

#if defined(ETH_43_GMAC_MACSEC_SUPPORT)
#if (STD_ON == ETH_43_GMAC_MACSEC_SUPPORT)
/*!
 * @brief Controls validation of received frames.
 */
typedef enum
{
    MKA_VALIDATE_DISABLED = 0U, /**< Disable validation, remove SecTAGs and ICVs (if present) from received frames. */
    MKA_VALIDATE_CHECKED,       /**< Enable validation, do not discard invalid frames. */
    MKA_VALIDATE_STRICT         /**< Enable validation and discard invalid frames. */
} Mka_ValidateFramesType;

/*!
 * @brief Indicates the offset in case of integrity with confidentiality.
 */
typedef enum
{
    MKA_CONFIDENTIALITY_NONE      = 0xFF,  /**< Confidentiality protection disabled. */
    MKA_CONFIDENTIALITY_OFFSET_0  = 0U,       /**< Zero initial octets of each user data without confidentiality protection. */
    MKA_CONFIDENTIALITY_OFFSET_30 = 30U,      /**< 30 initial octets of each user data without confidentiality protection. */
    MKA_CONFIDENTIALITY_OFFSET_50 = 50U      /**< 50 initial octets of each user data without confidentiality protection. */
} Mka_ConfidentialityOffsetType;

/*!
 * @brief SAK key reference.
 */
typedef struct
{
    const uint8 *HashKeyPtr; /**< Pointer to the Hash Key. */
    const uint8 *SakKeyPtr;  /**< Pointer to the SAK. */
    const uint8 *SaltKeyPtr; /**< Pointer to the Salt. */
} Mka_SakKeyPtrType;

/*!
 * @brief MACsec Entity (SecY) transmission statistics.
 */
typedef struct
{
    uint64 OutPkts_Untagged;    /**< The number of packets transmitted without a SecTAG. */
    uint64 OutPkts_TooLong;     /**< The number of transmitted packets discarded because their length is greater than the accepted maximum length (mtu) of the Port. */
    uint64 OutOctets_Protected; /**< The number of plain text octets integrity protected but not encrypted in transmitted frames. */
    uint64 OutOctets_Encrypted; /**< The number of plain text octets integrity protected and encrypted in transmitted frames. */
} Mka_Stats_Tx_SecYType;

/*!
 * @brief MACsec Entity (SecY) reception statistics.
 */
typedef struct
{
    uint64 InPkts_Untagged;    /**< The number of packets without the MACsec tag (SecTAG) received if ValidateFrames was not  VALIDATE_STRICT . */
    uint64 InPkts_NoTag;       /**< The number of received packets without a SecTAG discarded because ValidateFrames was  VALIDATE_STRICT . */
    uint64 InPkts_BadTag;      /**< The number of received packets discarded with an invalid SecTAG, zero value PN, or invalid ICV. */
    uint64 InPkts_NoSa;        /**< The number of received packets with an unknown SCI or for an unused SA by the security entity. */
    uint64 InPkts_NoSaError;   /**< The number of packets discarded because the received SCI is unknown or the SA is not in use. */
    uint64 InPkts_Overrun;     /**< The number of packets discarded because they exceeded cryptographic performance capabilities. */
    uint64 InOctets_Validated; /**< The number of plain text octets recovered from packets that were integrity protected but not encrypted. */
    uint64 InOctets_Decrypted; /**< The number of plain text octets recovered from packets that were integrity protected and encrypted. */
} Mka_Stats_Rx_SecYType;

/*!
 * @brief Secure Channel transmission statistics.
 */
typedef struct
{
    uint64 OutPkts_Protected; /**< The number of integrity protected but not encrypted packets for this transmit SC. */
    uint64 OutPkts_Encrypted; /**< The number of integrity protected and encrypted packets for this transmit SC. */
} Mka_Stats_Tx_ScType;

/*!
 * @brief Secure Channel reception statistics.
 */
typedef struct
{
    uint64 InPkts_Ok;        /**< The number of packets received for this secure channel successfully validated and within the replay window. */
    uint64 InPkts_Unchecked; /**< The number of packets received for this secure channel, if ValidateFrames was  VALIDATE_DISABLED . */
    uint64 InPkts_Delayed;   /**< The number of received packets, for this secure channel, with packet number (PN) lower than the lowest acceptable packet number (LowestPn) and ReplayProtect false. */
    uint64 InPkts_Late;      /**< The number of packets discarded, for this secure channel, because the received packet number (PN) was lower than the lowest acceptable packet number (LowestPn) and
                                  ReplayProtect was true. */
    uint64 InPkts_Invalid;   /**< The number of packets, for this secure channel, that failed validation but could be received because ValidateFrames was  VALIDATE_ CHECKED  and the data was not
                                  encrypted (so the original frame could be recovered). */
    uint64 InPkts_NotValid;  /**< The number of packets discarded, for this secure channel, because validation failed and ValidateFrames was  VALIDATE_STRICT  or the data was encrypted
                                  (so the original frame could not be recovered). */
} Mka_Stats_Rx_ScType;

/*!
 * @brief Security Entity statistics.
 */
typedef struct
{
    Mka_Stats_Tx_SecYType StatsTxPhy; /**< Set of statistics in the Security Entity Phy by transmission. */
    Mka_Stats_Rx_SecYType StatsRxPhy; /**< Set of statistics in the Security Entity Phy by reception. */
    Mka_Stats_Tx_ScType StatsTxSc;    /**< Set of statistics in the Security Entity s Secure Channel by reception. */
    Mka_Stats_Rx_ScType StatsRxSc;    /**< Set of statistics in the Security Entity s Secure Channel by reception. */
} Mka_Stats_SecYType;

/*!
 * @brief Structure to configure a referred SecY.
 */
typedef struct
{
    boolean ProtectFrames;                               /**< Indicates status if the MACsec protection of the frames is active or not. */
    boolean ReplayProtect;                               /**< Indicates status if replay protection is enable or disable. */
    uint32 ReplayWindow;                                 /**< If ReplayProtect is enable, indicates the used replay protect window. */
    Mka_ValidateFramesType ValidateFrames;               /**< Status of the validation of the frames. See Mka_ValidateFramesType for possible values. */
    uint64 CurrentCipherSuite;                           /**< Indicates which cipher suite is used in the SecY to update. */
    Mka_ConfidentialityOffsetType ConfidentialityOffset; /**< Set the Confidentiality Offset. See Mka_ConfidentialityOffsetType for possible values. */
    boolean ControlledPortEnabled;                       /**< Status if the controlled port is enabled or disabled. */
    const uint16 *BypassedVlanPtrs;                      /**< Pointer to the list of bypassed VLANs. */
    uint8 BypassedVlansLength;                           /**< Length of the list of bypassed VLANs. */
    const uint16 *BypassedEtherTypesPtr;                 /**< Pointer to the list of the bypassed Ethernet Types. */
    uint8 BypassedEtherTypesLength;                      /**< Length of the list of the bypassed Ethernet Types. */
} Mka_MacSecConfigType;

#endif /*defined(ETH_43_GMAC_MACSEC_SUPPORT) */
#endif  /*(STD_ON == ETH_43_GMAC_MACSEC_SUPPORT) */
/*==================================================================================================
*                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
*                                    FUNCTION PROTOTYPES
==================================================================================================*/

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* ETH_43_GMAC_CFG_H */

