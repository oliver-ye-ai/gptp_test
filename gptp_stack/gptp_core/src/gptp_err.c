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

/**
 * @file gptp_err.c
 *
 */

#include "gptp_def.h"
#include "gptp_err.h"
#include "gptp_port.h"
#include "gptp_internal.h"

#ifdef GPTP_USE_PRINTF
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define GPTP_ERR_DBG_MSG_MAX_LEN         80u

#endif /*GPTP_USE_PRINTF*/

/*******************************************************************************
 * Variables
 ******************************************************************************/
static gptp_err_error_t         rErrorLog[GPTP_ERR_LOG_LENGTH];
static gptp_err_notify_t        pfErrorNotify = NULL;

#ifdef GPTP_USE_PRINTF
static char aas8GptpErrDescription[GPTP_ERR_NUMBER_OF_ERRS][GPTP_ERR_DBG_MSG_MAX_LEN];

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void GPTP_ERR_MsgCopy(char cDest[GPTP_ERR_DBG_MSG_MAX_LEN], const char cSource[GPTP_ERR_DBG_MSG_MAX_LEN]);

/*******************************************************************************
 * Local Functions
 ******************************************************************************/
/*!
 * @brief The function copies debug messages.
 *
 * The function copies debug messages into the given array
 *
 */
static void GPTP_ERR_MsgCopy(char cDest[GPTP_ERR_DBG_MSG_MAX_LEN], const char cSource[GPTP_ERR_DBG_MSG_MAX_LEN])
{
    uint8_t u8Copy = 0u;
    do
    {
        cDest[u8Copy] = cSource[u8Copy];
        u8Copy++;
    }while (('\0' != cSource[u8Copy]) && (GPTP_ERR_DBG_MSG_MAX_LEN > u8Copy));
}
#endif /*GPTP_USE_PRINTF*/

/*******************************************************************************
 * Code
 ******************************************************************************/

 /*!
 * @brief The function initializes error management.
 *
 * The function initializes error detail text for console print (if preprocessor 
 * enabled) and initializes error notification callback function pointer.
 *
 * @ requirements 529199
 * @ requirements 536298
 */
void GPTP_ERR_MgmtInit(const gptp_def_data_t *prGptp)
{
    pfErrorNotify = prGptp->rPtpStackCallBacks.pfErrNotify;
    
#ifdef GPTP_USE_PRINTF
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_OK], "No error");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_I_NOT_INITIALIZED], "gPTP stack not initialized");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_I_NO_PORT_INITIALIZED], "Count of initialized gPTP ports is zero");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_I_NO_DOM_INITIALIZED], "Count of initialized Domains is zero");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_I_TOO_MANY_SLAV_DOM], "Too many Sync slaves under one domain");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_I_TOO_MANY_SYNCS_DOM], "Too many Sync state machines under the domain (not enough gPTP ports)");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_I_NO_SLAVE_NON_GM_DOM], "No Slave machine for non Grand Master domain");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_I_SLAVE_ON_GM_DOMAIN], "Slave Sync state machine initialized for Grand Master domain");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_I_NO_PDELAY_FOR_SLAVE], "Pdelay initiator not enabled for the slave sync machine");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_I_START_TOUT_TOO_LONG], "Start-up time-out for Bridge is too long");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_I_NO_SOURCE_MAC], "Source port MAC address not available");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_I_NO_LINK_STATUS], "Unable to get link status");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_I_COUNTERS_NULL_PTR], "Pointer to gPTP counters is NULL");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_I_PORT_MAP_NULL_PTR], "Pointer to port mapping structure is NULL");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_I_PORT_STRCT_NULL_PTR], "Pointer to port structure is NULL");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_I_DOM_STRCT_NULL_PTR], "Pointer to Domain structure is NULL");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_I_PD_STRCT_NULL_PTR], "Pointer to Pdelay structure is NULL");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_I_SYNC_STRCT_NULL_PTR], "Pointer to Sync structure under the Domain is NULL");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_I_PDEL_INTERVAL_OOR], "Pdelay interval out of range");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_I_SYNC_INTERVAL_OOR], "Sync interval out of range");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_I_SYNCED_GM_ON_NON_GM], "Initialized Synchronized Grand Master on non Grand Master domain");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_I_DOM_NUM_USED_TWICE], "The same domain number used twice");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_I_DOM_TO_REF_NOT_INIT], "Reference domain for synced GM is not initialized");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_A_DIV_BY_0_ATTEMPT], "Division by zero attempt");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_C_TS_INCORRECT], "Times stamp is incorrect to perform the calculation");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_T_TS_NOT_REGISTERED], "The time stamp is not registered");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_T_TS_NOT_IDENTIFIED], "Unexpected time stamp received on the port");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_T_SYNC_DOMAIN_INDEX], "Incorrect Sync domain index (out of range)");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_T_SYNC_MACHINE_INDEX], "Incorrect Sync machine index (out of range)");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_T_PDEL_MACHINE_INDEX], "Incorrect Pdelay machine index (out of range)");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_T_FRAME_ID_ERROR], "Incorrect frame id received for time-stamp handler");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_M_PDEL_RESP_DBL_RCVD], "Double reception of the Pdelay response");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_M_MESSAGE_ID_INVALID], "Unknown message type received");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_M_IDENTIFIERS_MISMATCH], "The time stamp doesn't match with send message");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_M_TRANSPORT_SPECIFIC], "Transport specific byte is not 1");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_M_SYNC_ON_MASTER_RCVD], "Sync message received on master sync machine");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_M_FUP_ON_MASTER_RCVD], "Follow Up message received on master sync machine");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_M_SYNC_ON_GM_RCVD], "Follow Up message received on Grand Master domain");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_M_FUP_ON_GM_RCVD], "Follow Up message received on Grand Master domain");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_M_SYNC_DOMAIN_UNKNOWN], "Sync message received for unknown domain");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_M_SYNC_MACHINE_UNKNOWN], "Sync message received for unknown Sync machine");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_M_PDEL_MACHINE_UNKNOWN], "Pdelay message received for unknown Pdelay machine");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_M_MSG_BUFF_PTR_NULL], "Pointer to TX buffer is null");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_M_NO_ETH_TYPE_PTP], "No ETH type PTP in the incoming frame");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_M_NO_ETH_TYPE_VLAN], "No ETH type VLAN in the incoming frame");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_N_UNABLE_NVM_READ], "Unable to read data from NVM");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_N_UNABLE_NVM_WRITE], "Unable to write data to the NVM");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_P_TOO_MANY_LOST_RESPS], "Lost Pdelay responses exceeded count of allowed lost responses");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_PI_INCORRECT_CFG], "Missing or incorrect configuration for PI Controller");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_S_LOSS_OF_SYNC], "Lost GM, no incoming Sync messages");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_S_MACHINE_STATE], "State machine state error");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_S_INCORRECT_TS], "Incorrect Time stamp for the follow-up message");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_S_RECEIPT_TIMEOUT], "Sync receipt time-out exceeded");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_L_TOO_LONG_RESIDENCE], "Residence time is too long");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_L_TOO_LONG_TURN_INIT], "Turnaround time is too long on the Pdelay initiator - other side responds late");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_L_TOO_LONG_TURN_RESP], "Turnaround time is too long on the Pdelay responder - the stack responds late");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_F_NULL_PTR], "Pointer to function is null");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_F_TIME_READ], "Function for reading time error");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_F_FRAME_SEND], "Function for sending ptp frame error");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_F_UPDATE_PTP], "Function for updating ptp results error");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_API_UNKNOWN_DOMAIN], "Unknown domain number used in API call");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_API_ILLEGAL_MACHINE_ID], "Illegal machine ID used in API call");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_API_ILLEGAL_PORT_NUM], "Illegal port number used in API call");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_API_ILLEGAL_CNTR_ID], "Illegal counter used in API call");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_V_NULL_PTR], "Pointer to variable is null");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_O_DOMAIN_GM_FAILURE], "Grand Master failure");
    GPTP_ERR_MsgCopy(aas8GptpErrDescription[GPTP_ERR_E_READ_INDEX_TOO_HIGH], "Reading of the error log with illegal index");
#endif /*GPTP_USE_PRINTF*/
}

/*!
 * @brief The function clears error log.
 *
 * The function clears error log of the gPTP stack
 *
 * @ requirements 529199
 * @ requirements 529214
 */
void GPTP_ERR_LogClear(void)
{
    uint16_t u16Sweep;

    for (u16Sweep = 0u; u16Sweep < GPTP_ERR_LOG_LENGTH; u16Sweep++)
    {
        rErrorLog[u16Sweep].eErrorType = GPTP_ERR_OK;
        rErrorLog[u16Sweep].u8RespGptpPortIndex = (uint8_t)0xFFu;
        rErrorLog[u16Sweep].u8RespGptpDomainIndex = (uint8_t)0xFFu;
        rErrorLog[u16Sweep].rLogTime.u64TimeStampS = 0u;
        rErrorLog[u16Sweep].rLogTime.u32TimeStampNs = 0u;
        rErrorLog[u16Sweep].u16RespMsgSeqId = 0u;
        rErrorLog[u16Sweep].bFreshLog = false;
    }
}

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
 *
 * @ requirements 529199
 * @ requirements 529215
 */
void GPTP_ERR_Register(uint8_t u8Port,
                       uint8_t u8Domain,
                       gptp_err_type_t eError,
                       uint16_t u16SeqId)
{
    uint16_t    u16Sweep;

    for (u16Sweep = GPTP_ERR_LOG_LENGTH - 1u; u16Sweep >= 1u; u16Sweep--)
    {
        rErrorLog[u16Sweep] = rErrorLog[(u16Sweep - 1u)];
    }
    rErrorLog[0u].eErrorType = eError;
    rErrorLog[0u].u8RespGptpPortIndex = u8Port;
    rErrorLog[0u].u8RespGptpDomainIndex = u8Domain;
    rErrorLog[0u].rLogTime = GPTP_PORT_CurrentTimeGet(GPTP_DEF_TS_CORRECTED);
    rErrorLog[0u].u16RespMsgSeqId = u16SeqId;
    rErrorLog[0u].bFreshLog = true;

#ifdef GPTP_USE_PRINTF
    GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_ERROR, ("gptp: Error ID %d: %s", eError, aas8GptpErrDescription[(uint16_t) eError]));
    if ((u8Port != GPTP_ERR_PORT_NOT_SPECIF) || (u8Domain != GPTP_ERR_DOMAIN_NOT_SPECIF))
    {
        GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_ERROR, (" on"));
        if (u8Port != GPTP_ERR_PORT_NOT_SPECIF)
        {
            GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_ERROR, (" port index %u", u8Port));
        }
        if (u8Domain != GPTP_ERR_DOMAIN_NOT_SPECIF)
        {
            GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_ERROR, (" domain index: %u", u8Domain));
        }
    }

    if (u16SeqId != GPTP_ERR_SEQ_ID_NOT_SPECIF)
    {
        GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_ERROR, (", Sequence ID: %u", u16SeqId));
    }
    GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_ERROR, (", Time:"));
    GPTP_PRINTF_TIME_UNSIG(GPTP_DEBUG_MSGTYPE_ERROR, rErrorLog[0u].rLogTime);
#endif

    if (NULL != pfErrorNotify)
    {
        pfErrorNotify();
    }
}

/*!
 * @brief The function reads error log.
 *
 * The function reads error from the error log on the specified index.
 *
 * @param[in] Error log index which will be read.
 * @return  Error details read from the log.
 *
 * @ requirements 529199
 * @ requirements 529216
 */
gptp_err_error_t GPTP_ERR_ReadIndex(uint16_t u16ErrIndex)
{
    gptp_err_error_t eError;

    if (GPTP_ERR_LOG_LENGTH > u16ErrIndex)
    {
        eError = rErrorLog[u16ErrIndex];
        rErrorLog[u16ErrIndex].bFreshLog = false;
    }

    else
    {
        eError.eErrorType = GPTP_ERR_E_READ_INDEX_TOO_HIGH;
        eError.u8RespGptpPortIndex = (uint8_t)0xFFu;
        eError.u8RespGptpDomainIndex = (uint8_t)0xFFu;
        eError.rLogTime.u64TimeStampS = 0u;
        eError.rLogTime.u32TimeStampNs = 0u;
        eError.u16RespMsgSeqId = 0u;
        eError.bFreshLog = false;
    }

    return eError;
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
