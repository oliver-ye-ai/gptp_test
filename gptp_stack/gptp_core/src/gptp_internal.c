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
 * @file gptp_internal.c
 *
 */

#include "gptp_def.h"
#include "gptp_err.h"
#include "gptp_timer.h"
#include "gptp_frame.h"
#include "gptp_port.h"
#include "gptp.h"
#include "gptp_internal.h"

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static float64_t GPTP_INTERNAL_AbsValue(float64_t f64Input);

/*******************************************************************************
 * Variables
 ******************************************************************************/
#ifdef GPTP_COUNTERS
static gptp_def_counters_t au8DomCntrLookUp[GPTP_COUNTERS_DOM_CNT] =
    {ieee8021AsPortStatRxSyncCount, ieee8021AsPortStatRxFollowUpCount, \
     ieee8021AsPortStatRxPTPPacketDiscard, ieee8021AsPortStatRxSyncReceiptTimeouts, \
     ieee8021AsPortStatTxSyncCount, ieee8021AsPortStatTxFollowUpCount};

static gptp_def_counters_t au8PortCntrLookUp[GPTP_COUNTERS_PORT_CNT] =
    {ieee8021AsPortStatRxSyncCount, ieee8021AsPortStatRxFollowUpCount, \
     ieee8021AsPortStatRxPdelayRequest, ieee8021AsPortStatRxPdelayResponse, \
     ieee8021AsPortStatRxPdelayResponseFollowUp, ieee8021AsPortStatRxPTPPacketDiscard, \
     ieee8021AsPortStatRxSyncReceiptTimeouts, ieee8021AsPortStatPdelayAllowedLostResponsesExceeded, \
     ieee8021AsPortStatTxSyncCount, ieee8021AsPortStatTxFollowUpCount, \
     ieee8021AsPortStatTxPdelayRequest, ieee8021AsPortStatTxPdelayResponse, \
     ieee8021AsPortStatTxPdelayResponseFollowUp};
#endif /* GPTP_COUNTERS */

/*******************************************************************************
 * Code
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
 * @ requirements 529199
 * @ requirements 529217
 */
void GPTP_INTERNAL_IncrementPortStats(const gptp_def_data_t *prGptp,
                                      uint8_t u8Port,
                                      gptp_def_counters_t rCntr)
{
    uint8_t     u8Cntr;
    uint8_t     u8CntrLookUp;
    bool        bCntrFound;

    bCntrFound = false;

    for (u8CntrLookUp = 0; u8CntrLookUp < GPTP_COUNTERS_PORT_CNT; u8CntrLookUp++)
    {
        if (rCntr == au8PortCntrLookUp[u8CntrLookUp])
        {
            u8Cntr = u8CntrLookUp;
            bCntrFound = true;
        }
    }

    if ((true == bCntrFound) && (u8Port < prGptp->rPerDeviceParams.u8PortsCount))
    {
        /* Increment respective Port counter */
        prGptp->prCntrsPort[u8Port].u32CountersPort[u8Cntr] = prGptp->prCntrsPort[u8Port].u32CountersPort[u8Cntr] + 1u;
    }
}

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
 * @ requirements 529199
 * @ requirements 529218
 */
void GPTP_INTERNAL_IncrementDomainStats(const gptp_def_data_t *prGptp,
                                        uint8_t u8Domain,
                                        uint8_t u8Port,
                                        gptp_def_counters_t rCntr)
{
    uint8_t     u8Cntr;
    uint8_t     u8CntrLookUp;
    bool        bCntrFound;

    bCntrFound = false;

    /* Find the counter index in the reduced domain array */
    for (u8CntrLookUp = 0u; u8CntrLookUp < GPTP_COUNTERS_DOM_CNT; u8CntrLookUp++)
    {
        if (rCntr == au8DomCntrLookUp[u8CntrLookUp])
        {
            u8Cntr = u8CntrLookUp;
            bCntrFound = true;
        }
    }

    /* If both found */
    if (true == bCntrFound)
    {
        if (u8Domain < prGptp->rPerDeviceParams.u8NumberOfDomains)
        {
            if (u8Port < prGptp->prPerDomainParams[u8Domain].u8NumberOfSyncsPerDom)
            {
                /* Increment respective counter */
                prGptp->prPerDomainParams[u8Domain].prCntrsDom[u8Port].u32CountersDom[u8Cntr] = prGptp->prPerDomainParams[u8Domain].prCntrsDom[u8Port].u32CountersDom[u8Cntr] + 1u;
            }
        }
    }
}

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
 * @ requirements 529199
 * @ requirements 529219
 */
gptp_err_type_t GPTP_INTERNAL_GetStatsValue(const gptp_def_data_t *prGptp,
                                            uint16_t u16Domain,
                                            uint8_t u8Port,
                                            gptp_def_counters_t rCntr,
                                            uint32_t *pu32Value)
{
    gptp_err_type_t             eError;
    uint8_t                     u8Cntr;
    uint8_t                     u8CntrLookUp;
    uint8_t                     u8Domain;
    uint8_t                     u8DomainLookup;
    uint8_t                     u8DomainIndex;
    bool                        bDomainFound;
    bool                        bCntrFound;

    eError = GPTP_ERR_OK;

    /* Reading of Port counters */
    if (u16Domain == 0xFFFFu)
    {
        bCntrFound = false;

        for (u8CntrLookUp = 0u; u8CntrLookUp < GPTP_COUNTERS_PORT_CNT; u8CntrLookUp++)
        {
            if (rCntr == au8PortCntrLookUp[u8CntrLookUp])
            {
                u8Cntr = u8CntrLookUp;
                bCntrFound = true;
            }
        }

        /* If valid port information */
        if (u8Port < prGptp->rPerDeviceParams.u8PortsCount)
        {
            /* If valid counter value requested */
            if (true == bCntrFound)
            {
                *pu32Value = prGptp->prCntrsPort[u8Port].u32CountersPort[u8Cntr];
            }

            else
            {
                if ((0u < ((uint32_t)rCntr)) && (GPTP_IEEE_COUNTERS_CNT >= ((uint32_t)rCntr)))
                {
                    *pu32Value = 0u;
                }

                /* Invalid counter index */
                else
                {
                    eError = GPTP_ERR_API_ILLEGAL_CNTR_ID;
                    GPTP_ERR_Register(u8Port, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_API_ILLEGAL_CNTR_ID, GPTP_ERR_SEQ_ID_NOT_SPECIF);
                }
            }
        }

        /* Incorrect port index */
        else
        {
            eError = GPTP_ERR_API_ILLEGAL_PORT_NUM;
            GPTP_ERR_Register(u8Port, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_API_ILLEGAL_PORT_NUM, GPTP_ERR_SEQ_ID_NOT_SPECIF);
        }
    }

    /* Reading of Domain counters */
    else
    {
        u8Domain = (uint8_t)u16Domain;
        bDomainFound = false;
        u8DomainIndex = 0u;

        /* Find the domain index by domain number */
        for (u8DomainLookup = 0u; u8DomainLookup < prGptp->rPerDeviceParams.u8NumberOfDomains; u8DomainLookup++)
        {
            if (u8Domain == prGptp->prPerDomainParams[u8DomainLookup].u8DomainNumber)
            {
                u8DomainIndex = u8DomainLookup;
                bDomainFound = true;
            }
        }

        /* If the domain index found by the domain number */
        if (true == bDomainFound)
        {
            if (u8DomainIndex < prGptp->rPerDeviceParams.u8NumberOfDomains)
            {
                if (u8Port < prGptp->prPerDomainParams[u8DomainIndex].u8NumberOfSyncsPerDom)
                {
                    bCntrFound = false;

                    /* Find the counter index in the reduced domain array */
                    for (u8CntrLookUp = 0u; u8CntrLookUp < GPTP_COUNTERS_DOM_CNT; u8CntrLookUp++)
                    {
                        if (rCntr == au8DomCntrLookUp[u8CntrLookUp])
                        {
                            u8Cntr = u8CntrLookUp;
                            bCntrFound = true;
                        }
                    }

                    /* If counter found */
                    if (true == bCntrFound)
                    {
                        *pu32Value = prGptp->prPerDomainParams[u8DomainIndex].prCntrsDom[u8Port].u32CountersDom[u8Cntr];
                    }

                    /* Invalid counter */
                    else
                    {
                        eError = GPTP_ERR_API_ILLEGAL_CNTR_ID;
                        GPTP_ERR_Register(u8Port, u8DomainIndex, GPTP_ERR_API_ILLEGAL_CNTR_ID, GPTP_ERR_SEQ_ID_NOT_SPECIF);
                    }
                }
                /* Sync machine invalid (for respective domain) */
                else
                {
                    eError = GPTP_ERR_API_ILLEGAL_MACHINE_ID;
                    GPTP_ERR_Register(u8Port, u8DomainIndex, GPTP_ERR_API_ILLEGAL_MACHINE_ID, GPTP_ERR_SEQ_ID_NOT_SPECIF);
                }
            }

            /* Invalid domain index */
            else
            {
                eError = GPTP_ERR_API_UNKNOWN_DOMAIN;
                GPTP_ERR_Register(u8Port, u8DomainIndex, GPTP_ERR_API_UNKNOWN_DOMAIN, GPTP_ERR_SEQ_ID_NOT_SPECIF);
            }
        }

        /* Unknown domain */
        else
        {
            eError = GPTP_ERR_API_UNKNOWN_DOMAIN;
            GPTP_ERR_Register(u8Port, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_API_UNKNOWN_DOMAIN, GPTP_ERR_SEQ_ID_NOT_SPECIF);
        }
    }

    return eError;
}

/*!
 * @brief Counters clear
 *
 * This function clears all the gPTP stats (counter values)
 *
 * @param[in] prGptp Pointer to global gPTP data structure.
 *
 * @ requirements 529199
 * @ requirements 529220
 */
void GPTP_INTERNAL_ClearStats(const gptp_def_data_t *prGptp)
{
    uint8_t u8Cntr;
    uint8_t u8Port;
    uint8_t u8Domain;
    uint8_t u8CountOfPorts;
    uint8_t u8CountOfDomains;

    u8CountOfPorts = prGptp->rPerDeviceParams.u8PortsCount;
    u8CountOfDomains = prGptp->rPerDeviceParams.u8NumberOfDomains;

    /* Clear port counters */
    for (u8Port = 0u; u8Port < u8CountOfPorts; u8Port++)
    {
        for (u8Cntr = 0u; u8Cntr < GPTP_COUNTERS_PORT_CNT; u8Cntr++)
        {
            prGptp->prCntrsPort[u8Port].u32CountersPort[u8Cntr] = 0u;
        }
    }

    /* Clear domain counters (by domain index) */
    for (u8Domain = 0u; u8Domain < u8CountOfDomains; u8Domain++)
    {
        u8CountOfPorts = prGptp->prPerDomainParams[u8Domain].u8NumberOfSyncsPerDom;
        for (u8Port = 0u; u8Port < u8CountOfPorts; u8Port++)
        {
            for (u8Cntr = 0u; u8Cntr < GPTP_COUNTERS_DOM_CNT; u8Cntr++)
            {
                prGptp->prPerDomainParams[u8Domain].prCntrsDom[u8Port].u32CountersDom[u8Cntr] = 0u;
            }
        }
    }
}

#endif /* GPTP_COUNTERS */

#ifdef GPTP_USE_PRINTF
/*!
 *
 * @brief   This function prints product details to console.
 *
 * @details This function prints manufacturer id, product version and product
 *          description to the console.
 *
 * @param[in] prGptp Pointer to global gPTP data structure.
 *
 * @ requirements 529199
 * @ requirements 529221
 */
void GPTP_INTERNAL_PrintDetails(const gptp_def_data_t *prGptp)
{
    uint8_t au8SubString[GPTP_DEF_PROD_DESC_LENGTH];
    uint8_t u8Seek;
    uint8_t u8Continue;

    GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_INFO, ("gptp: Initializing gPTP SW Stack \n"));
    /* Print manufacturer ID */
    GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_INFO, ("gptp:     Manufacturer ID: "));
    au8SubString[0] = prGptp->rProductDetails.cpu8ManId[0] & (uint8_t)0x0Fu;
    au8SubString[1] = (prGptp->rProductDetails.cpu8ManId[0] & (uint8_t)0xF0u) >> 4u;
    au8SubString[3] = prGptp->rProductDetails.cpu8ManId[1] & (uint8_t)0x0Fu;
    au8SubString[4] = (prGptp->rProductDetails.cpu8ManId[1] & (uint8_t)0xF0u) >> 4u;
    au8SubString[6] = prGptp->rProductDetails.cpu8ManId[2] & (uint8_t)0x0Fu;
    au8SubString[7] = (prGptp->rProductDetails.cpu8ManId[2] & (uint8_t)0xF0u) >> 4u;
    for (u8Seek = 0u; u8Seek < 8u; u8Seek++)
    {
        if (au8SubString[u8Seek] < 0xAu)
        {
            au8SubString[u8Seek] += 48u;
        }
        else
        {
            au8SubString[u8Seek] += 55u;
        }
    }
    au8SubString[2] = (uint8_t)' ';
    au8SubString[5] = (uint8_t)' ';
    au8SubString[8] = 0u;
    GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_INFO, ("%s\n", au8SubString));

    /*Print Product Revision */
    GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_INFO, ("gptp:     Product Revision: \n"));
    u8Seek = 0u;
    do
    {
        au8SubString[u8Seek] = prGptp->rProductDetails.cpu8ProdRev[u8Seek];
        u8Seek++;
    }while (prGptp->rProductDetails.cpu8ProdRev[u8Seek] != ((uint8_t)';'));
    au8SubString[u8Seek] = 0u;
    u8Continue = u8Seek + 1u;
    GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_INFO, ("gptp:         HW: %s\n", au8SubString));

    u8Seek = 0u;
    do
    {
        au8SubString[u8Seek] = prGptp->rProductDetails.cpu8ProdRev[u8Seek + u8Continue];
        u8Seek++;
    }while (prGptp->rProductDetails.cpu8ProdRev[u8Seek + u8Continue] != ((uint8_t)';'));
    au8SubString[u8Seek] = 0u;
    u8Continue = u8Seek + u8Continue + 1u;
    GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_INFO, ("gptp:         FW: %s\n", au8SubString));

    u8Seek = 0u;
    do
    {
        au8SubString[u8Seek] = prGptp->rProductDetails.cpu8ProdRev[u8Seek + u8Continue];
        u8Seek++;
    }while (prGptp->rProductDetails.cpu8ProdRev[u8Seek + u8Continue] != 0u);
    au8SubString[u8Seek] = 0u;
    GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_INFO, ("gptp:         SW: %s\n", au8SubString));

    /* Print Product Description */
    GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_INFO, ("gptp:     Product Description:\n"));

    u8Seek = 0u;
    do
    {
        au8SubString[u8Seek] = prGptp->rProductDetails.au8ProdDesc[u8Seek];
        u8Seek++;
    }while (prGptp->rProductDetails.au8ProdDesc[u8Seek] != ((uint8_t)';'));
    au8SubString[u8Seek] = 0u;
    u8Continue = u8Seek + 1u;
    GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_INFO, ("gptp:         Manufacturer Name: %s\n", au8SubString));

    u8Seek = 0u;
    do
    {
        au8SubString[u8Seek] = prGptp->rProductDetails.au8ProdDesc[u8Seek + u8Continue];
        u8Seek++;
    }while (prGptp->rProductDetails.au8ProdDesc[u8Seek + u8Continue] != ((uint8_t)';'));
    au8SubString[u8Seek] = 0u;
    u8Continue = u8Seek + u8Continue + 1u;
    GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_INFO, ("gptp:         Model Number: %s\n", au8SubString));

    u8Seek = 0u;
    do
    {
        au8SubString[u8Seek] = prGptp->rProductDetails.au8ProdDesc[u8Seek + u8Continue];
        u8Seek++;
    }while (prGptp->rProductDetails.au8ProdDesc[u8Seek + u8Continue] != 0u);
    au8SubString[u8Seek] = 0u;

    GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_INFO, ("gptp:         Unique Identifier: %s\n", au8SubString));
}
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
 *
 * @ requirements 529199
 * @ requirements 529222
 */
void GPTP_INTERNAL_CpyMac(gptp_def_data_t *prGptp,
                          uint64_t u64EthMac)
{
    uint8_t u8Seek;
    uint8_t u8IdenPosition;
    uint8_t au8Tmp[12];

    u8Seek = 0u;
    u8IdenPosition = 0u;
    do
    {
        if (((uint8_t)';') == prGptp->rProductDetails.au8ProdDesc[u8Seek])
        {
            u8IdenPosition = u8Seek + 1u;
        }
        u8Seek++;
    } while (u8Seek < GPTP_DEF_PROD_DESC_LENGTH);

    for (u8Seek = 0u; u8Seek < 12u; u8Seek++)
    {
        au8Tmp[u8Seek] = (uint8_t) ((u64EthMac >> (44u - (u8Seek * 4u))) & 0x000000000000000Fu);
        if (au8Tmp[u8Seek] < 0xAu)
        {
            au8Tmp[u8Seek] += 48u;
        }
        else
        {
            au8Tmp[u8Seek] += 55u;
        }
    }

    for (u8Seek = 0u; u8Seek < 6u; u8Seek++)
    {
        if (GPTP_DEF_PROD_DESC_LENGTH > (((u8Seek * 3u) + 2u) + u8IdenPosition))
        {
            prGptp->rProductDetails.au8ProdDesc[u8IdenPosition + (u8Seek * 3u) + 0u] = au8Tmp[((u8Seek * 2u) + 0u)];
            prGptp->rProductDetails.au8ProdDesc[u8IdenPosition + (u8Seek * 3u) + 1u] = au8Tmp[((u8Seek * 2u) + 1u)];
            if (u8Seek != 5u)
            {
                prGptp->rProductDetails.au8ProdDesc[u8IdenPosition + (u8Seek * 3u) + 2u] = (uint8_t)':';
            }
        }
    }
}

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
 * @ requirements 529199
 * @ requirements 529223
 */
static float64_t GPTP_INTERNAL_AbsValue(float64_t f64Input)
{
    float64_t f64Output;

    f64Output = f64Input;
    if (0.0 > f64Input)
    {
        /* If the is < 0, multiply it by -1 */
        f64Output = f64Input * (-1.0);
    }

    return f64Output;
}

/*!
 * @brief Unsigned Time Stamp minus unsigned Time Stamp calculation.
 *
 * This function subtracts unsigned Time Stamp B from the unsigned Time Stamp A.
 *
 * @param[in] rTsA Unsigned input Time Stamp.
 * @param[in] rTsB Unsigned input Time Stamp.
 * @return gptp_def_timestamp_t unsigned output Time Stamp.
 *
 * @ requirements 529199
 * @ requirements 529224
 */
gptp_def_timestamp_t GPTP_INTERNAL_TsMinTs(const gptp_def_timestamp_t rTsA,
                                           const gptp_def_timestamp_t rTsB)
{
    gptp_def_timestamp_t     rTsOut;
    gptp_err_type_t          eError;

    eError = GPTP_ERR_OK;

    /* If nanoseconds time value A is higher or equal than 1 000 000 000 nanoseconds*/
    if (rTsA.u32TimeStampNs >= GPTP_DEF_NS_IN_SECONDS)
    {
        /* Register error, because there can't be more or equal than 1 000 000 000 nanoseconds */
        GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_C_TS_INCORRECT, GPTP_ERR_SEQ_ID_NOT_SPECIF);
        eError = GPTP_ERR_C_TS_INCORRECT;
    }

    /* If nanosecondss time value A is higher or equal than 1 000 000 000 nanoseconds*/
    if (rTsB.u32TimeStampNs > GPTP_DEF_NS_IN_SECONDS)
    {
        /* Register error, because there can't be more or equal than 1 000 000 000 nanoseconds */
        GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_C_TS_INCORRECT, GPTP_ERR_SEQ_ID_NOT_SPECIF);
        eError = GPTP_ERR_C_TS_INCORRECT;
    }

    /* If seconds time value in A input is lower than in input B, register eror */
    if (rTsA.u64TimeStampS < rTsB.u64TimeStampS)
    {
        GPTP_ERR_Register(GPTP_ERR_PORT_NOT_SPECIF, GPTP_ERR_DOMAIN_NOT_SPECIF, GPTP_ERR_C_TS_INCORRECT, GPTP_ERR_SEQ_ID_NOT_SPECIF);
        eError = GPTP_ERR_C_TS_INCORRECT;
    }

    /* If no error, calculate the subtraction */
    if (GPTP_ERR_OK == eError)
    {
        /* If ns timestamp value in A input is higher or equal to input B */
        if (rTsA.u32TimeStampNs >= rTsB.u32TimeStampNs)
        {
            /* Subtract seconds and nanoseconds */
            rTsOut.u32TimeStampNs = rTsA.u32TimeStampNs - rTsB.u32TimeStampNs;
            rTsOut.u64TimeStampS = rTsA.u64TimeStampS - rTsB.u64TimeStampS;
        }

        /* If ns timestamp value in A input is lower than input B */
        else
        {
            /* subtract ns value and add 1 000 000 000ns */
            rTsOut.u32TimeStampNs = (rTsA.u32TimeStampNs - rTsB.u32TimeStampNs) + GPTP_DEF_NS_IN_SECONDS;
            /* subtract s value, minus 1 second  (1 000 000 000 ns) */
            rTsOut.u64TimeStampS = (rTsA.u64TimeStampS - rTsB.u64TimeStampS) - 1u;
        }
    }

    /* If error, output = 0 */
    else
    {
        rTsOut.u64TimeStampS = 0u;
        rTsOut.u32TimeStampNs = 0u;
    }

    return rTsOut;
}

/*!
 * @brief Signed Time Stamp minus signed Time Stamp calculation.
 *
 * This function subtracts signed Time Stamp B from the signed Time Stamp A.
 *
 * @param[in] rTsA Signed input Time Stamp.
 * @param[in] rTsB Signed input Time Stamp.
 * @return gptp_def_timestamp_sig_t signed output Time Stamp.
 *
 * @ requirements 529199
 * @ requirements 529225
 */
gptp_def_timestamp_sig_t GPTP_INTERNAL_TsMinTsSig(const gptp_def_timestamp_sig_t rTsA,
                                                  const gptp_def_timestamp_sig_t rTsB)
{
    gptp_def_timestamp_sig_t    rTsOut;

    int32_t                     s32ANs;
    int64_t                     s64AS;
    int32_t                     s32BNs;
    int64_t                     s64BS;

    int32_t                     s32ResNs;
    int64_t                     s64RrsS;

    s32ANs = rTsA.s32TimeStampNs;
    s64AS = rTsA.s64TimeStampS;
    s32BNs = rTsB.s32TimeStampNs;
    s64BS = rTsB.s64TimeStampS;

    s32ResNs = s32ANs - s32BNs;

    if (s64AS != s64BS)
    {
        s64RrsS = s64AS - s64BS;

        if ((s32ResNs < 0) && (s64RrsS >= 0 ))
        {
            s32ResNs += (int32_t)GPTP_DEF_NS_IN_SECONDS;
            s64RrsS -= 1;
        }

        else if ((s32ResNs >= 0) && (s64RrsS < 0 ))
        {
            s32ResNs -= (int32_t)GPTP_DEF_NS_IN_SECONDS;
            s64RrsS += 1;
        }

        else
        {
            /* Nothing to do */
        }
    }

    else
    {
        s64RrsS = 0;
    }

    rTsOut.s32TimeStampNs = s32ResNs;
    rTsOut.s64TimeStampS = s64RrsS;

    return rTsOut;
}

/*!
 * @brief Unsigned Time Stamp minus unsigned uint32 calculation.
 *
 * This function subtracts unsigned uint32 value from the unsigned Time Stamp.
 *
 * @param[in] rTsIn Unsigned input Time Stamp.
 * @param[in] u32In Unsigned uint32 input.
 * @return gptp_def_timestamp_t unsigned output Time Stamp.
 *
 * @ requirements 529199
 * @ requirements 529226
 */
gptp_def_timestamp_t GPTP_INTERNAL_TsMinU32(const gptp_def_timestamp_t rTsIn,
                                            const uint32_t u32In)
{
    gptp_def_timestamp_t    rTemp;

    /* Calculate timestamp value from the uint32 input */
    rTemp.u32TimeStampNs = u32In % GPTP_DEF_NS_IN_SECONDS;
    rTemp.u64TimeStampS = (uint64_t)(u32In) / (uint64_t)GPTP_DEF_NS_IN_SECONDS;

    /* use the TsMinTs function to calculate the subtraction */
    return GPTP_INTERNAL_TsMinTs(rTsIn,rTemp);
}

/*!
 * @brief Conversion from the unsigned Time Stamp to uint64 value.
 *
 * This function converts unsigned Time Stamp value (content of  the structure) to the unsigned
 * uint64 value. The input range is not limited but the result may not fit into the
 * uint64, if the input value is too high.
 *
 * @note  part of the time information will be lost if the timestamp
 *        second value is higher than 4294967296 seconds (136 years).
 *
 * @param[in] rTsInput Unsigned input variable.
 * @return uint64 result value.
 *
 * @ requirements 529199
 * @ requirements 529227
 */
uint64_t GPTP_INTERNAL_TsToU64(gptp_def_timestamp_t rTsInput)
{
    uint64_t u64Output;

    /* output si nanoseconds value plus seconds calculated to nanoseconds */
    u64Output = rTsInput.u32TimeStampNs + (rTsInput.u64TimeStampS * GPTP_DEF_NS_IN_SECONDS);

    return u64Output;
}

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
 * @ requirements 529199
 * @ requirements 529228
 */
int64_t GPTP_INTERNAL_TsToS64(gptp_def_timestamp_sig_t rTsInput)
{
    int64_t s64Output;

    /* output si nanoseconds value plus seconds calculated to nanoseconds */
    s64Output = rTsInput.s32TimeStampNs + (rTsInput.s64TimeStampS * (int64_t)GPTP_DEF_NS_IN_SECONDS);

    return s64Output;
}

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
 * @ requirements 529199
 * @ requirements 529229
 */
bool GPTP_INTERNAL_TsAboveTs(gptp_def_timestamp_t rTsA,
                             gptp_def_timestamp_t rTsB)
{
    bool bTorF;

    /* If the second timestamp A value is higher than B */
    if (rTsA.u64TimeStampS >= rTsB.u64TimeStampS)
    {
        /* If the second timestamp values are equal */
        if (rTsA.u64TimeStampS == rTsB.u64TimeStampS)
        {
            /* If nsnosecond timestamp A value is higher than B */
            if (rTsA.u32TimeStampNs > rTsB.u32TimeStampNs)
            {
                /* The timestamp A is higher */
                bTorF = true;
            }

            else
            {
                /* The timestamp A is not higher */
                bTorF = false;
            }
        }

        else
        {
            /* The timestamp A is higher */
            bTorF = true;
        }
    }

    else
    {
        /* The timestamp A is not higher */
        bTorF = false;
    }

    return bTorF;
}

/*!
 * @brief Sync lock update function.
 *
 * This function updates synchronization lock.
 *
 * @param[in] prGptp      Pointer to global gPTP data structure.
 * @param[in] rSyncOffset Synchronization offset.
 */
void GPTP_INTERNAL_UpdateSyncLock(gptp_def_data_t *prGptp,
                                  gptp_def_timestamp_sig_t rSyncOffset)
{
    int64_t i64ClkOfst = GPTP_INTERNAL_TsToS64(rSyncOffset);

    /* Synced. */
    if (prGptp->rPerDeviceParams.bSynced)
    {
        prGptp->rPerDeviceParams.u16SynCnt = 0u;

        /* Synchronization offset is inside unsynchronization limits. */
        if ((-(prGptp->rPerDeviceParams.rSynLockParam.i64UnsTrigOfst) <= i64ClkOfst) &&
            (i64ClkOfst <= prGptp->rPerDeviceParams.rSynLockParam.i64UnsTrigOfst))
        {
            prGptp->rPerDeviceParams.u16UnsCnt = 0u;
        }
        /* Synchronization offset is outside unsynchronization limits. */
        else
        {
            prGptp->rPerDeviceParams.u16UnsCnt++;

            /* Consecutive unsynchronizations limit reached. */
            if (prGptp->rPerDeviceParams.u16UnsCnt >= prGptp->rPerDeviceParams.rSynLockParam.u16UnsTrigCnt)
            {
                prGptp->rPerDeviceParams.bSynced = false;
                /* Notify application. */
                if (NULL != prGptp->rPtpStackCallBacks.pfSynNotify)
                {
                    prGptp->rPtpStackCallBacks.pfSynNotify(GPTP_SYN_UNLOCK);
                }
            }
        }

    }
    /* Unsynced. */
    else
    {
        prGptp->rPerDeviceParams.u16UnsCnt = 0u;

        /* Synchronization offset is inside synchronization limits. */
        if ((-(prGptp->rPerDeviceParams.rSynLockParam.i64SynTrigOfst) <= i64ClkOfst) &&
            (i64ClkOfst <= prGptp->rPerDeviceParams.rSynLockParam.i64SynTrigOfst))
        {
            prGptp->rPerDeviceParams.u16SynCnt++;

            /* Consecutive synchronizations limit reached. */
            if (prGptp->rPerDeviceParams.u16SynCnt >= prGptp->rPerDeviceParams.rSynLockParam.u16SynTrigCnt)
            {
                prGptp->rPerDeviceParams.bSynced = true;
                /* Notify application. */
                if (NULL != prGptp->rPtpStackCallBacks.pfSynNotify)
                {
                    prGptp->rPtpStackCallBacks.pfSynNotify(GPTP_SYN_LOCK);
                }
            }
        }
        /* Synchronization offset is outside synchronization limits. */
        else
        {
            prGptp->rPerDeviceParams.u16SynCnt = 0u;
        }
    }

}

/*!
 * @brief Sync lock reset function.
 *
 * This function resets synchronization lock.
 *
 * @param[in] prGptp  Pointer to global gPTP data structure.
 */
void GPTP_INTERNAL_ResetSyncLock(gptp_def_data_t *prGptp)
{
    prGptp->rPerDeviceParams.bSynced = false;
    prGptp->rPerDeviceParams.u16SynCnt = 0u;
    prGptp->rPerDeviceParams.u16UnsCnt = 0u;

    /* Notify application. */
    if (NULL != prGptp->rPtpStackCallBacks.pfSynNotify)
    {
        prGptp->rPtpStackCallBacks.pfSynNotify(GPTP_SYN_UNLOCK);
    }
}

/*!
 * @brief Pdelay check function.
 *
 * This function checks Pdelay validity.
 *
 * @param[in] u8GptpPort Respective gPTP port.
 * @param[in] f64PdelayThr Pdelay threshold.
 * @param[in] f64Pdelay Pdelay to be checked.
 *
 * @ requirements 529199
 * @ requirements 529230
 */
bool GPTP_INTERNAL_PdelayCheck(const uint8_t u8GptpPort,
                               const float64_t f64PdelayThr,
                               float64_t f64Pdelay)
{
    bool        bValid;
    float64_t   f64PdelayTemp;

#ifndef GPTP_USE_PRINTF
    (void) u8GptpPort;              /* If the printf not enabled, the u8GptpPort is unused parameter */
#endif /* GPTP_USE_PRINTF */

    f64PdelayTemp = f64Pdelay;

    if (f64PdelayTemp < 0.0)
    {
        f64PdelayTemp = f64PdelayTemp * (-1.0);
    }

    if (f64PdelayTemp < f64PdelayThr)
    {
        bValid = true;
    }

    else
    {
        bValid = false;
        if (f64PdelayTemp == f64PdelayTemp)
        {
            GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_INFO, ("gptp: Pdelay threshold on port %u exceeded with value: %f [ns]. \n", u8GptpPort, f64Pdelay));
        }
        else
        {
            GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_INFO, ("gptp: Pdelay data for port %u not available in the NVM\n", u8GptpPort));
        }
    }

    return bValid;
}

/*!
 * @brief Rate ratio check function.
 *
 * This function checks rate ratio validity.
 *
 * @param[in] f64RateRatioMaxDev Maximum rate ratio deviation.
 * @param[in] pf64RateRatio Rate ratio to be checked.
 *
 * @ requirements 529199
 * @ requirements 529233
 */
bool GPTP_INTERNAL_RateRratioCheck(const float64_t f64RateRatioMaxDev,
                                   float64_t *pf64RateRatio)
{
    float64_t   f64TempRatioDev;
    bool        bValid;

    /* Calculate ratio deviation */
    f64TempRatioDev = (1.0 - (*pf64RateRatio));
    f64TempRatioDev = GPTP_INTERNAL_AbsValue(f64TempRatioDev);

    /* Check whether the ratio deviation is below the defined limit */

    if (f64TempRatioDev < f64RateRatioMaxDev)
    {
        bValid = true;
    }

    else
    {
        /* If the ratio is outside of the limit, set the ratio to 1.0 */
        (*pf64RateRatio) = 1.0;
        bValid = false;
    }

    return bValid;
}

/*!
 * @brief Link Up function
 *
 * This function is called upon the Link Up on the gPTP port.
 *
 * @param[in] u8Port gPTP port on which the event occured.
 * @param[in] prGptp Pointer to global gPTP data structure.
 *
 * @ requirements 529199
 * @ requirements 529234
 * @ requirements 152450
 */
void GPTP_INTERNAL_LinkUp(const uint8_t u8Port,
                          const gptp_def_data_t *prGptp)
{
    const gptp_def_domain_t        *prDomain;
    uint8_t                        u8Domain;
    uint8_t                        u8Machine;
    gptp_def_sync_t                *prSyncMachine;
    gptp_def_pdelay_t              *prPdelayMachine;

    /* Port */
    if (u8Port < prGptp->rPerDeviceParams.u8PortsCount)
    {
        prGptp->prPerPortParams[u8Port].bPortEnabled = true;
    }

        /* Pdelay */
    if (u8Port < prGptp->rPerDeviceParams.u8NumberOfPdelayMachines)
    {
        prPdelayMachine = &prGptp->prPdelayMachines[u8Port];
        prPdelayMachine->bTimerPdelayTimerEnabled = true;

        prPdelayMachine->bPdelayInitiatorEnabled = prPdelayMachine->bPdelayInitiatorEnabledPrev;
        prPdelayMachine->bPdelayResponderEnabled = true;
        prPdelayMachine->s8PdelayIntervalLog = prPdelayMachine->s8InitPdelayIntervalLog;
        /* Update next Pdelay due */
        GPTP_TIMER_PdelayDueUpdate(prPdelayMachine);

        /* Load pdelay value from the NVM */
        GPTP_PORT_ValueNvmRead(prPdelayMachine, GPTP_DEF_NVM_PDELAY, prGptp);
        prPdelayMachine->bPdelayValueValid = GPTP_INTERNAL_PdelayCheck(prPdelayMachine->u8GptpPort, \
                                                                       (float64_t)prPdelayMachine->u64NeighborPropDelayThreshNs, \
                                                                       prPdelayMachine->f64NeighborPropDelay);
        
        if (false == prPdelayMachine->bPdelayValueValid)
        {
            /* Value from NVM is above the threshold or it's NAN */
            prPdelayMachine->f64NeighborPropDelay = 0.0;
        }

        /* Load neighbor rate ratio value from the NVM */
        GPTP_PORT_ValueNvmRead(prPdelayMachine, GPTP_DEF_NVM_RRATIO, prGptp);
        prPdelayMachine->bNeighborRateRatioValid = GPTP_INTERNAL_RateRratioCheck(prGptp->rPerDeviceParams.f64RratioMaxDev, \
                                                                                 &prPdelayMachine->f64NeighborRateRatio);

        prPdelayMachine->bPdelAveragerInitialized = false;
        prPdelayMachine->bRatioAveragerInitialized = false;

        prPdelayMachine->u16PdelayResponsesCnt = 0u;
    }

    /* Sync */
    for (u8Domain = 0u; u8Domain < prGptp->rPerDeviceParams.u8NumberOfDomains; u8Domain++)
    {
        prDomain = &prGptp->prPerDomainParams[u8Domain];

        for (u8Machine = 0u; u8Machine < prDomain->u8NumberOfSyncsPerDom; u8Machine++)
        {
            prSyncMachine = &prDomain->prSyncMachines[u8Machine];

            if (u8Port == prSyncMachine->u8GptpPort)
            {
                prSyncMachine->s8SyncIntervalLog = prSyncMachine->s8InitSyncIntervalLog;
                prSyncMachine->s8OperSyncIntervalLog = prSyncMachine->s8InitSyncIntervalLog;
                prSyncMachine->bTimerSyncEnabled = true;
            }
        }
    }

    GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_STATE, ("gptp: Link up event on gPTP port %u. \n", u8Port));
}

/*!
 * @brief Link Down function
 *
 * This function is called upon the Link Down on the gPTP port.
 *
 * @param[in] u8Port gPTP port on which the event occured.
 * @param[in] prGptp Pointer to global gPTP data structure.
 *
 * @ requirements 529199
 * @ requirements 529235
 * @ requirements 152450
 */
void GPTP_INTERNAL_LinkDown(const uint8_t u8Port,
                            gptp_def_data_t *prGptp)
{
    gptp_def_domain_t              *prDomain;
    uint8_t                        u8Domain;
    uint8_t                        u8Machine;
    gptp_def_sync_t                *prSyncMachine;
    gptp_def_pdelay_t              *prPdelayMachine;

    /* Port */
    if (u8Port < prGptp->rPerDeviceParams.u8PortsCount)
    {
        prGptp->prPerPortParams[u8Port].bPortEnabled = false;
    }

    /* Pdelay */
    if (u8Port < prGptp->rPerDeviceParams.u8NumberOfPdelayMachines)
    {
        prPdelayMachine = &prGptp->prPdelayMachines[u8Port];

        prPdelayMachine->bTimerPdelayTimerEnabled = false;
        prPdelayMachine->bPdelayInitiatorEnabledPrev = prPdelayMachine->bPdelayInitiatorEnabled;
        prPdelayMachine->bPdelayInitiatorEnabled = false;
        prPdelayMachine->bPdelayResponderEnabled = false;

        prPdelayMachine->ePdelayInitiatorState = GPTP_DEF_INITIATOR_NOT_ENABLED;
        prPdelayMachine->ePdelayResponderState = GPTP_DEF_RESPONDER_NOT_ENABLED;
    }

    /* Sync */
    for (u8Domain = 0u; u8Domain < prGptp->rPerDeviceParams.u8NumberOfDomains; u8Domain++)
    {
        prDomain = &prGptp->prPerDomainParams[u8Domain];

        for (u8Machine = 0u; u8Machine < prDomain->u8NumberOfSyncsPerDom; u8Machine++)
        {
            prSyncMachine = &prDomain->prSyncMachines[u8Machine];

            if (u8Port == prSyncMachine->u8GptpPort)
            {
                prSyncMachine->eSyncMasterState = GPTP_DEF_SYNC_MASTER_NOT_EN;
                prSyncMachine->eSyncSlaveState = GPTP_DEF_SYNC_SLAVE_NOT_EN;
                prSyncMachine->bTimerSyncEnabled = false;

                /* Only for slave machine that belongs to the port */
                if (false == prSyncMachine->bSyncRole)
                {
                    /* Force to call GPTP_INTERNAL_LosHandle function within the following GPTP_TIMER_TimerPeriodic function */
                    prDomain->u64TimerSyncReceiptTimeoutDue = 1u;
                }
            }
        }
    }

    /* Reset sync lock and report to the app. */
    GPTP_INTERNAL_ResetSyncLock(prGptp);

    GPTP_PRINTF(GPTP_DEBUG_MSGTYPE_STATE, ("gptp: Link down event on gPTP port %u. \n", u8Port));
}

/*!
 * @brief The function converts log delay value into uint64 [ns] value
 *
 * @param[in] Log value of the interval.
 * @return    Uint64 value calculated from the log input
 *
 * @ requirements 529199
 * @ requirements 529236
 */
uint64_t GPTP_INTERNAL_Log2Ns(int8_t s8Log)
{
    uint64_t u64Output;
    int8_t s8LogTemp;

    /* Load sLog(input) into the temporary variable */
    s8LogTemp = s8Log;

    /* If the Log value is below 0 */
    if (s8LogTemp < 0)
    {
        /* Multiply by -1 */
        s8LogTemp *= -1;
        /* Shift the 1000000000ns with (-1 * Log) value */
        u64Output = ((uint64_t)GPTP_DEF_NS_IN_SECONDS) >> (uint8_t)s8LogTemp;
    }

    /* If the value is equal or above zero */
    else
    {
        /* Shift the 1000000000ns with the Log value */
        u64Output = ((uint64_t)GPTP_DEF_NS_IN_SECONDS) << (uint8_t)s8LogTemp;
    }

    /* If the Log value is 127 */
    if (s8LogTemp == 127)
    {
        /* The output ns value is zero - no delay, stop sending */
        u64Output = 0u;
    }

    return u64Output;
}

/*!
 * @brief The function handles Loss Of Sync event.
 *
 * This function reports Loss Of Sync event and resets clock ratio
 *
 * @param[in] prGptp Pointer to global gPTP data structure.
 * @param[in] prDomain Pointer to respective domain structure
 * @param[in] u8Machine Respective machine.
 * @param[in] u16SequenceId Sequence id of message causing event
 *
 * @ requirements 529199
 * @ requirements 529237
 * @ requirements 120458
 */
void GPTP_INTERNAL_LosHandle(gptp_def_data_t *prGptp,
                             const gptp_def_domain_t *prDomain,
                             uint8_t u8Machine,
                             uint16_t u16SequenceId)
{
    gptp_err_type_t eErr;

    /* Update local clock in case it has already been updated (last setpoint exists) */
    if (true == prGptp->rPerDeviceParams.bEverUpdated) {
        /* Update local clock - to prevent from drifting */
        eErr = GPTP_PORT_FixLocalClock(&prDomain->prSyncMachines[prDomain->u8SlaveMachineId].f64RateRatio);
        if (GPTP_ERR_OK != eErr)
        {
            GPTP_ERR_Register(u8Machine, prDomain->u8DomainIndex, eErr, u16SequenceId);
        }
    }

    /* Reset sync lock and report to the app. */
    GPTP_INTERNAL_ResetSyncLock(prGptp);

    /* Register Loss of sync error */
    GPTP_ERR_Register(u8Machine, prDomain->u8DomainIndex, GPTP_ERR_S_LOSS_OF_SYNC, u16SequenceId);
}

/*!
 * @brief The function casts float64_t to int32_t.
 *
 * This function casts float64_t to int32_t with integer overflow correction.
 *
 * @param[in] f64Number Input number.
 * @return    int32_t value calculated from the input number
 *
 * @ requirements 529199
 * @ requirements 536299
 */
int32_t GPTP_INTERNAL_Float64ToInt32(float64_t f64Number)
{
    int32_t s32Result;

    if (f64Number <= (float64_t)GPTP_DEF_INT32_MIN)
    {
        s32Result = GPTP_DEF_INT32_MIN;
    }
    else if (f64Number >= (float64_t)GPTP_DEF_INT32_MAX)
    {
        s32Result = GPTP_DEF_INT32_MAX;
    }
    else
    {
        s32Result = (int32_t)f64Number;
    }

    return s32Result;
}

/*!
 * @brief The function calculates absolute value.
 *
 * The function calculates absolute value.
 *
 * @param[in] s64Number Input number.
 * @return    uint32_t value calculated from the input number
 *
 * @ requirements 529199
 * @ requirements 536300
 */
uint64_t GPTP_INTERNAL_AbsInt64ToUint64(int64_t s64Number)
{
    return (s64Number < 0LL) ? ((uint64_t)(-s64Number)) : ((uint64_t)s64Number);
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
