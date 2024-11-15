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
 * @file gptp_internal.h
 *
 */
#ifndef GPTP_INTERNAL
    #define GPTP_INTERNAL

#include "gptp_def.h"
#include "gptp_port_version.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#ifdef GPTP_USE_PRINTF

#ifdef __DCC__
#define GPTP_PRINT_SHORT_S32    (" %d [ns] \n")
#define GPTP_PRINT_LONG_S32     (" %d [s] %d [ns] \n")
#define GPTP_PRINT_SHORT_U32    (" %u [ns] \n")
#define GPTP_PRINT_LONG_U32     (" %u [s] %u [ns] \n")
#else
#define GPTP_PRINT_SHORT_S32    (" %ld [ns] \n")
#define GPTP_PRINT_LONG_S32     (" %ld [s] %ld [ns] \n")
#define GPTP_PRINT_SHORT_U32    (" %lu [ns] \n")
#define GPTP_PRINT_LONG_U32     (" %lu [s] %lu [ns] \n")
#endif /* __DCC__ */

    #ifdef GPTP_PORT_PRINTF
        #ifndef GPTP_DEBUG_VERBOSITY
            #warning "GPTP debug verbosity not defined. Falling back to 0 (errors only)."
            #define GPTP_DEBUG_VERBOSITY 2  /* default is only errors */
        #endif
        #define GPTP_PRINTF(messageType, message) { \
            if (GPTP_DEBUG_VERBOSITY >= (messageType)) \
            { \
                GPTP_PORT_PRINTF(message); \
            } \
        }
        #define GPTP_PRINTF_TIME_SIG(messageType, time) { \
            if (GPTP_DEBUG_VERBOSITY >= (messageType)) \
            { \
                if (0 == (time).s64TimeStampS){ \
                    GPTP_PORT_PRINTF((GPTP_PRINT_SHORT_S32, (time).s32TimeStampNs)); \
                } \
                else{ \
                    GPTP_PORT_PRINTF((GPTP_PRINT_LONG_S32, ((int32_t)((time).s64TimeStampS)), (time).s32TimeStampNs)); \
                } \
            } \
        }
        #define GPTP_PRINTF_TIME_UNSIG(messageType, time) { \
            if (GPTP_DEBUG_VERBOSITY >= (messageType)) \
            { \
                if (0u == (time).u64TimeStampS){ \
                    GPTP_PORT_PRINTF((GPTP_PRINT_SHORT_U32, (time).u32TimeStampNs)); \
                } \
                else{ \
                    GPTP_PORT_PRINTF((GPTP_PRINT_LONG_U32, ((uint32_t)((time).u64TimeStampS)), (time).u32TimeStampNs)); \
                } \
            } \
        }
    #else
        #error "Platform specific printf function needs to be configured"
    #endif
#else /* GPTP_USE_PRINTF */
    #ifndef GPTP_PRINTF
    #define GPTP_PRINTF(messageType, message) { \
        (void)(messageType); \
    }
    #define GPTP_PRINTF_TIME_SIG(messageType, message) { \
        (void)(messageType); \
    }
    #define GPTP_PRINTF_TIME_UNSIG(messageType, message) { \
        (void)(messageType); \
    }
    #endif
#endif  /* GPTP_USE_PRINTF */

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef GPTP_COUNTERS

/*!
 * @brief Port counter increment function.
 *
 * This function increments port gPTP counter.
 *
 * @param[in] prGptp Pointer to global gPTP data structure.
 * @param[in] u8Port Respective gPTP port.
 * @param[in] rCntr  Counter to increment.
 *
 */
void GPTP_INTERNAL_IncrementPortStats(const gptp_def_data_t *prGptp,
                                      uint8_t u8Port,
                                      gptp_def_counters_t rCntr);

/*!
 * @brief Domain counter increment function.
 *
 * This function increments domain gPTP counter.
 *
 * @param[in] prGptp Pointer to global gPTP data structure.
 * @param[in] u8Port Respective gPTP domain.
 * @param[in] u8Port Respective gPTP port (machine belongs to the domain).
 * @param[in] rCntr  Counter to increment.
 *
 */
void GPTP_INTERNAL_IncrementDomainStats(const gptp_def_data_t *prGptp,
                                        uint8_t u8Domain,
                                        uint8_t u8Port,
                                        gptp_def_counters_t rCntr);

/*!
 *
 * @brief       This function gets value of the gPTP counter.
 *
 * @details     The function reads the value of respective gPTP counter.
 *
 * @param[in] prGptp Pointer to global gPTP data structure.
 * @param[in] u16Domain Domain number (uint16). Value 0xFFFF reads port counter value.
 * @param[in] rCntr gPTP Counter.
 * @param[out] pu32Value Pointer to the uint32_t value providing counter value.
 *
 */
gptp_err_type_t GPTP_INTERNAL_GetStatsValue(const gptp_def_data_t *prGptp,
                                            uint16_t u16Domain,
                                            uint8_t u8Port,
                                            gptp_def_counters_t rCntr,
                                            uint32_t *pu32Value);

/*!
 * @brief Counters clear
 *
 * This function clears all the gPTP stats (counter values)
 *
 * @param[in] prGptp Pointer to global gPTP data structure.
 *
 */
void GPTP_INTERNAL_ClearStats(const gptp_def_data_t *prGptp);

#endif /* GPTP_COUNTERS */

#ifdef GPTP_USE_PRINTF
/*!
 *
 * @brief   This function prints product details to console.
 *
 * @details This function prints manufacturer id, product version and product
 *          description to the console at startup.
 *
 * @param[in] prGptp Pointer to global gPTP data structure.
 */
void GPTP_INTERNAL_PrintDetails(const gptp_def_data_t *prGptp);
#endif /* GPTP_USE_PRINTF */

/*!
 *
 * @brief   This function copies the MAC Address.
 *
 * @details This function copies the MAC address and provides it as
 *          Unique identigier in string format for the product description structure.
 *
 * @param[in] prGptp Pointer to global gPTP data structure.
 * @param[in] u64EthMac Mac Address provided as unique identifier.
 */
void GPTP_INTERNAL_CpyMac(gptp_def_data_t *prGptp,
                          uint64_t u64EthMac);

/*!
 * @brief Pdelay check function.
 *
 * This function checks Pdelay validity.
 *
 * @param[in] u8GptpPort Respective gPTP port.
 * @param[in] f64PdelayThr Pdelay threshold.
 * @param[in] f64Pdelay Pdelay to be checked.
 *
 */
bool GPTP_INTERNAL_PdelayCheck(const uint8_t u8GptpPort,
                               const float64_t f64PdelayThr,
                               float64_t f64Pdelay);

/*!
 * @brief Rate ratio check function.
 *
 * This function checks rate ratio validity.
 *
 * @param[in] f64RateRatioMaxDev Maximum rate ratio deviation.
 * @param[in] pf64RateRatio Rate ratio to be checked.
 *
 */
bool GPTP_INTERNAL_RateRratioCheck(const float64_t f64RateRatioMaxDev,
                                   float64_t *pf64RateRatio);

/*!
 * @brief Link Up function
 *
 * This function is called upon the Link Up on the gPTP port.
 *
 * @param[in] u8Port gPTP port on which the event occured.
 * @param[in] prGptp Pointer to global gPTP data structure.
 *
 */
void GPTP_INTERNAL_LinkUp(const uint8_t u8Port,
                          const gptp_def_data_t *prGptp);

/*!
 * @brief Link Down function
 *
 * This function is called upon the Link Down on the gPTP port.
 *
 * @param[in] u8Port gPTP port on which the event occured.
 * @param[in] prGptp Pointer to global gPTP data structure.
 *
 */
void GPTP_INTERNAL_LinkDown(const uint8_t u8Port,
                            gptp_def_data_t *prGptp);

/*!
 * @brief Unsigned Time Stamp minus unsigned uint32 calculation.
 *
 * This function subtracts unsigned uint32 value from the unsigned Time Stamp.
 *
 * @param[in] rTsIn Unsigned input Time Stamp.
 * @param[in] u32In Unsigned uint32 input.
 * @return gptp_def_timestamp_t unsigned output Time Stamp.
 *
 */
gptp_def_timestamp_t GPTP_INTERNAL_TsMinU32(const gptp_def_timestamp_t rTsIn,
                                            const uint32_t u32In);

/*!
 * @brief Unsigned Time Stamp minus unsigned Time Stamp calculation.
 *
 * This function subtracts unsigned Time Stamp B from the unsigned Time Stamp A.
 *
 * @param[in] rTsA Unsigned input Time Stamp.
 * @param[in] rTsB Unsigned input Time Stamp.
 * @return gptp_def_timestamp_t unsigned output Time Stamp.
 *
 */
gptp_def_timestamp_t GPTP_INTERNAL_TsMinTs(const gptp_def_timestamp_t rTsA,
                                           const gptp_def_timestamp_t rTsB);

/*!
 * @brief Signed Time Stamp minus signed Time Stamp calculation.
 *
 * This function subtracts signed Time Stamp B from the signed Time Stamp A.
 *
 * @param[in] rTsA Signed input Time Stamp.
 * @param[in] rTsB Signed input Time Stamp.
 * @return gptp_def_timestamp_sig_t signed output Time Stamp.
 *
 */
gptp_def_timestamp_sig_t GPTP_INTERNAL_TsMinTsSig(const gptp_def_timestamp_sig_t rTsA,
                                                  const gptp_def_timestamp_sig_t rTsB);

/*!
 * @brief Conversion from the unsigned Time Stamp to uint64 value.
 *
 * This function converts unsigned Time Stamp value (content of  the structure) to the unsigned
 * uint64 value. The input range is not limited but the result may not fit into the
 * uint64, if the input value is too high.
 *
 * @param[in] rTsInput Unsigned input variable.
 * @return uint64 result value.
 *
 */
uint64_t GPTP_INTERNAL_TsToU64(gptp_def_timestamp_t rTsInput);

/*!
 * @brief Conversion from the signed Time Stamp to int64 value.
 *
 * This function converts signed Time Stamp value (content of  the structure) to the signed
 * int64 value. The input range is not limited but the result may not fit into the
 * int64, if the input value is too high.
 *
 * @param[in] rTsInput Signed input variable.
 * @return uint64 result value.
 *
 */
int64_t GPTP_INTERNAL_TsToS64(gptp_def_timestamp_sig_t rTsInput);

/*!
 * @brief Value compare function
 *
 * This function compares input parameters. If the input A is greater than the input B,
 * the function returns true. If the input B is greater or equal to the input value A,
 * the function returns false.
 *
 * @param[in] rTsA Unsigned input variable.
 * @param[in] rTsB Unsigned input variable.
 * @return boolean result of the comparation.
 *
 */
bool GPTP_INTERNAL_TsAboveTs(gptp_def_timestamp_t rTsA,
                             gptp_def_timestamp_t rTsB);

/*!
 * @brief Sync lock update function.
 *
 * This function updates synchronization lock.
 *
 * @param[in] prGptp      Pointer to global gPTP data structure.
 * @param[in] rSyncOffset Synchronization offset.
 */
void GPTP_INTERNAL_UpdateSyncLock(gptp_def_data_t *prGptp,
                                  gptp_def_timestamp_sig_t rSyncOffset);

/*!
 * @brief Sync lock reset function.
 *
 * This function resets synchronization lock.
 *
 * @param[in] prGptp  Pointer to global gPTP data structure.
 */
void GPTP_INTERNAL_ResetSyncLock(gptp_def_data_t *prGptp);

/*!
 * @brief The function converts log delay value into uint64 [ns] value
 *
 * @param[in] Log value of the interval.
 * @return    Uint64 value calculated from the log input
 */
uint64_t GPTP_INTERNAL_Log2Ns(int8_t s8Log);

/*!
 * @brief The function handles Loss Of Sync event.
 *
 * This function reports Loss Of Sync event and resets clock ratio
 *
 * @param[in] prGptp Pointer to global gPTP data structure.
 * @param[in] prDomain Pointer to respective domain structure
 * @param[in] u8Machine Respective machine.
 * @param[in] u16SequenceId Sequence id of message causing event
 */
void GPTP_INTERNAL_LosHandle(gptp_def_data_t *prGptp,
                             const gptp_def_domain_t *prDomain,
                             uint8_t u8Machine,
                             uint16_t u16SequenceId);

/*!
 * @brief The function casts float64_t to int32_t.
 *
 * This function casts float64_t to int32_t with integer overflow correction.
 *
 * @param[in] f64Number Input number.
 * @return    int32_t value calculated from the input number
 */
int32_t GPTP_INTERNAL_Float64ToInt32(float64_t f64Number);

/*!
 * @brief The function calculates absolute value of an signed 64 bit integer.
 *
 * The function calculates absolute value of an signed 64 bit integer.
 *
 * @param[in] s64Number Input number.
 * @return    uint32_t value calculated from the input number
 */
uint64_t GPTP_INTERNAL_AbsInt64ToUint64(int64_t s64Number);

#endif /* GPTP_INTERNAL */
/*******************************************************************************
 * EOF
 ******************************************************************************/
