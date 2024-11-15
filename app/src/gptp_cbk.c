/*
 *  Copyright 2023-2024 NXP
 *
 *  NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only
 *  be used strictly in accordance with the applicable license terms. By
 *  expressly accepting such terms or by downloading, installing, activating
 *  and/or otherwise using the software, you are agreeing that you have read,
 *  and that you agree to comply with and are bound by, such license terms. If
 *  you do not agree to be bound by the applicable license terms, then you may
 *  not retain, install, activate or otherwise use the software.
 *
 *  This file contains sample code only. It is not part of the production code deliverables.
 */

/**
 * @file gptp_cbk.c
 *
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "gptp_cbk.h"
#include "gptp_err.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* None */

/*******************************************************************************
 * Data types
 ******************************************************************************/
/* None */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

extern uint8_t EEPROM_Write_CB(uint8_t u8PdelayMachine,
                               gptp_def_nvm_data_t eNvmDataType,
                               float64_t f64Value,
                               gptp_def_mem_write_stat *peWriteStat);
extern uint8_t EEPROM_Read_CB(uint8_t u8PdelayMachine,
                              gptp_def_nvm_data_t eNvmDataType,
                              float64_t *f64Value);

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief           This function provides domain selection.
 *
 * @details         This function provides domain selection as which domain
 *                  clock can be corrected.
 *
 * @param[in]       u8DomainNumber Domain number of the received Follow Up
 *                  message.
 * @param[in]       pf64RateRatio Pointer to Rate Ratio.
 * @param[in]       prTime Pointer to current time offset between local
 *                  (corrected) clock and the Grand Master.
 * @param[in]       pbNegativeOfst Pointer to positive/negative flag offset.
 * @param[in]       pi8SyncIntervalLog Pointer to current Sync interval.
 *
 * @return          Returns true if the clock of current domain can be
 *                  corrected, otherwise returns false.
*/
bool GPTP_DomainSelectionCB(uint8_t u8DomainNumber, 
                            float64_t *pf64RateRatio, 
                            gptp_def_timestamp_sig_t *prTime, 
                            bool *pbNegativeOfst, 
                            int8_t *pi8SyncIntervalLog)
{
    (void)u8DomainNumber;
    (void)*pf64RateRatio;
    (void)*prTime;
    (void)*pbNegativeOfst;
    (void)*pi8SyncIntervalLog;

    bool bUpdateClock;

    bUpdateClock = true;

    /* Place for custom implementation of domain selection. */

    return bUpdateClock;
}

/*!
 * @brief           This functions writes data to the NVM.
 *
 * @details         This function is an example of callbeck wich si called from
 *                  gPTP stack directly.
 *
 * @param[in]       u8PdelayMachine PDelay machine index.
 * @param[in]       eNvmDataType NVM data type to access.
 * @param[in]       f64Value Number to write.
 * @param[out]      peWriteStat Write state.
 *
 * @return          Error status.
*/
uint8_t GPTP_NVMWriteCB(uint8_t u8PdelayMachine, 
                        gptp_def_nvm_data_t eNvmDataType, 
                        float64_t f64Value, 
                        gptp_def_mem_write_stat *peWriteStat)
{
    return EEPROM_Write_CB(u8PdelayMachine, eNvmDataType, f64Value,
                           peWriteStat);
}

/*!
 * @brief           This functions reads data from the NVM.
 *
 * @details         This function is an stub function of callbeck wich si called
 *                  from gPTP stack directly.
 *
 * @param[in]       u8PdelayMachine PDelay machine index.
 * @param[in]       eNvmDataType NVM data type to access.
 * @param[out]      pf64Value Pointer to number.
 *
 * @return          Error status.
*/
uint8_t GPTP_NVMReadCB(uint8_t u8PdelayMachine, 
                       gptp_def_nvm_data_t eNvmDataType, 
                       float64_t *pf64Value)
{
    return EEPROM_Read_CB(u8PdelayMachine, eNvmDataType, pf64Value);
}

/*!
 * @brief           This functions occurs when new error in gPTP stack
 *                  is registered.
 *
 * @details         This function is an example of callback wich is called from
 *                  gPTP stack directly.
*/
void GPTP_ErrorNotifyCB(void)
{
    /* Place for custom implementation of reaction on error notification. */
}

/*!
 * @brief           This functions occurs when synchronization state change
 *                  is registered in gPTP stack.
 *
 * @details         This function is an example of callback wich is called from
 *                  gPTP stack directly.
 *
 * @param[in]       eSyncLockState Synchronization lock state.
*/
void GPTP_SyncNotifyCB(gptp_def_syn_lock_state_type_t eSyncLockState)
{
    /* Place for custom implementation of reaction on error notification. */
    (void)eSyncLockState;
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
