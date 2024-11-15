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

/*!
 * @file gptp_frame.h
 *
 */

#ifndef GPTP_FRAME
    #define GPTP_FRAME

#include "gptp_def.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Ethernet type PTP */
#define GPTP_FR_ETH_TYPE_PTP                        0x88F7u
#define GPTP_FR_ETH_TYPE_VLAN                       0x8100u

/* Ethernet frame field ofssets */
#define GPTP_FR_ETH_TYPE_OFFSET                     12u
#define GPTP_FR_MSG_ID_OFFSET                       0u
#define GPTP_FR_DOMAIN_NUM                          4u
#define GPTP_FR_CORR_OFFSET                         8u
#define GPTP_FR_SC_CLK_ID_OFFSET                    20u
#define GPTP_FR_SC_PORT_ID_OFFSET                   28u
#define GPTP_FR_SEQ_ID_OFFSET                       30u
#define GPTP_FR_MSG_PER_LOG_OFFSET                  33u
#define GPTP_FR_TS_S_OFFSET                         34u
#define GPTP_FR_TS_NS_OFFSET                        40u
#define GPTP_FR_RQ_CLK_ID_OFFSET                    44u
#define GPTP_FR_RQ_PORT_ID_OFFSET                   52u
#define GPTP_FR_CS_RATE_RATIO_OFFSET                54u
#define GPTP_FR_SIG_ORG_SUBTYPE_OFFSET              50u
#define GPTP_FR_SIG_TIME_SYNC_OFFSET                55u
#define GPTP_FR_GM_TIME_BASE_INDIC                  58u
#define GPTP_FR_LAST_GM_PHASE_CHANGE_H              60u
#define GPTP_FR_LAST_GM_PHASE_CHANGE_L              64u
#define GPTP_FR_SCALED_LAST_GM_F_CHANGE             72u

/* Follow up TLV length field */
#define GPTP_FR_FUP_TLV_FIELD_LEN                   28u
/* Follow up TLV type */
#define GPTP_FR_FUP_TLV_TYPE                        3u
/* Follow up TLV organization sub type */
#define GPTP_FR_FUP_TLV_ORG_SUB_TYPE                1u
/* Follow up TLV organization ID */
#define GPTP_FR_FUP_TLV_ORGANIZATION_ID             0x0080C2

/* Signaling TLV length field */
#define GPTP_FR_SIG_TLV_FIELD_LEN                   12u
/* Signaling TLV length field */
#define GPTP_FR_SIG_TLV_TYPE                        3u
/* Signaling TLV organization sub type - message interval */
#define GPTP_FR_SIG_TLV_OSUB_T_MSG_INT              2u
/* Signaling TLV organization sub type - gPTP capable */
#define GPTP_FR_SIG_TLV_OSUB_T_CPBL                 4u
/* Signaling TLV organization sub type - gPTP capable interval */
#define GPTP_FR_SIG_TLV_OSUB_T_CPBL_INT             5u
/* Signaling TLV organization ID */
#define GPTP_FR_SIG_TLV_ORGANIZATION_ID             0x0080C2

/* Period stop log value */
#define GPTP_FR_LOG_MSG_PERIOD_STOP                 0x7F

/* Control field values for specific messages */
/* Sync message */
#define GPTP_FR_CONTROL_SYNC                        0x0
/* Follow up message */
#define GPTP_FR_CONTROL_FOLLOW_UP                   0x02
/* All other messages */
#define GPTP_FR_CONTROL_OTHER                       0x05

/*******************************************************************************
 * API
 ******************************************************************************/

/*!
 * @brief The function parses received PTP ethernet frame.
 *
 * The function parses received PTP ethernet frame.
 *
 * @param[in] rRxData structure containing all the necessary gPTP data for processing.
 * @param[in] prGptp pointer to gPTP global data structure
 * @param[in] prMessageTypeRcvd Message type received
 * @param[in] pu8DomainRcvd pointer to domain for received data
 * @param[in] pu8MachineRcvd pointer to machine for received data
 *
 * @return Error information
 */
gptp_err_type_t GPTP_FRAME_ParseRx(gptp_def_rx_data_t rRxData,
                                   const gptp_def_data_t *prGptp,
                                   gptp_def_msg_type_t  *prMessageTypeRcvd,
                                   uint8_t *pu8DomainRcvd,
                                   uint8_t *pu8MachineRcvd);

/*!
 * @brief The function builds PTP ethernet frame to send
 *
 * The function builds PTP ethernet frame to send
 *
 * @param[in] prGptp pointe to global data structure
 * @param[in] ptxData pointer to tx data buffer
 * @param[in] u8Domain Domain number
 * @param[in] u8Port gPTP port number
 *
 */
void GPTP_FRAME_BuildTx(const gptp_def_data_t *prGptp,
                        gptp_def_tx_data_t *ptxData,
                        const uint8_t u8Domain,
                        const uint8_t u8Port);

#endif /* GPTP_FRAME */
/*******************************************************************************
 * EOF
 ******************************************************************************/
