/*
 * Copyright 2019-2023 NXP
 *
 * NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be
 * used strictly in accordance with the applicable license terms. By expressly
 * accepting such terms or by downloading, installing, activating and/or otherwise
 * using the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms.  If you do not agree to be
 * bound by the applicable license terms, then you may not retain, install,
 * activate or otherwise use the software.
 */

#ifndef GPTP_ERR
    #define GPTP_ERR

#include "gptp_def.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! Length of the error buffer */
#define GPTP_ERR_LOG_LENGTH            32u
/*! Port not specified constant */
#define GPTP_ERR_PORT_NOT_SPECIF       255u
/*! Domain not specified constant */
#define GPTP_ERR_DOMAIN_NOT_SPECIF     255u
/*! Sequence ID not specified constant */
#define GPTP_ERR_SEQ_ID_NOT_SPECIF     65535u

/*! @brief List of gPTP errors <p>
    Prefix GPTP_ERR_I, Initialize errors category<p>
    Prefix GPTP_ERR_A, Arithmetic errors category<p>
    Prefix GPTP_ERR_C, Conversion errors category<p>
    Prefix GPTP_ERR_T, Time-stamp errors category<p>
    Prefix GPTP_ERR_M, Message errors category<p>
    Prefix GPTP_ERR_N, NVM related error category<p>
    Prefix GPTP_ERR_P, Pdelay errors category<p>
    Prefix GPTP_ERR_S, Sync errors category<p>
    Prefix GPTP_ERR_L, Limit errors category<p>
    Prefix GPTP_ERR_F, Function call category<p>
    Prefix GPTP_ERR_API, API category<p>
    Prefix GPTP_ERR_V, Variable error category<p>
    Prefix GPTP_ERR_E, Error management category<p>
    Prefix GPTP_ERR_O, Outside situation category<p>*/
typedef enum
{
    /*! No error */
    GPTP_ERR_OK = 0,

    /* Prefix GPTP_ERR_I, Initialize errors category */
    /*! Stack not initialized */
    GPTP_ERR_I_NOT_INITIALIZED,
    /*! Count of initialized gPTP ports is zero */
    GPTP_ERR_I_NO_PORT_INITIALIZED,
    /*! Count of initialized Domains is zero */
    GPTP_ERR_I_NO_DOM_INITIALIZED,
    /*! Too many Sync slaves under one domain */
    GPTP_ERR_I_TOO_MANY_SLAV_DOM,
    /*! Too many Sync state machines under the domain (not enough gPTP ports) */
    GPTP_ERR_I_TOO_MANY_SYNCS_DOM,
    /*! No Slave machine for non Grand Master domain */
    GPTP_ERR_I_NO_SLAVE_NON_GM_DOM,
    /*! Slave Sync state machine initialized for Grand Master domain*/
    GPTP_ERR_I_SLAVE_ON_GM_DOMAIN,
    /*! Pdelay initiator not enabled for the slave sync machine */
    GPTP_ERR_I_NO_PDELAY_FOR_SLAVE,
    /*! Start-up time-out for Bridge is too long */
    GPTP_ERR_I_START_TOUT_TOO_LONG,
    /*! Source port MAC address not available */
    GPTP_ERR_I_NO_SOURCE_MAC,
    /*! Unable to get link status */
    GPTP_ERR_I_NO_LINK_STATUS,
    /*! Pointer to gPTP counters is NULL */
    GPTP_ERR_I_COUNTERS_NULL_PTR,
    /*! Pointer to port mapping structure is NULL */
    GPTP_ERR_I_PORT_MAP_NULL_PTR,
    /*! Pointer to port structure is NULL */
    GPTP_ERR_I_PORT_STRCT_NULL_PTR,
    /*! Pointer to Domain structure is NULL */
    GPTP_ERR_I_DOM_STRCT_NULL_PTR,
    /*! Pointer to Pdelay structure is NULL */
    GPTP_ERR_I_PD_STRCT_NULL_PTR,
    /*! Pointer to Sync structure under the Domain is NULL */
    GPTP_ERR_I_SYNC_STRCT_NULL_PTR,
    /*! Pdelay interval out of range */
    GPTP_ERR_I_PDEL_INTERVAL_OOR,
    /*! Sync interval out of range */
    GPTP_ERR_I_SYNC_INTERVAL_OOR,
    /*! Initialized Synchronized Grand Master on non Grand Master domain */
    GPTP_ERR_I_SYNCED_GM_ON_NON_GM,
    /*! The same domain number used twice */
    GPTP_ERR_I_DOM_NUM_USED_TWICE,
    /*! Reference domain for synced GM is not initialized */
    GPTP_ERR_I_DOM_TO_REF_NOT_INIT,

    /* Prefix GPTP_ERR_A, Arithmetic errors category */
    /*! Division by zero attempt */
    GPTP_ERR_A_DIV_BY_0_ATTEMPT,

    /* Prefix GPTP_ERR_C, Conversion errors category */
    /*! Times stamp is incorrect to perform the calculation */
    GPTP_ERR_C_TS_INCORRECT,

    /* Prefix GPTP_ERR_T, Timestamp errors category */
    /*! The time stamp is not registered */
    GPTP_ERR_T_TS_NOT_REGISTERED,
    /*! Unexpected time stamp received on the port */
    GPTP_ERR_T_TS_NOT_IDENTIFIED,
    /*! Incorrect Sync domain index (out of range) */
    GPTP_ERR_T_SYNC_DOMAIN_INDEX,
    /*! Incorrect Sync machine index (out of range) */
    GPTP_ERR_T_SYNC_MACHINE_INDEX,
    /*! Incorrect Pdelay machine index (out of range) */
    GPTP_ERR_T_PDEL_MACHINE_INDEX,
    /*! Incorrect frame id received for time-stamp handler*/
    GPTP_ERR_T_FRAME_ID_ERROR,

    /* Prefix GPTP_ERR_M, Message errors category */
    /*! Double reception of the Pdelay response */
    GPTP_ERR_M_PDEL_RESP_DBL_RCVD,
    /*! Unknown message type received */
    GPTP_ERR_M_MESSAGE_ID_INVALID,
    /*! The time stamp doesn't match with send message */
    GPTP_ERR_M_IDENTIFIERS_MISMATCH,
    /*! Transport specific byte is not 1 */
    GPTP_ERR_M_TRANSPORT_SPECIFIC,
    /*! Sync message received on master sync machine */
    GPTP_ERR_M_SYNC_ON_MASTER_RCVD,
    /*! Follow Up message received on master sync machine */
    GPTP_ERR_M_FUP_ON_MASTER_RCVD,
    /*! Follow Up message received on Grand Master domain */
    GPTP_ERR_M_SYNC_ON_GM_RCVD,
    /*! Follow Up message received on Grand Master domain */
    GPTP_ERR_M_FUP_ON_GM_RCVD,
    /*! Sync message received for unknown domain */
    GPTP_ERR_M_SYNC_DOMAIN_UNKNOWN,
    /*! Sync message received for unknown Sync machine */
    GPTP_ERR_M_SYNC_MACHINE_UNKNOWN,
    /*! Pdelay message received for unknown Pdelay machine */
    GPTP_ERR_M_PDEL_MACHINE_UNKNOWN,
    /*! Pointer to TX buffer is null */
    GPTP_ERR_M_MSG_BUFF_PTR_NULL,
    /*! No ETH type PTP in the incoming frame */
    GPTP_ERR_M_NO_ETH_TYPE_PTP,
    /*! No ETH type VLAN in the incoming frame */
    GPTP_ERR_M_NO_ETH_TYPE_VLAN,

    /* Prefix GPTP_ERR_N, NVM related error category */
    /*! Unable to read data from NVM */
    GPTP_ERR_N_UNABLE_NVM_READ,
    /*! Unable to write data to the NVM */
    GPTP_ERR_N_UNABLE_NVM_WRITE,

    /* Prefix GPTP_ERR_P, Pdelay errors category */
    /*! Lost Pdelay responses exceeded count of allowed lost responses */
    GPTP_ERR_P_TOO_MANY_LOST_RESPS,

    /* Prefix GPTP_ERR_PI, PI controller errors category */
    /*! Missing or incorrect configuration for PI Controller */
    GPTP_ERR_PI_INCORRECT_CFG,

    /* Prefix GPTP_ERR_S, Sync errors category */
    /*! Lost GM, no incoming Sync messages */
    GPTP_ERR_S_LOSS_OF_SYNC,
    /*! State machine state error */
    GPTP_ERR_S_MACHINE_STATE,
    /*! Incorrect Time stamp for the follow-up message */
    GPTP_ERR_S_INCORRECT_TS,
    /*! Sync receipt time-out exceeded */
    GPTP_ERR_S_RECEIPT_TIMEOUT,

    /* Prefix GPTP_ERR_L, Limit errors category */
    /*! Residence time is too long */
    GPTP_ERR_L_TOO_LONG_RESIDENCE,
    /*! Turnaround time is too long on the Pdelay initiator - other side responds late */
    GPTP_ERR_L_TOO_LONG_TURN_INIT,
    /*! Turnaround time is too long on the Pdelay responder - the stack responds late */
    GPTP_ERR_L_TOO_LONG_TURN_RESP,

    /* Prefix GPTP_ERR_F, function category */
    /*! Pointer to function is null */
    GPTP_ERR_F_NULL_PTR,
    /*! Function for reading time error */
    GPTP_ERR_F_TIME_READ,
    /*! Function for sending ptp frame error */
    GPTP_ERR_F_FRAME_SEND,
    /*! Function for updating ptp results error */
    GPTP_ERR_F_UPDATE_PTP,

    /* Prefix GPTP_ERR_API, API category */
    /*! Unknown domain number used in API call */
    GPTP_ERR_API_UNKNOWN_DOMAIN,
    /*! Illegal machine ID used in API call */
    GPTP_ERR_API_ILLEGAL_MACHINE_ID,
    /*! Illegal port number used in API call */
    GPTP_ERR_API_ILLEGAL_PORT_NUM,
    /*! Illegal counter used in API call */
    GPTP_ERR_API_ILLEGAL_CNTR_ID,

    /* Prefix GPTP_ERR_V, variable error */
    /*! Pointer to variable is null */
    GPTP_ERR_V_NULL_PTR,

    /*! Grand Master for the particular domain stopped working and the sync
    messages from the up-stream gPTP bridge are still comming to the Slave port */
    /* Prefix GPTP_ERR_O, Outside situation category */
    GPTP_ERR_O_DOMAIN_GM_FAILURE,

    /*! Reading of the error log with illegal index */
    /* Prefix GPTP_ERR_E, error management category */
    GPTP_ERR_E_READ_INDEX_TOO_HIGH,

    /*! Overal number of gPTP errors  */
    GPTP_ERR_NUMBER_OF_ERRS
}gptp_err_type_t;

/*!
 * @brief Error log read structure
 */
typedef struct
{   /*! Error type */
    gptp_err_type_t                 eErrorType;
    /*! gPTP port of the error occurence */
    uint8_t                         u8RespGptpPortIndex;
    /*! gPTP domain index of the error occurence */
    uint8_t                         u8RespGptpDomainIndex;
    /*! Sequence ID of message causing the error event (if applicable) */
    uint16_t                        u16RespMsgSeqId;
    /*! Time of the error occurence (local of global, depends on the error type */
    gptp_def_timestamp_t            rLogTime;
    /*! Fresh log flag. Reports if the particular error log has been read before or if it is fresh new. Fresh = true */
    bool                            bFreshLog;
}gptp_err_error_t;

/** @cond Internal */

/*******************************************************************************
 * API
 ******************************************************************************/

 /*!
 * @brief The function initializes error management.
 *
 * The function initializes error detail text for console print (if preprocessor 
 * enabled) and initializes error notification callback function pointer.
 *
 */
void GPTP_ERR_MgmtInit(const gptp_def_data_t *prGptp);

/*!
 * @brief The function clears error log.
 *
 * The function clears error log of the gPTP stack
 *
 */
void GPTP_ERR_LogClear(void);

/*!
 * @brief The function writes error log.
 *
 * The function writes error into the error log. The fresh log is written to the index 0.
 * Previous logs are shifted towards the end of the log.
 *
 * @param[in] Port information - on which port the error occurred (if applicable).
 * @param[in] Domain information - on which domain the error occured (if applicable).
 * @param[in] Error type to be logged.
 * @param[in] Sequence ID of PTP message which caused error (if applicable).
 */
void GPTP_ERR_Register(uint8_t u8Port,
                       uint8_t u8Domain,
                       gptp_err_type_t eError,
                       uint16_t u16SeqId);

/*!
 * @brief The function reads error log.
 *
 * The function reads error from the error log on the specified index.
 *
 * @param[in] Error log index which will be read.
 * @return  Error details read from the log.
 */
gptp_err_error_t GPTP_ERR_ReadIndex(uint16_t u16ErrIndex);
/** @endcond */

#endif /* GPTP_ERR */
/*******************************************************************************
 * EOF
 ******************************************************************************/
