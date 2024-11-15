/*
 *   Copyright 2023-2024 NXP
 *
 *   NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be
 *   used strictly in accordance with the applicable license terms. By expressly
 *   accepting such terms or by downloading, installing, activating and/or otherwise
 *   using the software, you are agreeing that you have read, and that you agree to
 *   comply with and are bound by, such license terms. If you do not agree to be
 *   bound by the applicable license terms, then you may not retain, install,
 *   activate or otherwise use the software.
 * 
 *   This file contains sample code only. It is not part of the production code deliverables.
 */

/**
 * @file device.c
 *
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "Mcu.h"
#include "Port.h"
#include "Eth_43_GMAC.h"
#include "OsIf.h"
#include "Gmac_Ip.h"
#include "device.h"
#include "Mcal.h"
#include "Gmac_Ip.h"
#include "Clock_Ip.h"
#include "Clock_Ip_Specific.h"
#include "Gpt.h"
#include "Platform.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#ifdef USED_PHY_TJA1100

/*! @brief Define CSR_CLOCK frequency in Hz. */
#define CSR_CLOCK                       (120000000UL)

/*! @brief Define MDIO_INSTANCE number. */
#define MDIO_INSTANCE                   (0x00U)

/*! @brief Define PHY TJA1100 registers address. */
#define PHY_BASIC_CONTROL_REGISTER_ADDRESS      (0x00U)
#define PHY_BASIC_STATUS_REGISTER_ADDRESS       (0x01U)
#define PHY_IDENTIFICATION_REGISTER_ADDRESS     (0x02U)
#define PHY_EXTENDED_STATUS_REGISTER_ADDRESS    (0x0FU)
#define PHY_EXTENDED_CONTROL_REGISTER_ADDRESS   (0x11U)
#define PHY_CONFIGURATION_REGISTER_1_ADDRESS    (0x12U)
#define PHY_GENERAL_STATUS_REGISTER             (0x18U)

/*! @brief Define PHY TJA1100 identification number bits 3..18. */
#define PHY_ID_B3_18                    (0x0180U)

/*! @brief Define PHY TJA1100 link status bit in status register. */
#define PHY_SR_LINK_STATUS_LINK_UP      (0x0004U)

/*! @brief Define PHY TJA1100 bits in control register. */
#define PHY_CR_LOOPBACK_BIT             (0x4000U)
#define PHY_CR_SPEED_SELECT_LSB_BIT     (0x2000U)
#define PHY_CR_SPEED_SELECT_MSB_BIT     (0x0040U)

/*! @brief Define PHY TJA1100 bits in extended control register. */
#define PHY_EXCR_LINK_CONTROL_BIT           (0x8000U)
#define PHY_EXCR_POWER_MODE_BITS            (0x1800U)
#define PHY_EXCR_LOOPBACK_MODE_EXTL_BITS    (0x0008U)
#define PHY_EXCR_CONFIG_EN_BIT              (0x0004U)

/*! @brief Define PHY TJA1100 bits in configuration register 1. */
#define PHY_CFGR1_LED_MODE_MASK         (0x0030U)
#define PHY_CFGR1_LED_ENABLE_BIT        (0x0008U)

/*! @brief Define PHY TJA1100 100BASE-T1 support bit in extended status
 *         register.
*/
#define PHY_ESTR_100BASE_T1             (0x0080U)

/*! @brief Define PHY TJA1100 bits in general status register. */
#define PHY_GSR_LINKFAIL_CNT_MASK       (0x00F8U)
#define PHY_GSR_LINKFAIL_CNT_SHIFT      (3U)

/*! @brief Define MDIO read/write timeout. */
#define PHY_MDIO_READ_WRITE_TIMEOUT_MS  (1000LU)

/*! @brief Define wait for PHY link up timeout. */
#define PHY_WAIT_FOR_LINK_UP_TIMEOUT    (1000000LU)

#endif /* USED_PHY_TJA1100 */

/*******************************************************************************
 * Data types
 ******************************************************************************/
/* None */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static void Check_PHY(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* None */

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief           Check PHY TJA1100 interface.
*/
static void Check_PHY(void)
{
#ifdef USED_PHY_TJA1100
    Gmac_Ip_StatusType  ePHY_Status;
    uint16              u16PHY_IdentificationRegister1;
    uint16              u16PHY_ControlRegister;
    uint16              u16PHY_ExtendedControlRegister;
    uint16              u16PHY_ConfigR1;
    uint16              u16PHY_StatusRegister;
    uint16              u16PHY_ExtendedStatusRegister;
    uint8               u8PHY_Address = 0x05u;
    uint32              u32WaitLinkUpTimeout = PHY_WAIT_FOR_LINK_UP_TIMEOUT;

    Gmac_Ip_EnableMDIO(MDIO_INSTANCE, false, CSR_CLOCK);

    ePHY_Status = Gmac_Ip_MDIORead(MDIO_INSTANCE, u8PHY_Address,
                                   PHY_IDENTIFICATION_REGISTER_ADDRESS,
                                   &u16PHY_IdentificationRegister1,
                                   PHY_MDIO_READ_WRITE_TIMEOUT_MS);
    DevAssert((Gmac_Ip_StatusType)GMAC_STATUS_SUCCESS == ePHY_Status);
    DevAssert(PHY_ID_B3_18 == u16PHY_IdentificationRegister1);

    ePHY_Status = Gmac_Ip_MDIORead(MDIO_INSTANCE, u8PHY_Address,
                                   PHY_BASIC_CONTROL_REGISTER_ADDRESS,
                                   &u16PHY_ControlRegister,
                                   PHY_MDIO_READ_WRITE_TIMEOUT_MS);
    DevAssert((Gmac_Ip_StatusType)GMAC_STATUS_SUCCESS == ePHY_Status);

    /* Disable loopback. */
    u16PHY_ControlRegister &= ~PHY_CR_LOOPBACK_BIT;

    /* Speed select 100MBit/s LSB == 1 MSB == 0. */
    u16PHY_ControlRegister |= PHY_CR_SPEED_SELECT_LSB_BIT;
    u16PHY_ControlRegister &= ~PHY_CR_SPEED_SELECT_MSB_BIT;
    ePHY_Status = Gmac_Ip_MDIOWrite(MDIO_INSTANCE, u8PHY_Address,
                                    PHY_BASIC_CONTROL_REGISTER_ADDRESS,
                                    u16PHY_ControlRegister,
                                    PHY_MDIO_READ_WRITE_TIMEOUT_MS);
    DevAssert((Gmac_Ip_StatusType)GMAC_STATUS_SUCCESS == ePHY_Status);

    /* Enable link control, set normal mode and enable access to configuration
       register. */
    u16PHY_ExtendedControlRegister = PHY_EXCR_CONFIG_EN_BIT | \
                                     PHY_EXCR_POWER_MODE_BITS | \
                                     PHY_EXCR_LINK_CONTROL_BIT;

    ePHY_Status = Gmac_Ip_MDIOWrite(MDIO_INSTANCE, u8PHY_Address,
                                    PHY_EXTENDED_CONTROL_REGISTER_ADDRESS,
                                    u16PHY_ExtendedControlRegister,
                                    PHY_MDIO_READ_WRITE_TIMEOUT_MS);
    DevAssert((Gmac_Ip_StatusType)GMAC_STATUS_SUCCESS == ePHY_Status);

    ePHY_Status = Gmac_Ip_MDIORead(MDIO_INSTANCE, u8PHY_Address,
                                   PHY_CONFIGURATION_REGISTER_1_ADDRESS,
                                   &u16PHY_ConfigR1,
                                   PHY_MDIO_READ_WRITE_TIMEOUT_MS);
    DevAssert((Gmac_Ip_StatusType)GMAC_STATUS_SUCCESS == ePHY_Status);

    /* LED mode - link up. */
    u16PHY_ConfigR1 &= ~PHY_CFGR1_LED_MODE_MASK;
    /* Enable LED. */
    u16PHY_ConfigR1 |= PHY_CFGR1_LED_ENABLE_BIT;

    ePHY_Status = Gmac_Ip_MDIOWrite(MDIO_INSTANCE, u8PHY_Address,
                                    PHY_CONFIGURATION_REGISTER_1_ADDRESS,
                                    u16PHY_ConfigR1,
                                    PHY_MDIO_READ_WRITE_TIMEOUT_MS);
    DevAssert((Gmac_Ip_StatusType)GMAC_STATUS_SUCCESS == ePHY_Status);

    ePHY_Status = Gmac_Ip_MDIORead(MDIO_INSTANCE, u8PHY_Address,
                                   PHY_EXTENDED_STATUS_REGISTER_ADDRESS,
                                   &u16PHY_ExtendedStatusRegister,
                                   PHY_MDIO_READ_WRITE_TIMEOUT_MS);
    DevAssert((Gmac_Ip_StatusType)GMAC_STATUS_SUCCESS == ePHY_Status);

    /* Supported 100BASET1. */
    DevAssert(0 != (u16PHY_ExtendedStatusRegister & PHY_ESTR_100BASE_T1));

    /* Wait for link up. */
    while (0u < u32WaitLinkUpTimeout)
    {
        ePHY_Status = Gmac_Ip_MDIORead(MDIO_INSTANCE, u8PHY_Address,
                                       PHY_BASIC_STATUS_REGISTER_ADDRESS,
                                       &u16PHY_StatusRegister,
                                       PHY_MDIO_READ_WRITE_TIMEOUT_MS);
        DevAssert((Gmac_Ip_StatusType)GMAC_STATUS_SUCCESS == ePHY_Status);
        if (0u != (u16PHY_StatusRegister & PHY_SR_LINK_STATUS_LINK_UP))
        {
            u32WaitLinkUpTimeout = 0u;
        }
    }
    DevAssert(0 != (u16PHY_StatusRegister & PHY_SR_LINK_STATUS_LINK_UP));
#endif /* USED_PHY_TJA1100 */
}

/*!
 * @brief           This function return PHY link fail status.
*/
bool get_device_link_status(void)
{
#ifdef USED_PHY_TJA1100
    uint16              u16PHY_StatusRegister;
    uint8               u8PHY_Address = 0x05u;
    Gmac_Ip_StatusType  ePHY_Status;

    ePHY_Status = Gmac_Ip_MDIORead(MDIO_INSTANCE, u8PHY_Address,
                                   PHY_BASIC_STATUS_REGISTER_ADDRESS,
                                   &u16PHY_StatusRegister,
                                   PHY_MDIO_READ_WRITE_TIMEOUT_MS);
    DevAssert((Gmac_Ip_StatusType)GMAC_STATUS_SUCCESS == ePHY_Status);

    return (0 != (u16PHY_StatusRegister & PHY_SR_LINK_STATUS_LINK_UP));
#else
    return TRUE;
#endif /* USED_PHY_TJA1100 */
}

/*!
 * @brief           This function initializes all drivers.
*/
void device_init(void)
{
    StatusType u8Err;

    OsIf_Init(NULL_PTR);

    /* Initialize all pins using the Port driver. */
    Port_Init(NULL_PTR);

    /* Initialize Clocks. */
    Mcu_Init(NULL_PTR);
    u8Err = Mcu_InitClock(McuClockSettingConfig_0);
    DevAssert((StatusType)E_OK == u8Err);
    /* Activates the PLL clock to the MCU clock distribution. */
    u8Err = Mcu_DistributePllClock();
    DevAssert((StatusType)E_OK == u8Err);

    /* Sets the MCU power mode. */
    Mcu_SetMode(McuModeSettingConf_0);

    /* Initialize the Eth module. */
    Eth_43_GMAC_Init(NULL_PTR);

    /* Initialize platform. */
    Platform_Init(NULL_PTR);

    /* Initialize the high level configuration structure of Gpt driver. */
    Gpt_Init(NULL_PTR);
    /* Enable interrupt and start PIT timer channel used for switching tasks. */
    Gpt_EnableNotification(GptConf_GptChannelConfiguration_GptChannelConfiguration_0);

    /* Enable the Eth module. */
    u8Err = Eth_43_GMAC_SetControllerMode(EthConf_EthCtrlConfig_EthCtrlConfig_0,
                                          ETH_MODE_ACTIVE);
    DevAssert((StatusType)E_OK == u8Err);

    /* Test PHY. */
    Check_PHY();
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
