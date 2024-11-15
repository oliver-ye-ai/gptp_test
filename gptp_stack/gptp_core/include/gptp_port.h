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

#ifndef GPTP_PORT
#define GPTP_PORT

#include "gptp_def.h"
#include "gptp_err.h"

/*******************************************************************************
 * API
 ******************************************************************************/
/*!
 *
 * @brief   This function finds gPTP port number in the port map.
 *
 * @details The function finds the gPTP port number in the port map by the
 *          Switch index and Switch port index.
 *
 * @param[out] pu8GptpPort  Pointer to gPTP port number
 * @param[in] u8PortOnSw    Number of the respective port on switch.
 * @param[in] u8Sw          Number of the respective switch.
 *
 * @return GPTP_ERR_OK in case the gPTP port is found
 */
gptp_err_type_t GPTP_PORT_PortLookup(uint8_t *pu8GptpPort,
                                     uint8_t u8PortOnSw,
                                     uint8_t u8Sw);

/*!
 *
 * @brief   This function registers multicast MAC.
 *
 * @details This function provides interface to register MAC in the Ethernet
 *          module so the multicast MAC is forwarded.
 *
 * @param[in] cpu8MacAddr Pointer to multicast MAC address.
 */
void GPTP_PORT_MulticastForward(const uint8_t *cpu8MacAddr);

/*!
 *
 * @brief   This function gets Time Base Information from the switch driver
 *
 * @details This function gets Time Base Information from the switch driver and provides
 *          it to the gPTP stack.
 *
 * @param[out] pu16GmTimeBaseIndicator Pointer to the GM Time Base Indicator variable which is updated by calling of this function.
 * @param[out] pu32LastGmPhaseChangeH Pointer to the Last GM Phase Change (High) variable which is updated by calling of this function.
 * @param[out] pu64LastGmPhaseChangeL Pointer to the Last GM Phase Change (Low) variable which is updated by calling of this function.
 * @param[out] pu32ScaledLastGmFreqChange Pointer to the Scaled Last GM Frequency Change variable which is updated by calling of this function.
 */
void GPTP_PORT_TimeBaseInfoGet(uint16_t *pu16GmTimeBaseIndicator,
                               uint32_t *pu32LastGmPhaseChangeH,
                               uint64_t *pu64LastGmPhaseChangeL,
                               uint32_t *pu32ScaledLastGmFreqChange);

/*!
 *
 * @brief   This function initializes port module
 *
 * @details This function initializes port module with providing port map table to the module.
 *
 * @param[in] cprPortTable Pointer to the Port Map Table.
 * @param[in] u8NumOfPorts Count of gPTP ports.
 */
void GPTP_PORT_PortMapInit(const gptp_def_map_table_t *cprPortTable,
                           uint8_t u8NumOfPorts);

/*!
 *
 * @brief   This function writes data to the NVM.
 *
 * @details This function writes data to the NVM using flash driver.
 *
 * @param[in] prPdelMachine Pointer to respective Pdelay machine.
 * @param[in] eNvmDataType NVM data type to access.
 * @param[in] cprGptp Pointer to global gPTP structure.
 */
void GPTP_PORT_ValueNvmWrite(gptp_def_pdelay_t *prPdelMachine,
                             gptp_def_nvm_data_t eNvmDataType,
                             const gptp_def_data_t *cprGptp);

/*!
 *
 * @brief   This function reads data from the NVM.
 *
 * @details This function reads data from the NVM using flash driver.
 *
 * @param[in] prPdelMachine Pointer to respective Pdelay machine.
 * @param[in] eNvmDataType NVM data type to access.
 * @param[in] cprGptp Pointer to global gPTP structure.
 */
void GPTP_PORT_ValueNvmRead(gptp_def_pdelay_t *prPdelMachine,
                            gptp_def_nvm_data_t eNvmDataType,
                            const gptp_def_data_t *cprGptp);

/*!
 *
 * @brief   This function sends PTP message.
 *
 * @details This function sends PTP message using the ENET driver.
 *
 * @param[in] u8Port gPTP port on which the PTP message is transmitted.
 * @param[in] u8FramePrio Eth frame priority.
 * @param[in] cprTxData Pointer to the structure containing data to send.
 * @param[in] u8FrameId Frame ID of the transmitted message.
 *
 * @return GPTP_ERR_OK in case the frame is successfully transmitted.
 */
gptp_err_type_t GPTP_PORT_MsgSend(uint8_t u8Port,
                                  uint8_t u8FramePrio,
                                  const gptp_def_tx_data_t *cprTxData,
                                  uint8_t u8FrameId);

/*!
 *
 * @brief   This function gets current Time Stamp time.
 *
 * @details This function gets current Time Stamp time from the Switch driver.
 *
 * @param[in] eTsType Type of the Time Stamp (free-running of corrected).
 *
 * @return gptp_def_timestamp_t with the current Time Stamp time.
 */
gptp_def_timestamp_t GPTP_PORT_CurrentTimeGet(gptp_def_ts_type_t eTsType);

/*!
 *
 * @brief   This function reads current link status of the gPTP port.
 *
 * @details This function reads current link status of the gPTP port using Switch driver.
 *
 * @param[in] u8Port gPTP port number.
 * @param[out] pbStat Pointer to the status variable, updated in the function.
 *
 * @return GPTP_ERR_OK in case the link status is successfully provided.
 */
gptp_err_type_t GPTP_PORT_GetLinkStatus(uint8_t u8Port,
                                        bool *pbStat);

/*!
 *
 * @brief   This function reads MAC Address of the gPTP port.
 *
 * @details This function reads MAC Address of the gPTP port using the Switch driver.
 *
 * @param[in] u8Port gPTP port number.
 * @param[out] pu64Mac Pointer to the MAC Address variable, updated in the function.
 *
 * @return GPTP_ERR_OK in case the Swt Port MAC address is successfully provided.
 */
gptp_err_type_t GPTP_PORT_ObtainPortMac(uint8_t u8Port,
                                        uint64_t *pu64Mac);

/*!
 *
 * @brief     This function provides time synchronization information.
 *
 * @details   This function provides time synchronization information received from the Grand Master to the
 *            Switch driver.
 *
 * @param[in] prGptp Pointer to global gPTP structure.
 * @param[in] cu8DomainNumber Domain number of the received Follow Up message.
 * @param[in] cpf64RateRatio Rate Ratio.
 * @param[in] crOffset Current time offset between local (corrected) clock and the Grand Master.
 * @param[in] cbNegative Positive/negative offset.
 * @param[in] ci8SyncIntervalLog Current Sync interval.
 * @param[out] pbUpdated Pointer to flag signalizing stack the clock is updated
 *
 * @return GPTP_ERR_OK in case the Local clock is successfully updated.
 */
gptp_err_type_t GPTP_PORT_UpdateLocalClock(gptp_def_data_t *prGptp,
                                           const uint8_t cu8DomainNumber,
                                           const float64_t *cpf64RateRatio,
                                           const gptp_def_timestamp_sig_t crOffset,
                                           const bool cbNegative,
                                           const int8_t ci8SyncIntervalLog,
                                           bool *pbUpdated);

/*!
 *
 * @brief      This function stabilizes the local clock.
 *
 * @details    This function stabilizes the local clock running. The stabilized clock runs at the
 *             current rate ratio. It prevents clock from drifting, in case of Grand Master loss.
 *
 * @param[in]  cpf64RateRatio Rate Ratio.
 *
 * @return GPTP_ERR_OK in case the Local clock is successfully stabilized.
 */
gptp_err_type_t GPTP_PORT_FixLocalClock(const float64_t *cpf64RateRatio);

/*!
 *
 * @brief      This function gets current time.
 *
 * @details    This function gets current Time Stamp(free running) clock time and current PTP(corrected) clock time.
 *             These Time Stamps are taken at once.
 *
 * @param[out] prFreeRunClk Pointer to the free running clock variable, updated in the function.
 * @param[out] prCorrectedClk Pointer to the corrected clock variable, updated in the function.
 *
 * @return GPTP_ERR_OK in case the time is successfully read.
 */
gptp_err_type_t GPTP_PORT_GetSwitchTimes(gptp_def_timestamp_t *prFreeRunClk,
                                         gptp_def_timestamp_t *prCorrectedClk);

/*!
 *
 * @brief   This function reads TX management object to obtain TX timestamp.
 *
 * @details This function reads TX management object which have been identified as open. If the object is
 *          owned by UPPER LAYER and contains valid egress timestamp, the timestamp is provided to the
 *          timestamp handler.
 *
 * @param[in] prFrameMap pointer to the frame map with open buffer index
 *
 */
void GPTP_PORT_ProcessMapEntry(gptp_def_tx_frame_map_t *prFrameMap);

/*!
 *
 * @brief     This function executes platform specific functionality of the Periodic function.
 *
 * @details   This function executes platform specific functionality of the Periodic function.
 *            This function may be left empty or it can perform the time stamp polling
 *            by the frame buffer and frame table index.
 *
 * @param[in] prGptp Pointer to global gPTP structure.
 *
 */
void GPTP_PORT_TimerPeriodic(gptp_def_data_t *prGptp);

/*!
 *
 * @brief     This function initializes platform part of gptp_def_pdelay_t struct.
 *
 * @details   This function executes platform specific parts of the initialization
 *            for each Path delay machine.
 *
 * @param[in] cu8Port gPTP port on which the PTP message is transmitted.
 * @param[in] cprPdelayInit Pointer to gPTP Pdelay Initialization
 * @param[in] prPdelay Pointer to gPTP INTERNAL stucture - Pdelay machine
 *
 * @return GPTP_ERR_OK in case the structure has been set.
 *
 */
gptp_err_type_t GPTP_PORT_InitPDelayStruct(const uint8_t cu8Port,
                                           const gptp_def_init_params_pdel_t *cprPdelayInit,
                                           gptp_def_pdelay_t *prPdelay);

/*!
 *
 * @brief     This function initializes platform part of gptp_def_domain_t struct.
 *
 * @details   This function executes platform specific parts of the initialization
 *            for each Domain.
 *
 * @param[in] cu8Domain Respective Domain number
 * @param[in] cprDomainInit Pointer to gPTP Domain parameters initialization
 * @param[in] prDomain Pointer to gPTP INTERNAL stucture - Per domain parameters
 *
 * @return GPTP_ERR_OK in case the structure has been set.
 *
 */
gptp_err_type_t GPTP_PORT_InitDomainStruct(const uint8_t cu8Domain,
                                           const gptp_def_init_params_domain_t *cprDomainInit,
                                           gptp_def_domain_t *prDomain);

/*!
 *
 * @brief     This function initializes platform part of gptp_def_sync_t struct.
 *
 * @details   This function executes platform specific parts of the initialization
 *            for each Sync machine.
 *
 * @param[in] cu8Machine Sync machine index for the domain
 * @param[in] cu8Domain Respective Domain number
 * @param[in] cprSyncInit Pointer to Sync initialization parameters
 * @param[in] prSync Pointer to respective Sync machine structure
 *
 * @return GPTP_ERR_OK in case the structure has been set.
 *
 */
gptp_err_type_t GPTP_PORT_InitSyncStruct(const uint8_t cu8Machine,
                                         const uint8_t cu8Domain,
                                         const gptp_def_init_params_sync_t *cprSyncInit,
                                         gptp_def_sync_t *prSync);

/*!
 *
 * @brief This function returns Ethernet II offset
 *
 * @details This function provides frame offset to the Eth II part of the frame.
 *
 * @return Ethernet II offset
 */
uint8_t GPTP_PORT_FrameRxGetOffset(void);

#endif /* GPTP_PORT */

/*******************************************************************************
 * EOF
 ******************************************************************************/
