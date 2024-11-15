/*
 * Copyright 2019-2024 NXP
 *
 * NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be
 * used strictly in accordance with the applicable license terms. By expressly
 * accepting such terms or by downloading, installing, activating and/or otherwise
 * using the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms.  If you do not agree to be
 * bound by the applicable license terms, then you may not retain, install,
 * activate or otherwise use the software.
 */

#ifndef GPTP_DEF
    #define GPTP_DEF

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "gptp_port_def.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*!
 * @name Product detail string lengths
 * @{
 */
/*! Manufacturer identity string length */
#define GPTP_DEF_MAN_IDENTITY_LENGTH    3u
/*! Product description string length */
#define GPTP_DEF_PROD_DESC_LENGTH       64u
/*! Product revision string length */
#define GPTP_DEF_PROD_REV_LENGTH        32u
/*! @} */

/*!
 * @name PTP message parts length
 * @{
 */
/*! ETH II length */
#define GPTP_DEF_ETH_II_LEN             14u
/*! gPTP Sync message length */
#define GPTP_DEF_SYNC_MSG_LEN           44u
/*! gPTP Pdelay request message length */
#define GPTP_DEF_PDELAY_REQ_MSG_LEN     54u
/*! gPTP Pdelay response message length */
#define GPTP_DEF_PDELAY_RESP_MSG_LEN    54u
/*! gPTP Follow up message length */
#define GPTP_DEF_FOLLOW_UP_MSG_LEN      76u
/*! gPTP Pdelay follow up message length */
#define GPTP_DEF_PDELAY_FUP_MSG_LEN     54u
/*! gPTP Signaling message interval length */
#define GPTP_DEF_SIGNALING_MSG_LEN      60u
/*! @} */

/*!
 * @name Flags values for specific messages
 * @{
 */
/*! 'flags' value for Sync message */
#define GPTP_DEF_SYNC_FLAGS             0x0208
/*! 'flags' value for Follow-up message */
#define GPTP_DEF_FUP_FLAGS              0x0008
/*! 'flags' value for Pdelay request message */
#define GPTP_DEF_PDELAY_REQ_FLAGS       0x0008
/*! 'flags' value for Pdelay response message */
#define GPTP_DEF_PDELAY_RESP_FLAGS      0x0208
/*! 'flags' value for Pdelay response follow-up message */
#define GPTP_DEF_PDELAY_RESP_FUP_FLAGS  0x0008
/*! 'flags' value for Signaling message */
#define GPTP_DEF_SIGNALING_FLAGS        0x0008
/*! 'flags' value for Signaling message TLV */
#define GPTP_DEF_SIG_TLV_FLAGS          0x03
/*! @} */

/*!
 * @name Input sizes for buffer length calculations
 * @{
 */
/*! VLAN tag length including Ethtype */
#define GPTP_DEF_ETH_VLAN_LEN           4u
/*! Number of cascaded switches (depth of the cascade) */
#define GPTP_DEF_NUM_CASCADE_SW         2u
/*! Header for the switch length */
#define GPTP_DEF_HEADER_LEN             4u
/*! Trailed for the switch length */
#define GPTP_DEF_TRAILER_LEN            (1u + (GPTP_DEF_NUM_CASCADE_SW * 3u))
/*! Header + Trailer length */
#define GPTP_DEF_HEAD_TRAIL_LEN         (GPTP_DEF_HEADER_LEN + GPTP_DEF_TRAILER_LEN)
/*! @} */

/*!
 * @name Size of buffers for PTP messages
 * @{
 */
/*! Length of TX buffer for Sync message */
#define GPTP_DEF_TX_BUF_LEN_SYNC        (((GPTP_DEF_SYNC_MSG_LEN + GPTP_DEF_ETH_II_LEN) + GPTP_DEF_HEAD_TRAIL_LEN) + GPTP_DEF_ETH_VLAN_LEN)
/*! Length of TX buffer for Pdelay request message */
#define GPTP_DEF_TX_BUF_LEN_PD_RQ       (((GPTP_DEF_PDELAY_REQ_MSG_LEN + GPTP_DEF_ETH_II_LEN) + GPTP_DEF_HEAD_TRAIL_LEN) + GPTP_DEF_ETH_VLAN_LEN)
/*! Length of TX buffer for Pdelay response message */
#define GPTP_DEF_TX_BUF_LEN_PD_RSP      (((GPTP_DEF_PDELAY_RESP_MSG_LEN + GPTP_DEF_ETH_II_LEN) + GPTP_DEF_HEAD_TRAIL_LEN) + GPTP_DEF_ETH_VLAN_LEN)
/*! Length of TX buffer for Follow-up message */
#define GPTP_DEF_TX_BUF_LEN_FUP         (((GPTP_DEF_FOLLOW_UP_MSG_LEN + GPTP_DEF_ETH_II_LEN) + GPTP_DEF_HEAD_TRAIL_LEN) + GPTP_DEF_ETH_VLAN_LEN)
/*! Length of TX buffer for Pdelay follow-up message */
#define GPTP_DEF_TX_BUF_LEN_PD_FUP      (((GPTP_DEF_PDELAY_FUP_MSG_LEN + GPTP_DEF_ETH_II_LEN) + GPTP_DEF_HEAD_TRAIL_LEN) + GPTP_DEF_ETH_VLAN_LEN)
/*! Length of TX buffer for Signaling message */
#define GPTP_DEF_TX_BUF_LEN_SIGNL       (((GPTP_DEF_SIGNALING_MSG_LEN + GPTP_DEF_ETH_II_LEN) + GPTP_DEF_HEAD_TRAIL_LEN) + GPTP_DEF_ETH_VLAN_LEN)
/*! @} */

/*!
 * @name Various bit masks (count of low bits "L", count of high bits "H")
 * @{
 */
/*! Bit mask, upper 16 bits low, lower 48 bits high */
#define GPTP_DEF_BIT_MASK_16L_48H       (0x0000FFFFFFFFFFFFu)
/*! Bit mask, upper 32 bits low, lower 32 bits high */
#define GPTP_DEF_BIT_MASK_32L_32H       (0x00000000FFFFFFFFu)
/*! Bit mask, upper 48 bits low, lower 16 bits high */
#define GPTP_DEF_BIT_MASK_48L_16H       (0x000000000000FFFFu)
/*! Bit mask, upper 56 bits low, lower 8 bits high */
#define GPTP_DEF_BIT_MASK_56L_8H        (0x00000000000000FFu)
/*! Bit mask, upper 32 bits high, lower 32 bits low */
#define GPTP_DEF_BIT_MASK_32H_32L       (0xFFFFFFFF00000000u)
/*! Bit mask, upper 8 bits low, lower 24 bits high */
#define GPTP_DEF_BIT_MASK_8L_24H        0x00FFFFFFu
/*! Bit mask, upper 16 bits low, lower 16 bits high */
#define GPTP_DEF_BIT_MASK_16L_16H       0x0000FFFFu
/*! Bit mask, upper 24 bits low, lower 8 bits high */
#define GPTP_DEF_BIT_MASK_24L_8H        0x000000FFu
/*! @} */

/*!
 * @name Various limits of integer types
 * @{
 */
/*! INT32 min value */
#define GPTP_DEF_INT32_MIN              (-2147483647 -1)
/*! INT32 max value */
#define GPTP_DEF_INT32_MAX              (2147483647)
/*! @} */

/*!
 * @name Pre-calculated float constants
 * @{
 */
/*! 2^(-41) */
#define GPTP_DEF_POW_2_M41              (4.5474735e-13)
/*! 2^(41) */
#define GPTP_DEF_POW_2_41               (2.1990233e+12)
/*! @} */

/*!
 * @name Sync machine roles
 * @{
 */
/*! Sync state machine master */
#define GPTP_DEF_PTP_MASTER             (true)
/*! Sync state machine slave */
#define GPTP_DEF_PTP_SLAVE              (false)
/*! @} */

/*!
 * @name PTP Messaging interval limits
 * @{
 */
/*! Minimum Pdelay interval limit - 31.25ms */
#define GPTP_DEF_LOG_PDEL_INT_MIN       (-5)
/*! Maximum Pdelay interval limit - 8s*/
#define GPTP_DEF_LOG_PDEL_INT_MAX       3
/*! Minimum Sync interval limit - 31.25ms */
#define GPTP_DEF_LOG_SYNC_INT_MIN       (-5)
/*! Maximum Sync interval limit - 1s*/
#define GPTP_DEF_LOG_SYNC_INT_MAX       0
/*! @} */

/*!
 * @name Frame ID start and maximum ranges for messages
 * @{
 */
/*! Sync frame ID start position */
#define GPTP_DEF_FID_SYNC_START         0u
/*! Sync frame ID end position */
#define GPTP_DEF_FID_SYNC_MAX           150u
/*! Pdelay request start position */
#define GPTP_DEF_FID_PDREQ_START        150u
/*! Pdelay request end position */
#define GPTP_DEF_FID_PDREQ_MAX          200u
/*! Pdelay response start position */
#define GPTP_DEF_FID_PDRESP_START       200u
/*! Pdelay response end position */
#define GPTP_DEF_FID_PDRESP_MAX         250u
/*! @} */

/*!
 * @name Debug messages verbosity levels
 * @{
 */
/*! Highest verbosity, containing the synchronization status messages */
#define GPTP_DEBUG_MSGTYPE_SYNC         3
/*! High verbosity, containing informative messages */
#define GPTP_DEBUG_MSGTYPE_INFO         2
/*! Medium verbosity, containing state messages */
#define GPTP_DEBUG_MSGTYPE_STATE        1
/*! Low verbosity, Error messages only */
#define GPTP_DEBUG_MSGTYPE_ERROR        0
/*! @} */

/*!
 * @name Misc gPTP constants
 * @{
 */

/*! Period of Offset estimation calculation */
#define GPTP_DEF_CALC_PERIOD_100_MS     100000000u

/*! Transport specific value for the check */
#define GPTP_DEF_TRANSPORT_SPEC_1       0x10

/*! Maximum start-up timeout [s] */
#define GPTP_DEF_STARTUP_TIMEOUT_MAX    20

/*! Signaling message value for stop sending */
#define GPTP_DEF_SIG_TLV_STOP_SENDING   127
/*! Signaling message value for setting to initial value */
#define GPTP_DEF_SIG_TLV_SET_TO_INIT    126
/*! Signaling message value for not changing interval */
#define GPTP_DEF_SIG_TLV_NOT_CHANGE     (-128)

/*! Count of nanoseconds in 1.5 seconds */
#define GPTP_DEF_1S5_IN_NS              1500000000u
/*! Count of nanoseconds in 1 second */
#define GPTP_DEF_NS_IN_SECONDS          1000000000u
/*! Count of nanoseconds in 10 miliseconds */
#define GPTP_DEF_10_MS_IN_NS            10000000u

/*! First sequence ID for Pdelay messages */
#define GPTP_DEF_PDELAY_FIRST_SEQ_ID    0x0100u
/*! First sequence ID for Sync messages */
#define GPTP_DEF_SYNC_FIRST_SEQ_ID      0x0400u

/*! PTP Multicast MAC Address - each PTP message is transmitted to this address */
#define GPTP_DEF_ETH_PTP_MAC_MULTICAST  (0x0180C200000Eu)

/*! Buffer index not used - map entry closed */
#define GPTP_DEF_BUFF_INDEX_CLOSED      0xFFFFFFFFU
#define GPTP_DEF_FRAME_INDEX_CLOSED     0xFFU

#ifdef GPTP_COUNTERS
/*! Count of port counters */
#define GPTP_COUNTERS_PORT_CNT          13u
/*! Count of domain counters */
#define GPTP_COUNTERS_DOM_CNT           6u
/*! Count of counters defined by IEEE */
#define GPTP_IEEE_COUNTERS_CNT          18u
/*! @} */

/*!
 * @name gPTP Counter enum
 * @{
 */

/*!
 * @brief gPTP Counters
 */
typedef enum
{   /*! Count of valid Sync messages received on the gPTP port */
    ieee8021AsPortStatRxSyncCount = 1,
    /*! This counter is not used, no One Step Syncs */
    ieee8021AsPortStatRxOneStepSyncCount,
    /*! Count of valid Follow-Up messages received on the gPTP port */
    ieee8021AsPortStatRxFollowUpCount,
    /*! Count of valid Pdelay Request messages received on the gPTP port */
    ieee8021AsPortStatRxPdelayRequest,
    /*! Count of valid Pdelay Response messages received on the gPTP port */
    ieee8021AsPortStatRxPdelayResponse,
    /*! Count of valid Pdelay Request Follow-Up messages received on the gPTP port */
    ieee8021AsPortStatRxPdelayResponseFollowUp,
    /*! This counter is not used, no Announce messages allowed */
    ieee8021AsPortStatRxAnnounce,
    /*! Count of discarded PTP packets */
    ieee8021AsPortStatRxPTPPacketDiscard,
    /*! Count of Sync Receipt Timeout events */
    ieee8021AsPortStatRxSyncReceiptTimeouts,
    /*! This counter is not used, no Announce messages allowed */
    ieee8021AsPortStatAnnounceReceiptTimeouts,
    /*! Count of exceeded Alowed Lost Pdelay Responses (Too many lost responses) events */
    ieee8021AsPortStatPdelayAllowedLostResponsesExceeded,
    /*! Count of Sync messages transmitted on the gPTP port */
    ieee8021AsPortStatTxSyncCount,
    /*! This counter is not used, no One Step Syncs */
    ieee8021AsPortStatTxOneStepSyncCount,
    /*! Count of Follow-Up messages transmitted on the gPTP port */
    ieee8021AsPortStatTxFollowUpCount,
    /*! Count of Pdelay Request messages transmitted on the gPTP port */
    ieee8021AsPortStatTxPdelayRequest,
    /*! Count of Pdelay Response messages transmitted on the gPTP port */
    ieee8021AsPortStatTxPdelayResponse,
    /*! Count of Pdelay Request Follow-Up messages transmitted on the gPTP port */
    ieee8021AsPortStatTxPdelayResponseFollowUp,
    /*! This counter is not used, no Announce messages allowed */
    ieee8021AsPortStatTxAnnounce,
}gptp_def_counters_t;
#endif /* GPTP_COUNTERS */
/*! @} */

/*!
 * @name gPTP stack INTERNAL enums
 * @{
 */

/*!
 * @brief Platform timebases
 */
typedef enum
{
    /*! Timestamp from the freerunning timebase */
    GPTP_DEF_TS_FREERUNNING,
    /*! Timestamp from the corrected (synchronized to Grand Master) timebase */
    GPTP_DEF_TS_CORRECTED
}gptp_def_ts_type_t;

/*!
 * @brief State machines API call types
 */
typedef enum
{
    /*! Not used */
    GPTP_DEF_CALL_BY_MISTAKE = 0,
    /*! Initiate the State machine operation */
    GPTP_DEF_CALL_INITIATE,
    /*! State machine action upon the ptp message reception */
    GPTP_DEF_CALL_PTP_RECEIVED,
    /*! State machine action upon the timestamp reception */
    GPTP_DEF_CALL_ON_TIMESTAMP
}gptp_def_sm_call_type_t;

/*!
 * @brief Timestamp vs. buffer index map entry status  
 */
typedef enum
{
    /*! Entry not used */
    GPTP_DEF_TS_MAP_ENTRY_UNUSED = 0,
    /*! Entry enqueued */ 
    GPTP_DEF_TS_MAP_ENTRY_ENQUEUED,
    /*! Entry confirmed */ 
    GPTP_DEF_TS_MAP_ENTRY_CNFRMD
}gptp_def_ts_map_status_type_t;

/*!
 * @brief PTP message types carried in the PTP message
 */
typedef enum
{
    /*! Sync message type */
    GPTP_DEF_MSG_TYPE_SYNC = 0x00,
    /*! Pdelay request message type */
    GPTP_DEF_MSG_TYPE_PD_REQ = 0x02,
    /*! Pdelay response message type */
    GPTP_DEF_MSG_TYPE_PD_RESP = 0x03,
    /*! Sync followup message type */
    GPTP_DEF_MSG_TYPE_FOLLOW_UP = 0x08,
    /*! Pdelay response folloup message type */
    GPTP_DEF_MSG_TYPE_PD_RESP_FUP = 0x0a,
    /*! Announce message type */
    GPTP_DEF_MSG_TYPE_ANNOUNCE = 0x0b,
    /*! Signaling message type */
    GPTP_DEF_MSG_TYPE_SIGNALING = 0x0c,
    /*! Unknown message type */
    GPTP_DEF_MSG_TYPE_UNKNOWN = 0xff
}gptp_def_msg_type_t;

/*!
 * @brief Pdelay inititator state machine states
 */
typedef enum
{
    /*! Pdelay initiator not enabled */
    GPTP_DEF_INITIATOR_NOT_ENABLED = 0,
    /*! Initiation of the Pdelay request transmission */
    GPTP_DEF_INIIAL_SEND_PD_REQ,
    /*! Waiting for pdelay request */
    GPTP_DEF_WAITING_FOR_PD_RESP,
    /*! Waiting for pdelay response followup */
    GPTP_DEF_WAITING_FOR_PD_RESP_F,
    /*! Waiting for pdelay interval */
    GPTP_DEF_WAITING_FOR_PD_INTERV,
    /*! Transmit pdelay request */
    GPTP_DEF_SEND_PDELAY_REQ,
    /*! Pdelay state machine reset */
    GPTP_DEF_PDELAY_RESET
}gptp_def_pdel_init_state_t;

/*!
 * @brief Pdelay responder state machine states
 */
typedef enum
{
    /*! Pdelay responder not enabled */
    GPTP_DEF_RESPONDER_NOT_ENABLED = 0,
    /*! Machine waiting for Pdelay request */
    GPTP_DEF_INIT_WAITING_FOR_PD_R,
    /*! Pdelay response sent, waiting for timestamp */
    GPTP_DEF_SENT_PD_RESP_WTING_4_T,
    /*! Waiting for Pdelay request */
    GPTP_DEF_WAITING_FOR_PDELAY_REQ
}gptp_def_pdel_resp_state_t;

/*!
 * @brief Sync state machine states - master
 */
typedef enum
{
    /*! Sync master not enabled */
    GPTP_DEF_SYNC_MASTER_NOT_EN = 0,
    /*! Sync master initialization */
    GPTP_DEF_SYNC_MASTER_INITING,
    /*! Sync master sync message sent */
    GPTP_DEF_SYNC_MASTER_SEND_SYNC,
    /*! Sync master followup message sent */
    GPTP_DEF_SYNC_MASTER_SEND_FUP
}gptp_def_sync_master_state_t;

/*!
 * @brief Sync state machine states - slave
 */
typedef enum
{
    /*! Sync slave not enabled */
    GPTP_DEF_SYNC_SLAVE_NOT_EN = 0,
    /*! Sync slave sync message discard */
    GPTP_DEF_SYNC_SLAVE_DISCARD,
    /*! Sync slave waiting for sync message */
    GPTP_DEF_SYNC_SLAVE_WT_4_FUP,
    /*! Sync slave waiting for followup message */
    GPTP_DEF_SYNC_SLAVE_WT_4_SYNC
}gptp_def_sync_slave_state_t;

/*!
 * @brief Synchronization lock states.
 */
typedef enum
{
    /*! Unlocked. */
    GPTP_SYN_UNLOCK,
    /*! Locked. */
    GPTP_SYN_LOCK
}gptp_def_syn_lock_state_type_t;
/*! @} */

/*!
 * @name gPTP stack data structures visible in the API
 * @{
 */
 /*!
 * @brief NVM writer machine states
 */
typedef enum
{
    /*! NVM writing stopped */
    GPTP_DEF_MEM_WRITE_STOP,
    /*! NVM writing initialized */
    GPTP_DEF_MEM_WRITE_INIT,
    /*! NVM writing finish */
    GPTP_DEF_MEM_WRITE_FINISH
}gptp_def_mem_write_stat;

 /*!
 * @brief NVM Data type
 */
typedef enum
{
    /*! NVM data Path delay */
    GPTP_DEF_NVM_PDELAY,
    /*! NVM data Rate ratio */
    GPTP_DEF_NVM_RRATIO
}gptp_def_nvm_data_t;

/*!
 * @brief Clock and port ID structure
 */
typedef struct
{
    /*! Clock ID information */
    uint64_t                            u64ClockId;
    /*! Port ID information */
    uint16_t                            u16PortId;
}gptp_def_clock_port_id_t;

/*!
 * @brief Time information data structure - unsigned
 */
typedef struct
{
    /*! Seconds part of the time information - uint64_t */
    uint64_t                            u64TimeStampS;
    /*! Nanoseconds part of the time information - uint32_t */
    uint32_t                            u32TimeStampNs;
}gptp_def_timestamp_t;

/*!
 * @brief Time information data structure - signed
 */
typedef struct
{
    /*! Seconds part of the time information - int64_t */
    int64_t                             s64TimeStampS;
    /*! Nanoseconds part of the time information - int32_t */
    int32_t                             s32TimeStampNs;
}gptp_def_timestamp_sig_t;

/*!
 * @brief The structure gathering RX data
 */
typedef struct
{
    /*! gPTP port received the message */
    uint8_t                             u8PtpPort;
    /*! Ingress timestamp - seconds */
    uint32_t                            u32TsSec;
    /*! Ingress timestamp - nano seconds */
    uint32_t                            u32TsNsec;
    /*! Pointer to the received gPTP data */
    const uint8_t                       *cpu8RxData;
    /*! Ethertype */
    uint16_t                            u16EthType;
    /* Source MAC address. */
    uint64_t                            u64SourceMac;
}gptp_def_rx_data_t;
/*! @} */

/*!
 * @name gPTP Time Validation structures
 * @{
 */
#ifdef GPTP_TIME_VALIDATION
/*!
 * @brief Time Validation Sync Slave measuremetn data structure
 */
typedef struct
{
    /*! Sequence Id of received Sync frame */
    uint16_t                            u16SequenceId;
    /*! sourcePortId from received Sync frame */
    gptp_def_clock_port_id_t            rSourcePortId;
    /*! Ingress timestamp of Sync frame converted to Virtual Local Time */
    gptp_def_timestamp_t                rSyncIngressTimestamp;
    /*! preciseOriginTimestamp taken from the received Follow_Up frame */
    gptp_def_timestamp_t                rPreciseOriginTimestamp;
    /*! correctionField taken from the received Follow_Up frame */
    int64_t                             s64CorrectionField;
    /*! Currently valid pDelay value */
    uint32_t                            u32Pdelay;
    /*! SyncLocal Time Tuple (Virtual Local Time part) */
    gptp_def_timestamp_t                rReferenceLocalTimestamp;
    /*! SyncLocal Time Tuple (Global Time part) */
    gptp_def_timestamp_t                rReferenceGlobalTimestamp;
}gptp_def_time_slave_measure_t;

/*!
 * @brief Time Validation Sync Master measurement data structure
 */
typedef struct
{
    /*! sequenceId of sent Ethernet frame */
    uint16_t                            u16SequenceId;
    /*! sourcePortId of sending Ethernet port */
    gptp_def_clock_port_id_t            rSourcePortId;
    /*! Egress timestamp of Sync frame */
    gptp_def_timestamp_t                rSyncEgressTimestamp;
    /*! The preciseOriginTime as copied to the Follow_Up frame */
    gptp_def_timestamp_t                rPreciseOriginTimestamp;
    /*! The correctionField as copied to the Follow_Up frame */
    int64_t                             s64CorrectionField;
}gptp_def_time_master_measure_t;

/*!
 * @brief Time Validation Pdelay initiator measurement data structure
 */
typedef struct
{
    /*! Sequence Id of sent Pdelay_Req frame */
    uint16_t                            u16SequenceId;
    /*! sourcePortId of sent Pdelay_Req frame */
    gptp_def_clock_port_id_t            rRequestPortId;
    /*! sourcePortId of received Pdelay_Resp frame */
    gptp_def_clock_port_id_t            rResponsePortId;
    /*! Egress timestamp of Pdelay_Req in Virtual Local Time */
    gptp_def_timestamp_t                rRequestOriginTimestamp;
    /*! Ingress timestamp of Pdelay_Resp in Virtual Local Time */
    gptp_def_timestamp_t                rResponseReceiptTimestamp;
    /*! Ingress timestamp of Pdelay_Req in Global Time taken from the received Pdelay_Resp */
    gptp_def_timestamp_t                rRequestReceiptTimestamp;
    /*! Egress timestamp of Pdelay_Resp in Global Time taken from the received Pdelay_Resp_Follow_Up */
    gptp_def_timestamp_t                rResponseOriginTimestamp;
    /*! Value of the Virtual Local Time of the reference Global Time Tuple */
    gptp_def_timestamp_t                rReferenceLocalTimestampInit;
    /*! Value of the local instance of the Global Time of the reference Global Time Tuple */
    gptp_def_timestamp_t                rReferenceGlobalTimestampInit;
    /*! Currently valid Pdelay value */
    uint32_t                            u32Pdelay;
}gptp_def_pdelay_init_meas_t;

/*!
 * @brief Time Validation Pdelay responder measurement data structure
 */
typedef struct
{
    /*! sequenceId of received Pdelay_Req frame */
    uint16_t                            u16SequenceId;
    /*! sourcePortId of received Pdelay_Req frame */
    gptp_def_clock_port_id_t            rRequestPortId;
    /*! sourcePortId of sent Pdelay_Resp frame */
    gptp_def_clock_port_id_t            rResponsePortId;
    /*! Ingress timestamp of Pdelay_Req converted to Virtual Local Time */
    gptp_def_timestamp_t                rRequestReceiptTimestamp;
    /*! Egress timestamp of Pdelay_Resp converted to Virtual Local Time */
    gptp_def_timestamp_t                rResponseOriginTimestamp;
    /*! Value of the Virtual Local Time of the reference Global Time Tuple used to convert requestReceiptTimestamp and responseOriginTimestamp into Global Time */
    gptp_def_timestamp_t                rReferenceLocalTimestampInit;
    /*! Value of the local instance of the Global Time of the reference Global Time Tuple used to convert requestReceiptTimestamp and responseOriginTimestamp into Global Time */
    gptp_def_timestamp_t                rReferenceGlobalTimestampInit;
}gptp_def_pdelay_resp_meas_t;
#endif /* GPTP_TIME_VALIDATION */
/*! @} */

/*!
 * @name Callback function typedefs
 * @{
 */
/*!
 * @brief Domain selection callback function
 */
typedef bool (*gptp_domain_sel_callback_t)(uint8_t u8DomainNumber, float64_t *pf64RateRatio, gptp_def_timestamp_sig_t *prTime, bool *pbNegativeOfst, int8_t *ps8SyncIntervalLog);
/*!
 * @brief Callback function for NVM write access
 */
typedef uint8_t (*gptp_nvm_write_callback_t)(uint8_t u8PdelayMachine, gptp_def_nvm_data_t eNvmDataType, float64_t f64Value, gptp_def_mem_write_stat *peWriteStat);
/*!
 * @brief Callback function for NVM read access
 */
typedef uint8_t (*gptp_nvm_read_callback_t)(uint8_t u8PdelayMachine, gptp_def_nvm_data_t eNvmDataType, float64_t *pf64Value);
/*!
 * @brief Callback function for error event notification
 */
typedef void (*gptp_err_notify_t)(void);
/*!
 * @brief Callback function for synchronization notification.
 */
typedef void (*gptp_syn_notify_t)(gptp_def_syn_lock_state_type_t eSyncLockState);

#ifdef GPTP_TIME_VALIDATION
/*!
 * @brief Time Validation callback function, reporting Sync Slave data
 */
typedef void (*gptp_def_t_b_notif_slave)(uint8_t u8DomainNumber, gptp_def_time_slave_measure_t rTimeSlaveData);
/*!
 * @brief Time Validation callback function, reporting Sync Master data
 */
typedef void (*gptp_def_t_b_notif_master)(uint8_t u8DomainNumber, gptp_def_time_master_measure_t rTimeMasterData);
/*!
 * @brief Time Validation callback function, reporting Pdelay initiator data
 */
typedef void (*gptp_def_s_p_notif_init)(gptp_def_pdelay_init_meas_t rPdelayInitData);
/*!
 * @brief Time Validation callback function, reporting Pdelay responder data
 */
typedef void (*gptp_def_s_p_notif_resp)(gptp_def_pdelay_resp_meas_t rPdelayRespData);
#endif /* GPTP_TIME_VALIDATION */

/*!
 * @brief Data structure concentrates all gPTP stack callback function pointers
 */
typedef struct
{
    /*! Callback function for multidomain operation. The function allows to select domain to clock update */
    gptp_domain_sel_callback_t          pfDomSelectionCB;
    /*! Callback function for NVM write access */
    gptp_nvm_write_callback_t           pfNvmWriteCB;
    /*! Callback function for NVM read access */
    gptp_nvm_read_callback_t            pfNvmReadCB;
    /*! Callback function for Error notification */
    gptp_err_notify_t                   pfErrNotify;
    /*! Callback function for synchronization notification. */
    gptp_syn_notify_t                   pfSynNotify;
#ifdef GPTP_TIME_VALIDATION
    /*! Callback function for Time Validation notification - slave */
    gptp_def_t_b_notif_slave            pfTimeBaseProvideNotifSlave;
    /*! Callback function for Time Validation notification - master */
    gptp_def_t_b_notif_master           pfTimeBaseProvideNotifMaster;
    /*! Callback function for Pdelay Initiator notification */
    gptp_def_s_p_notif_init             pfSetPdelayInitiatorData;
    /*! Callback function for Pdelay Responder notification */
    gptp_def_s_p_notif_resp             pfSetPdelayResponderData;
#endif /* GPTP_TIME_VALIDATION */
}gptp_def_cb_funcs_t;
/*! @} */

/*!
 * @name gPTP stack INTERNAL structs
 * @{
 */
#ifdef GPTP_COUNTERS
/*!
 * @brief gPTP INTERNAL stucture - gPTP port mapping to the ports on switch
 */
typedef struct
{
    /*! Array of uint32_t - port counters */
    uint32_t    u32CountersPort[GPTP_COUNTERS_PORT_CNT];
}gptp_def_countes_port_t;
/*!
 * @brief gPTP INTERNAL stucture - gPTP port mapping to the ports on switch
 */
typedef struct
{
    /*! Array of uint32_t - domain counters */
    uint32_t    u32CountersDom[GPTP_COUNTERS_DOM_CNT];
}gptp_def_countes_dom_t;
#endif /* GPTP_COUNTERS */

/*!
 * @brief gPTP INTERNAL stucture - port mapping to the ports on switch
 */
typedef struct
{
    /*! Port number on the switch */
    uint8_t                             u8SwitchPort;
    /*! Switch number */
    uint8_t                             u8Switch;
}gptp_def_map_table_t;

/*!
 * @brief gPTP INTERNAL stucture - frame ID mapping to the buffer index
 */
typedef struct
{
    /*! gPTP frame ID */
    uint8_t                             u8PtpFrameId;
    /*! TX mechanism buffer index */
    uint32_t                            u32BufferIndex;
    /*! Egress timestamp placeholder */
    uint32_t                            u32EgressTimeStampSeconds;
    /*! Egress timestamp placeholder */
    uint32_t                            u32EgressTimeStampNanoseconds;
    /*! Egress port */
    uint8_t                             u8EgressPort;
    /*! Timestamp map entry status */
    gptp_def_ts_map_status_type_t       eTsEntryStatus;
    /*! Pointer to management object used in Autosar EthSwt switch management. */
    void                                *MgmtObjectPtr;
}gptp_def_tx_frame_map_t;

/*!
 * @brief gPTP INTERNAL stucture - product details
 */
typedef struct
{
    /*! Pointer to manufacturer ID string */
    const uint8_t                       *cpu8ManId;
    /*! Pointer to product revision string */
    const uint8_t                       *cpu8ProdRev;
    /*! Array of product description */
    uint8_t                             au8ProdDesc[GPTP_DEF_PROD_DESC_LENGTH];
}gptp_def_prod_detail_t;

/*!
 * @brief gPTP INTERNAL stucture - port map information
 */
typedef struct
{
    /*! Number of gPTP ports */
    uint8_t                             u8NumOfGptpPorts;
    /*! Pointer to port map table */
    const gptp_def_map_table_t          *prMapTable;
}gptp_def_map_info_t;

/*!
 * @brief gPTP INTERNAL stucture - frame ID details
 */
typedef struct
{
    /*! Message type */
    gptp_def_msg_type_t                 eMsgType;
    /*! Sequence ID */
    uint16_t                            u16SequenceId;
    /*! Port number */
    uint8_t                             u8Port;
    /*! Machine ID  */
    uint8_t                             u8MachineId;
    /*! Domain ID */
    uint8_t                             u8DomainId;
    /*! Sync machine acting as GM - flag */
    bool                                bSyncActingGm;
}gptp_def_frame_id_element_t;

/*!
 * @brief gPTP INTERNAL stucture - MAC address
 */
typedef struct
{
    /*! Dummy bits */
    uint64_t                            u16Dummy:16;
    /*! MAC address  */
    uint64_t                            u48Mac:48;
}gptp_def_eth_mac_t;

/*!
 * @brief gPTP INTERNAL stucture - PTP message header data
 */
typedef struct
{
    /*! Message ID */
    gptp_def_msg_type_t                 eMsgId;
    /*! Correction time in sub ns */
    uint64_t                            u64CorrectionSubNs;
    /*! Source clock ID */
    uint64_t                            u64SourceClockId;
    /*! Source port ID */
    uint16_t                            u16SourcePortId;
    /*! Sequence ID */
    uint16_t                            u16SequenceId;
    /*! Message period in log format */
    int8_t                              s8MessagePeriodLog;
}gptp_def_msg_header_t;

/*!
 * @brief gPTP INTERNAL stucture - Sync message content
 */
typedef struct
{
    /*! Structure containing message header information */
    gptp_def_msg_header_t               rHeader;
    /*! Vlan information */
    uint16_t                            u16VlanTci;
    /*! Sync RX timestamp */
    gptp_def_timestamp_t                rSyncRxTs;
}gptp_def_sync_data_t;

/*!
 * @brief gPTP INTERNAL stucture - Pdelay request message content
 */
typedef struct
{
    /*! Structure containing message header information */
    gptp_def_msg_header_t               rHeader;
    /*! T2 timestamp */
    gptp_def_timestamp_t                rT2Ts;
    /*! Major SDO ID */
    uint8_t                             u8MajorSdoId;
    /*! Subdomain number */
    uint8_t                             u8SubdomainNum;
    /*! Vlan information */
    uint16_t                            u16VlanTci;
}gptp_def_pdel_req_data_t;

/*!
 * @brief gPTP INTERNAL stucture - Pdelay response message content
 */
typedef struct
{
    /*! Structure containing message header information */
    gptp_def_msg_header_t               rHeader;
    /*! T2 timestamp */
    gptp_def_timestamp_t                rT2Ts;
    /*! T4 timestamp */
    gptp_def_timestamp_t                rT4Ts;
    /*! Requesting port ID */
    gptp_def_clock_port_id_t            rRequestingId;
}gptp_def_pdel_resp_data_t;

/*!
 * @brief gPTP INTERNAL stucture - Pdelay response follow-up message content
 */
typedef struct
{
    /*! Structure containing message header information */
    gptp_def_msg_header_t               rHeader;
    /*! T3 timestamp */
    gptp_def_timestamp_t                rT3Ts;
    /*! Requesting port ID */
    gptp_def_clock_port_id_t            rRequestingId;
}gptp_def_pdel_resp_fup_data_t;

/*!
 * @brief gPTP INTERNAL stucture - Follow-up message content
 */
typedef struct
{
    /*! Structure containing message header information */
    gptp_def_msg_header_t               rHeader;
    /*! Sync TX timestamp */
    gptp_def_timestamp_t                rSyncTxTs;
    /*! Cumulative rate ratio */
    float64_t                           f64RateRatio;
}gptp_def_fup_data_t;

/*!
 * @brief gPTP INTERNAL stucture - TX data
 */
typedef struct
{
    /*! Message ID */
    gptp_def_msg_type_t                 eMsgId;
    /*! Rate Ratio */
    float64_t                           f64RateRatio;
    /*! Correction time in sub ns */
    uint64_t                            u64CorrectionSubNs;
    /*! Tx timestamp */
    gptp_def_timestamp_t                rTs;
    /*! Requesting port ID */
    gptp_def_clock_port_id_t            rRequesting;
    /*! Source port ID */
    gptp_def_clock_port_id_t            rSrcPortID;
    /*! Vlan information */
    uint16_t                            u16VlanTci;
    /*! Sequence ID */
    uint16_t                            u16SequenceId;
    /*! Subdomain number */
    uint8_t                             u8SubdomainNumber;
    /*! Frame Length */
    uint8_t                             u8FrameLength;
    /*! Message period in log format */
    int8_t                              s8LogMessagePeriod;
    /*! Flag if the TS is requested upon transmission */
    bool                                bTsRequested;
    /*! Flag of the Brifge acting GM */
    bool                                bSyncActingGm;
    /*! Pointer to array of TX buffer */
    uint8_t                             *pau8TxBuffPtr;
    /*! Pointer to TX Cmd structure */
    void                                *pvTxCmd;
    /*! Pointer to Buffer index map for corresponding machine */
    gptp_def_tx_frame_map_t             *prFrameMap;
    /* Destination MAC address. */
    uint64_t                            u64DestMac;
}gptp_def_tx_data_t;

/*!
 * @brief gPTP INTERNAL stucture - Pdelay machine
 */
typedef struct
{
    /*! gPTP port number */
    uint8_t                             u8GptpPort;
    /*! Pointer to port enabled flag */
    bool                                *pbPortEnabled;
    /*! flag if to compute Neighbor Rate Ratio */
    bool                                bComputeNeighborRateRatio;
    /*! flag if to compute Neighbor Rate Ratio */
    bool                                bComputeNeighborPropDelay;
    /*! Pdelay request received flag */
    bool                                bPdelayReqReceived;
    /*! Pdelay response received flag */
    bool                                bPdelayRespReceived;
    /*! Pdelay response follow-up received flag */
    bool                                bPdelayRespFupReceived;
    /*! T1 timestamp registered flag */
    bool                                bT1TsRegistred;
    /*! T3 timestamp registered flag */
    bool                                bT3TsRegistred;
    /*! Neighbor rate ratio valid - flag */
    bool                                bNeighborRateRatioValid;
    /*! Current neighbor rate ratio valid - flag */
    bool                                bNeighborRateRatioValidCurrent;
    /*! Rate ratio averager initialized - flag */
    bool                                bRatioAveragerInitialized;
    /*! Pdelay value is valid - flag */
    bool                                bPdelayValueValid;
    /*! Current pldelay value is valid - flag */
    bool                                bPdelayValueValidCurrent;
    /*! Prelay aferager initialized - flag */
    bool                                bPdelAveragerInitialized;
    /*! Pdelay initiator enabled - flag */
    bool                                bPdelayInitiatorEnabled;
    /*! Pdelay initiator enabled - previous state flag */
    bool                                bPdelayInitiatorEnabledPrev;
    /*! Respond to a Pdelay request with responder's (unicast) instead of
        multicast MAC address. */
    bool                                bPdelayUnicastResp;
    /*! Pdelay responder enabled - flag */
    bool                                bPdelayResponderEnabled;
    /*! Pdelay timer enabled */
    bool                                bTimerPdelayTimerEnabled;
    /*! Structure containing Pdelay request message RX data */
    gptp_def_pdel_req_data_t            rPdReqMsgRx;
    /*! Structure containing Pdelay response message RX data */
    gptp_def_pdel_resp_data_t           rPdRespMsgRx;
    /*! Structure containing Pdelay response followup message RX data */
    gptp_def_pdel_resp_fup_data_t       rPdRespFupMsgRx;
    /*! Squence ID of T1 timestamp */
    uint16_t                            u16T1TsSeqId;
    /*! Pdelay initiator state machine state */
    gptp_def_pdel_init_state_t          ePdelayInitiatorState;
    /*! Pdelay initiator state machine state */
    gptp_def_pdel_resp_state_t          ePdelayResponderState;
    /*! State machine for NVM writing Pdelay */
    gptp_def_mem_write_stat             rPdelayNvmWriteStat;
    /*! State machine for NVM writing Rate Ratio */
    gptp_def_mem_write_stat             rRratioNvmWriteStat;
    /*! Structure containing Pdelay response follow-up TX data */
    gptp_def_tx_data_t                  rTxDataReFu;
    /*! Structure containing Pdelay response TX data */
    gptp_def_tx_data_t                  rTxDataResp;
    /*! Structure containing Pdelay request TX data */
    gptp_def_tx_data_t                  rTxDataReq;
    /*! Sequence ID of T3 timestamp */
    uint16_t                            u16T3TsSeqId;
    /*! Pdelay response sequence ID */
    uint16_t                            u16RespSeqId;
    /*! Count of lost responses */
    uint16_t                            u16LostResponses;
    /*! Maximum allowed lost responses */
    uint16_t                            u16PdelLostReponsesAllowedCnt;
    /*! Count of pdelay measurements till slow donw */
    uint16_t                            u16MsmtsTillSlowDown;
    /*! Pdelay responses count */
    uint16_t                            u16PdelayResponsesCnt;
    /*! Vlan information */
    uint16_t                            u16VlanTci;
    /*! Sequence ID */
    uint16_t                            u16SequenceId;
    /*! NVM storage physical address for Pdelay value */
    uint32_t                            u32NvmAddressPdelay;
    /*! NVM storage physical address for Rate ratio. */
    uint32_t                            u32NvmAddressRratio;
    /*! T1 timestamp value */
    gptp_def_timestamp_t                rT1Ts;
    /*! T3 timestamp value */
    gptp_def_timestamp_t                rT3Ts;
    /*! Previous T1 timestamp */
    gptp_def_timestamp_t                rNeighborCalcPrevT1;
    /*! Previous T2 timestamp */
    gptp_def_timestamp_t                rNeighborCalcPrevT2;
    /*! Neighbor rate ratio - filtered */
    float64_t                           f64NeighborRateRatio;
    /*! Current result of Neighbor rate ratio */
    float64_t                           f64NeighborRateRatioCurrent;
    /*! Temporary Rate ratio - current */
    float64_t                           f64TempRateRatioCurrent;
    /*! Neighbor propagation delay - filtered */
    float64_t                           f64NeighborPropDelay;
    /*! Current result of Neighbor propagation delay */
    float64_t                           f64NeighborPropDelayCurrent;
    /*! Pdelay value to write into the NVM */
    float64_t                           f64PdelayToWrite;
    /*! Rate ratio value to write into the NVM */
    float64_t                           f64RratioToWrite;
    /*! Pointer to clock ID */
    uint64_t                            *pu64ClockId;
    /*! Array Pdelay request message buffer */
    uint8_t                             au8SendBuffPdelReq[GPTP_DEF_TX_BUF_LEN_PD_RQ];
    /*! Initial pdelay interval in log format */
    int8_t                              s8InitPdelayIntervalLog;
    /*! Array Pdelay response message buffer */
    uint8_t                             au8SendBuffPdelResp[GPTP_DEF_TX_BUF_LEN_PD_RSP];
    /*! Operational pdelay interval in log format */
    int8_t                              s8OperPdelayIntervalLog;
    /*! Array Pdelay response follow-up message buffer */
    uint8_t                             au8SendBuffPdelRespFup[GPTP_DEF_TX_BUF_LEN_PD_FUP];
    /*! Current pdelay interval in log format */
    int8_t                              s8PdelayIntervalLog;
    /*! Pdelay timer Pdelay request send due */
    uint64_t                            u64TimerPdelaySendDue;
    /*! Neighbor propagation delay threshold */
    uint64_t                            u64NeighborPropDelayThreshNs;
    /*! Start time of turnaround measurement on the initiator side */
    uint64_t                            u64TurnaroundMeasStartPtpNsInit;
    /*! Start time of turnaround measurement on the responder side */
    uint64_t                            u64TurnaroundMeasStartPtpNsResp;
    /* Source MAC address. */
    uint64_t                            u64SourceMac;
#ifdef GPTP_TIME_VALIDATION
    /*! Local timestamp of Time validation on initiator side */
    gptp_def_timestamp_t                rReferenceLocalTimestampInit;
    /*! Global timestamp of Time validation on initiator side */
    gptp_def_timestamp_t                rReferenceGlobalTimestampInit;
    /*! Local timestamp of Time validation on responder side */
    gptp_def_timestamp_t                rReferenceLocalTimestampResp;
    /*! Global timestamp of Time validation on responder side */
    gptp_def_timestamp_t                rReferenceGlobalTimestampResp;
#endif /* GPTP_TIME_VALIDATION */
    /*! Delay asymmetry for corresponding port [ns] */
    int16_t                             s16DelayAsymmetry;
}gptp_def_pdelay_t;

/*!
 * @brief gPTP INTERNAL stucture - Sync machine
 */
typedef struct
{
    /*! Destination MAC address */
    gptp_def_eth_mac_t                  rDestMac;
    /*! Follow-up message RX data */
    gptp_def_fup_data_t                 rFupMsgRx;
    /*! Store correctly received timestamp */
    gptp_def_timestamp_t                rPrecOrigTsRcvd;
    /*! Sync message RX data */
    gptp_def_sync_data_t                rSyncMsgRx;
    /*! gPTP port number */
    uint8_t                             u8GptpPort;
    /*! Initial sync interval in log format */
    int8_t                              s8InitSyncIntervalLog;
    /*! Operating sync interval in log format */
    int8_t                              s8OperSyncIntervalLog;
    /*! Pointer to port enabled flag  */
    bool                                *pbPortEnabled;
    /*! Sync message received - flag */
    bool                                bSyncMsgReceived;
    /*! Follow-up message received - flag */
    bool                                bFupMsgReceived;
    /*! TS timestamp registered - flag */
    bool                                bTsTsRegistred;
    /*! Sync role flag - master or slave */
    bool                                bSyncRole;
    /*! Sync machine is acting GM - flag */
    bool                                bSyncActingGm;
    /*! TR timestamp registered - flag */
    bool                                bTrTsRegistred;
    /*! Timer for Sync tranmission enabled - flag */
    bool                                bTimerSyncEnabled;
    /*! Sync machine Master state */
    gptp_def_sync_master_state_t        eSyncMasterState;
    /*! Sync machine Slave state */
    gptp_def_sync_slave_state_t         eSyncSlaveState;
    /*! TS timestamp value */
    gptp_def_timestamp_t                rTsTs;
    /*! TR timestamp value */
    gptp_def_timestamp_t                rTrTs;
    /*! Upstream TX time in timestamp format */
    gptp_def_timestamp_t                rUpstreamTxTime;
    /*! Correction value in sub ns */
    uint64_t                            u64CorrectionSubNs;
    /*! Upstream TX time - local time of GM timestamp transmission */
    uint64_t                            u64UpstreamTxTime;
    /*! Log of last sync transmission time */
    uint64_t                            u64TimerLastSyncSent;
    /*! Sync transmission due */
    uint64_t                            u64TimerSyncSendDue;
    /*! Residence measurement start time */
    uint64_t                            u64ResidenceMeasStartPtpNs;
    /*! Pointer to neighbor propagation delay for corresponding port */
    float64_t                           *pf64NeighborPropDelay;
    /*! Pointer to neighbor rate ratio for corresponding port */
    float64_t                           *pf64NeighborRateRatio;
    /*! Rate ratio */
    float64_t                           f64RateRatio;
    /*! Structure containing data for follow-up transmission */
    gptp_def_tx_data_t                  rTxDataFup;
    /*! Structure containing data for sync transmission */
    gptp_def_tx_data_t                  rTxDataSyn;
    /*! Propagatio ndelay in */
    uint32_t                            u32MeanPropDelay;
    /*! Sync send buffer */
    uint8_t                             au8SendBuffSync[GPTP_DEF_TX_BUF_LEN_SYNC];
    /*! TR timestamp sequence ID */
    uint16_t                            u16TsTsSeqId;
    /*! Count of currently skipped syncs if the transmission is slower than reception */
    uint8_t                             u8SkippedSyncs;
    /*! Follow-up send buffer */
    uint8_t                             au8SendBuffFup[GPTP_DEF_TX_BUF_LEN_FUP];
    /*! Sequence ID */
    uint16_t                            u16SequenceId;
    /*! Current sync interval in log format */
    int8_t                              s8SyncIntervalLog;
    /*! Pointer to delay asymmetry for corresponding port [ns] */
    int16_t                             *ps16DelayAsymmetry;
}gptp_def_sync_t;

/*!
 * @brief gPTP INTERNAL stucture - PI Controller configuration structure
 */
typedef struct
{
    /*! Integral windup limit (max positive/negative output value of the I term) [PPB] - uint32_t */
    uint32_t                            u32IntegralWindupLimit;
    /*! The system dumping ratio is dimensionless quantity describing the loop attenuation - float64_t */
    float64_t                           f64DampingRatio;
    /*! The natural frequency ratio defines ration between system natural frequency and sampling frequency - float64_t */
    float64_t                           f64NatFreqRatio;
}gptp_def_pi_controller_conf_t;

/*!
 * @brief gPTP INTERNAL stucture - Synchronization notification parameters.
 */
typedef struct
{
    /*! Synchronization offset threshold. */
    int64_t i64SynTrigOfst;
    /*! Unsynchronization offset threshold. */
    int64_t i64UnsTrigOfst;
    /*! Synchronization consecutive occurrence limit. */
    uint16_t u16SynTrigCnt;
    /*! Unsynchronization consecutive occurrence limit. */
    uint16_t u16UnsTrigCnt;
}gptp_def_params_syn_lock_t;

/*!
 * @brief gPTP INTERNAL stucture - Per device parameters
 */
typedef struct
{
    /*! SDO ID compatibility mode - flag */
    bool                                bSdoIdCompatibilityMode;
    /*! Vlan enabled - flag */
    bool                                bVlanEnabled;
    /*! Last reported GM offset to the synchronization mechanism - negative flag */
    bool                                bReportedOffsetNeg;
    /*! Local clock ever updated to the GM time - flag */
    bool                                bEverUpdated;
    /*! Estimated offset of local clock to the GM clock calculated */
    bool                                bEstimGmOffsetCalculated;
    /*! Signaling mechanism enabled - flag */
    bool                                bSignalingEnabled;
    /*! Synchronized state - flag */
    bool                                bSynced;
    /*! Eth frame priority */
    uint8_t                             u8EthFramePrio;
    /*! Count of gPTP ports */
    uint8_t                             u8PortsCount;
    /*! Next Sync frame ID */
    uint8_t                             u8NextFrameIdSync;
    /*! Next Pdelay request frame ID */
    uint8_t                             u8NextFrameIdPdReq;
    /*! Next Pdelay response frame ID */
    uint8_t                             u8NextFrameIdPdResp;
    /*! Count of initialized domains */
    uint8_t                             u8NumberOfDomains;
    /*! Count of Pdelay machines */
    uint8_t                             u8NumberOfPdelayMachines;
    /*! Vlan information */
    uint16_t                            u16VlanTci;
    /*! Number of consecutive synchronizations. */
    uint16_t                            u16SynCnt;
    /*! Number of consecutive unsynchronizations. */
    uint16_t                            u16UnsCnt;
    /*! Pdelay filter weight */
    float64_t                           f64PdelAvgWeight;
    /*! Rate ratio filter weight */
    float64_t                           f64RratioAvgWeight;
    /*! Rate ratio maximum allowed deviation */
    float64_t                           f64RratioMaxDev;
    /*! Threshold for Pdelay writing into the NVM */
    float64_t                           f64PdelayNvmWriteThr;
    /*! Threshold for Rate ratio writing into the NVM */
    float64_t                           f64RratioNvmWriteThr;
    /*! Corrected clock rate ratio */
    float64_t                           f64CorrClockRateRatio;
    /*! Frame ID table carrying transmitted frames details */
    gptp_def_frame_id_element_t         arFrameIdTable[256];
    /*! Last reported GM offset to the synchronization mechanism */
    gptp_def_timestamp_sig_t            rReportedOffset;
    /*! Estimated offset of local clock to the GM clock */
    gptp_def_timestamp_sig_t            rEstimGmOffset;
    /*! Last free running clock reference for offset estimation calculation */
    gptp_def_timestamp_t                rLastFreeRunRef;
    /*! PI Controller configuration structure */
    gptp_def_pi_controller_conf_t       rPIControllerConfig;
    /*! Data structure containing all synchronization notification parameters. */
    gptp_def_params_syn_lock_t          rSynLockParam;
    /*! PI Controller maximum threshold - creates PI Controller action band */
    uint64_t                            u64PIControllerMaxThreshold;
}gptp_def_device_t;

/*!
 * @brief gPTP INTERNAL stucture -  Per port parameters
 */
typedef struct
{
    /*! Clock ID */
    uint64_t                            u64ClockId;
    /*! Source port MAC address */
    gptp_def_eth_mac_t                  rSourceMac;
    /*! gPTP port number */
    uint8_t                             u8PortId;
    /*! Port enabled (link up/down) - flag */
    bool                                bPortEnabled;
}gptp_def_port_t;

/*!
 * @brief gPTP INTERNAL stucture -  Per domain parameters
 */
typedef struct
{
    /*! Received request for Sync interval update */
    bool                                bSignalingIntervalRcvd;
    /*! Valid sync message received - flag */
    bool                                bValidSyncReceived;
    /*! Domain role - flag */
    bool                                bDomainIsGm;
    /*! If the sync is acting  */
    bool                                bSyncActingGm;
    /*! Synced GM - flag */
    bool                                bSyncedGm;
    /*! GM failure already reported - flag */
    bool                                bGmFailureReported;
    /*! Received sync has been used for local clock update - flag */
    bool                                bRcvdSyncUsedForLocalClk;
    /*! Valid sync message received since start - flag */
    bool                                bSyncValidEverReceived;
    /*! Domain index - configuration order */
    uint8_t                             u8DomainIndex;
    /*! Domain number - number on wire (0-255) */
    uint8_t                             u8DomainNumber;
    /*! Count of sync machines per domain */
    uint8_t                             u8NumberOfSyncsPerDom;
    /*! Slave machine ID - which sync machine is the slave one */
    uint8_t                             u8SlaveMachineId;
    /*! Reference domain for the Synced GM */
    uint8_t                             u8RefDomForSyncedGIndex;
    /*! Startup timeout in seconds */
    uint8_t                             u8StartupTimeoutS;
    /*! Count of missing Sync messages till the Sync receipt timeout  */
    uint8_t                             u8SyncReceiptTimeoutCnt;
    /*! Count of allowed outliers received */
    uint8_t                             u8OutlierIgnoreCnt;
    /*! Coutliers received */
    uint8_t                             u8OutlierCnt;
    /*! Signaling message sequence ID counter */
    uint16_t                            u16SigSequenceId;
    /*! GM timebase indicator */
    uint16_t                            u16GmTimeBaseIndicator;
    /*! Time of last Sync reception */
    uint64_t                            u64TimerLastSyncRcvd;
    /*! Last GM phase change - high */
    uint32_t                            u32LastGmPhaseChangeH;
    /*! Last GM phase change - low */
    uint64_t                            u64LastGmPhaseChangeL;
    /*! Scaled last GM frequency change */
    uint32_t                            u32ScaledLastGmFreqChange;
    /*! Sync status check due */
    uint64_t                            u64TimerSyncCheckDue;
    /*! Sync receipt timeout due */
    uint64_t                            u64TimerSyncReceiptTimeoutDue;
    /*! Threshold for the Sync outlier reception */
    uint32_t                            u32SyncOutlierThrNs;
    /*! Structure containing data for signaling transmission */
    gptp_def_tx_data_t                  rTxDataSig;
    /*! Signaling send buffer */
    uint8_t                             au8SendBuffSig[GPTP_DEF_TX_BUF_LEN_SIGNL];
    /*! Vlan information */
    uint16_t                            u16VlanTci;
    /*! Sync interval of incoming Sync messages */
    int8_t                              s8SyncIntervalLogRcvd;
    /*! Pointer to array od Sync machine structures */
    gptp_def_sync_t                     *prSyncMachines;
    /*! Last valid GM time plus correction */
    gptp_def_timestamp_t                rLastValidGmPlusCor;
    /*! Last valid Timestamp from GM plus correction */
    gptp_def_timestamp_t                rLastValidTsOfGmPlusCor;
    /*! Previously received precision origin timestamp */
    gptp_def_timestamp_t                rPrevPrecOrigTs;
#ifdef GPTP_COUNTERS
    /*! Pointer to array of domain specific counters */
    gptp_def_countes_dom_t              *prCntrsDom;
#endif /* GPTP_COUNTERS */
}gptp_def_domain_t;

/*!
 * @brief gPTP INTERNAL stucture - Main data structure containing all the instance data
 */
typedef struct
{
    /*! Structure containing all product detail information */
    gptp_def_prod_detail_t              rProductDetails;
    /*! Structure containing all device specific parameters */
    gptp_def_device_t                   rPerDeviceParams;
    /*! Pointer to structure containing all per port parameters */
    gptp_def_port_t                     *prPerPortParams;
    /*! Pointer to structure containing all per domain parameters */
    gptp_def_domain_t                   *prPerDomainParams;
    /*! Pointer to structure containing all Pdelay machine parameters */
    gptp_def_pdelay_t                   *prPdelayMachines;
    /*! Data structure containing pointers to all callback functions */
    gptp_def_cb_funcs_t                 rPtpStackCallBacks;
#ifdef GPTP_COUNTERS
    /*! Pointer to port specific counters */
    gptp_def_countes_port_t             *prCntrsPort;
#endif /* GPTP_COUNTERS */
    /*! Sync status calculation due */
    uint64_t                            u64SyncStatCalcDue;
    /*! Local clock is to be updated - flag */
    bool                                bLocClkToUpdate;
    /*! gPTP stack initialized - flag */
    bool                                bGptpInitialized;
}gptp_def_data_t;
/*! @} */

/*!
 * @name gPTP Initialization parameters structs
 * @{
 */
/*!
 * @brief gPTP Initialization - Pdelay state machine
 */
typedef struct
{
    /*! Pdelay initiator enabled = true / disabled = false */
    bool                                bPdelayInitiatorEnabled;
    /*! Respond to a Pdelay request with responder's (unicast) instead of
        multicast MAC address. */
    bool                                bPdelayUnicastResp;
    /*! Pdelay initial interval, in log. format. E.g., value -2 represents 250ms */
    int8_t                              s8PdelIntervalLogInit;
    /*! Pdelay operational interval in log format. E.g., value 0 represents 1s */
    int8_t                              s8PdelIntervalLogOper;
    /*! Neighbor propagation delay threshold [ns] */
    uint64_t                            u64NeighborPropDelayThreshNs;
    /*! Count of Pdelay measurement, till the interval is switched from initial to operational. */
    uint16_t                            u16MeasurementsTillSlowDown;
    /*! Count of allowed lost Pdelay responses */
    uint16_t                            u16PdelLostReponsesAllowedCnt;
    /*! Address of Pdelay record in the Non Volatile Memory */
    uint32_t                            u32NvmAddressPdelay;
    /*! Address of Rate Ratio record in the Non Volatile Memory */
    uint32_t                            u32NvmAddressRratio;
    /*! Clock ID of gPTP port. */
    uint64_t                            u64ClockId;
    /*! Pointer to the cmd structure for sending ETH frames */
    void                                *pvTxCmdReq;
    /*! Pointer to the cmd structure for sending ETH frames */
    void                                *pvTxCmdResp;
    /*! Pointer to the cmd structure for sending ETH frames */
    void                                *pvTxCmdReFu;
    /*! Pointer to Buffer index map for Pdelay Request on corresponding machine */
    gptp_def_tx_frame_map_t             *prFrameMapReq;
    /*! Pointer to Buffer index map for Pdelay Response on corresponding machine */
    gptp_def_tx_frame_map_t             *prFrameMapResp;
    /*! Pointer to Buffer index map for Pdelay Response FUP on corresponding machine */
    gptp_def_tx_frame_map_t             *prFrameMapReFu;
    /*! Delay asymmetry for corresponding port [ns] */
    int16_t                             s16DelayAsymmetry;
}gptp_def_init_params_pdel_t;

/*!
 * @brief gPTP Initialization - Sync state machine
 */
typedef struct
{
    /*! Index of gPTP port on which the state machine operates */
    uint8_t                             u8GptpPort;
    /*! The state machine role master = true / slave = false */
    bool                                bMachineRole;
    /*! Sync interval, in log. format. E.g., value -3 represents 125ms */
    int8_t                              s8SyncIntervalLog;
    /*! Pointer to the cmd structure for sending ETH frames */
    void                                *pvTxCmdSyn;
    /*! Pointer to the cmd structure for sending ETH frames */
    void                                *pvTxCmdFup;
    /*! Pointer to Buffer index map for Sync message on corresponding machine */
    gptp_def_tx_frame_map_t             *prFrameMapSync;
    /*! Pointer to Buffer index map for FUP message on corresponding machine */
    gptp_def_tx_frame_map_t             *prFrameMapFup;
}gptp_def_init_params_sync_t;

/*!
 * @brief gPTP Initialization - Domain parameters
 */
typedef struct
{
    /*! Domain number */
    uint8_t                             u8DomainNumber;
    /*! Count of Sync machines operating under the domain */
    uint8_t                             u8NumberOfSyncMachines;
    /*! Reference domain number for synced GM. The domain from which the synced GM takes Rate Ratio to the Grand Master.
    If reference to itself, the Rate Ratio is 1.0 */
    uint8_t                             u8RefDomForSynced;
    /*! Start-up time-out - time-out for non Grand Master, after which the domain starts acting as Grand Master, if not received sync message on slave port */
    uint8_t                             u8StartupTimeoutS;
    /*! Sync receipt time-out [count of missed syncs] - count of missed sync messages on the slave port after which the Error is raised */
    uint8_t                             u8SyncReceiptTimeoutCnt;
    /*! VLAN TCI */
    uint16_t                            u16VlanTci;
    /*! Threshold for the outlier time value received */
    uint32_t                            u32SyncOutlierThrNs;
    /*! Count of consecutive outliers to drop */
    uint8_t                             u8OutlierIgnoreCnt;
    /*! Synchronized Grand Master transmits Corrected time, instead of free running. */
    bool                                bSyncedGm;
    /*! Domain is Grand Master = true / domain is not Grand Master = false */
    bool                                bDomainIsGm;
    /*! Pointer to array of structure, containing initial parameters of the sync machines which belong to this domain */
    const gptp_def_init_params_sync_t   *pcrDomainSyncMachinesPtr;
    /*! Pointer to array of structure, containing all operational parameters of the sync machines, which belong to this domain */
    gptp_def_sync_t                     *prSync;
    /*! Pointer to the cmd structure for sending ETH frames */
    void                                *pvTxCmdSig;
    /*! Pointer to Buffer index map for Signal message */
    gptp_def_tx_frame_map_t             *prFrameMapSign;
#ifdef GPTP_COUNTERS
    /*! Pointer to domain counters array */
    gptp_def_countes_dom_t              *prCntrsDom;
#endif /* GPTP_COUNTERS */
}gptp_def_init_params_domain_t;

/*!
 * @brief gPTP Initialization - gPTP Stack parameters
 */
typedef struct
{
    /*! Priority of transmitted ETH Frame */
    uint8_t                             u8EthFramePrio;
    /*! Count of domains used in the application */
    uint8_t                             u8GptpDomainsCount;
    /*! VLAN TCI */
    uint16_t                            u16VlanTci;
    /*! Count of gPTP ports which corresponds to the count of Pdelay machines */
    uint8_t                             u8GptpPortsCount;
    /*! Compatibility with SdoID - if disabled, other "transport specific" than 0x1 is ignored. For the AVNU compliance must be disabled */
    bool                                bSdoIdCompatibilityMode;
    /*! Flag indicating, if the Signaling mechanism is enabled */
    bool                                bSignalingEnabled;
    /*! VLAN enabled */
    bool                                bVlanEnabled;
    /*! Pointer to array of structure - mapping of gPTP ports to the ports on switch */
    const gptp_def_map_table_t          *pcrPortMapTable;
    /*! Pointer to array of structure containing Pdelay initial parameters of Pdelay machines */
    const gptp_def_init_params_pdel_t   *pcrPdelayMachinesParam;
    /*! Pointer to array of structure containing Domain initial parameters of all domains */
    const gptp_def_init_params_domain_t *pcrDomainsSyncsParam;
    /*! Structure containing synchronization notification parameters. */
    gptp_def_params_syn_lock_t          rSynLockParam;
    /*! Pdelay averaging filter weight */
    float64_t                           f64PdelAvgWeight;
    /*! Rate ratio averaging filter weight */
    float64_t                           f64RratioAvgWeight;
    /*! Rate ratio maximum deviation */
    float64_t                           f64RratioMaxDev;
    /*! Pdelay NVM threshold - if current pdelay is different of the threshold, the value in the NVM is updated */
    float64_t                           f64PdelayNvmWriteThr;
    /*! Neighbor rate ratio NVM threshold - if current neighbor rate ration is different of the threshold, the value in the NVM is updated */
    float64_t                           f64RratioNvmWriteThr;
    /*! Pointer to array of structure - map of gPTP ports to switch ports */
    gptp_def_port_t                     *paPort;
    /*! Pointer to array of structure - internal gPTP Domain data */
    gptp_def_domain_t                   *paDomain;
    /*! Pointer to array of structure - internal gPTP Pdelay data */
    gptp_def_pdelay_t                   *paPdelay;
    /*! PI Controller configuration structure */
    gptp_def_pi_controller_conf_t       rPIControllerConfig;
    /*! PI Controller maximum threshold - creates PI Controller action band */
    uint64_t                            u64PIControllerMaxThreshold;
    /*! Data structure containing pointers to all callback functions */
    gptp_def_cb_funcs_t                 rPtpStackCallBacks;
#ifdef GPTP_COUNTERS
    /*! Pointer to port counters array */
    gptp_def_countes_port_t             *prCntrsPort;
#endif /* GPTP_COUNTERS */
}gptp_def_init_params_t;
/*! @} */

#endif /* GPTP_DEF */
/*******************************************************************************
 * EOF
 ******************************************************************************/
