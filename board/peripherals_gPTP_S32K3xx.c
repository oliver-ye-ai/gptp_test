/***********************************************************************************************************************
 * This file was generated by the S32 Configuration Tools. Any manual edits made to this file
 * will be overwritten if the respective S32 Configuration Tools is used to update this file.
 **********************************************************************************************************************/

/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Peripherals v14.0
processor: S32K344
package_id: S32K344_172HDQFP
mcu_data: PlatformSDK_S32K3
processor_version: 0.0.0
functionalGroups:
- name: VS_0
  UUID: 343d8c09-29ab-44df-a8a2-0e35bf16a60f
  selectedCore: M7_0
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
component:
- type: 'system'
- type_id: 'system'
- SystemModel:
  - Name: 'SystemModel'
  - EcvdGenerationMethod: 'INDIVIDUAL'
  - EcvdOutputPath: ''
  - EcvdGenerationTrigger: 'Generate Configuration'
  - SyncFunctionalGroups: 'true'
  - IgnoreComponentSuffix: 'true'
  - ComponentGenerationMethod: 'EcucPostBuildVariants'
  - DefaultFunctionalGroup: 'VS_0'
  - PostBuildSelectable:
    - Name: 'PostBuildSelectable'
    - PredefinedVariants:
      - 0:
        - Name: 'VS_0'
        - Path: '/system/SystemModel/PostBuildSelectable/VS_0'
        - PostBuildVariantCriterionValues: []
    - quick_selection: 'Default'
  - Criterions:
    - Name: 'Criterions'
    - PostBuildVariantCriterions: []
    - quick_selection: 'Default'
  - quick_selection: 'Default'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/*******************************************************************************
 * Included files 
 ******************************************************************************/
#include "peripherals_gPTP_S32K3xx.h"

/*******************************************************************************
 * gPTP_S32K3xx initialization code
 ******************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'gPTP_S32K3xx'
- type: 'gPTP_S32K3xx'
- mode: 'general'
- custom_name_enabled: 'false'
- type_id: 'gPTP_S32K3xx'
- functional_group: 'VS_0'
- config_sets:
  - gPTP_S32K3xx:
    - Name: 'gPTP_S32K3xx'
    - GptpDefInitParams:
      - u8EthFramePrio: '7'
      - bSdoIdCompatibilityMode: 'false'
      - bSignalingEnabled: 'true'
      - f64PdelAvgWeight: '0.9'
      - f64RratioAvgWeight: '0.9'
      - f64RratioMaxDev: '0.01'
      - f64PdelayNvmWriteThr: '40'
      - f64RratioNvmWriteThr: '0.0000001'
      - bVlanEnabled: 'false'
      - u16VlanTci:
        - u16VlanTci_PCP: '7'
        - u16VlanTci_DEI: 'true'
        - u16VlanTci_VID: '4095'
      - PIControllerConfig:
        - u32IntegralWindupLimit: '100000'
        - f64DampingRatio: '1.0'
        - f64NatFreqRatio: '12.0'
        - u64PIControllerMaxThreshold: '50000'
      - SyncLockConfig:
        - i64SynTrigOfst: '60'
        - i64UnsTrigOfst: '100'
        - u16SynTrigCnt: '2'
        - u16UnsTrigCnt: '4'
      - pfDomSelectionCB: 'GPTP_DomainSelectionCB'
      - pfNvmWriteCallback: 'GPTP_NVMWriteCB'
      - pfNvmReadCallback: 'GPTP_NVMReadCB'
      - pfErrNotifyCallback: 'GPTP_ErrorNotifyCB'
      - pfSynNotifyCallback: 'GPTP_SyncNotifyCB'
      - GptpDefMapTable:
        - 0:
          - u8SwitchPort: '0'
          - u8Switch: '0'
          - u64ClockId: '0xE05124FFFE000001'
          - bPdelayInitiatorEnabled: 'true'
          - bPdelayUnicastResp: 'false'
          - s8PdelIntervalLogInit: '0'
          - s8PdelIntervalLogOper: '3'
          - u64NeighborPropDelayThreshNs: '20000'
          - u16MeasurementsTillSlowDown: '50'
          - u16PdelLostReponsesAllowedCnt: '3'
      - quick_selection: 'Default'
    - GptpDefInitParamsDomain:
      - 0:
        - bDomainIsGm: 'false'
        - u8SlaveMachineIndex: '0'
        - bSyncedGm: 'false'
        - u8RefDomForSynced: '0'
        - u8StartupTimeoutS: '10'
        - u8SyncReceiptTimeoutCnt: '3'
        - u32SyncOutlierThrNs: '10000'
        - u8OutlierIgnoreCnt: '2'
        - u16VlanTci:
          - u16VlanTci_PCP: '1'
          - u16VlanTci_DEI: 'true'
          - u16VlanTci_VID: '0'
        - GptpDefInitParamsSync:
          - 0:
            - u8GptpPort: '0'
            - s8SyncIntervalLog: '-3'
    - CommonPublishedInformation:
      - Name: 'CommonPublishedInformation'
      - ArReleaseMajorVersion: '0'
      - ArReleaseMinorVersion: '8'
      - ArReleaseRevisionVersion: '0'
      - SwMajorVersion: '0'
      - SwMinorVersion: '0'
      - SwPatchVersion: '0'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/*
 *   Copyright 2024 NXP
 *
 *   NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly
 *   in accordance with the applicable license terms.  By expressly accepting
 *   such terms or by downloading, installing, activating and/or otherwise using
 *   the software, you are agreeing that you have read, and that you agree to
 *   comply with and are bound by, such license terms.  If you do not agree to
 *   be bound by the applicable license terms, then you may not retain,
 *   install, activate or otherwise use the software.
 *
 *   This file contains sample code only. It is not part of the production code deliverables.
 */

 /**
 * @file peripherals_gPTP_S32K3xx.c
 *
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "gptp.h"
#include "gptp_cbk.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define GPTP_CFG_CNT_OF_PORTS           (1u)
#define GPTP_CFG_CNT_OF_SYNC            (1u)
#define GPTP_CFG_CNT_OF_DOMAINS         (1u)
#define GPTP_CFG_CNT_OF_FRAME_MAP_SYNC  (1u)

/*******************************************************************************
 * Data types
 ******************************************************************************/
/* None */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* None */

/*******************************************************************************
 * Variables
 ******************************************************************************/

static gptp_def_port_t                  sarPort[GPTP_CFG_CNT_OF_PORTS];
static gptp_def_domain_t                sarDomain[GPTP_CFG_CNT_OF_DOMAINS];
static gptp_def_pdelay_t                sarPdelay[GPTP_CFG_CNT_OF_PORTS];
#ifdef GPTP_COUNTERS
static gptp_def_countes_port_t          sarPortCounters[GPTP_CFG_CNT_OF_PORTS];
#endif /* GPTP_COUNTERS */

/* Switch and port mapping. */
static gptp_def_map_table_t             sarPortMapTable[GPTP_CFG_CNT_OF_PORTS] =
{
    {
        .u8SwitchPort = 0u,
        .u8Switch = 0u
    },
};

static gptp_def_tx_frame_map_t          sarFrameMapSync[GPTP_CFG_CNT_OF_FRAME_MAP_SYNC];

static gptp_def_sync_t                  sarSyncDom0[GPTP_CFG_CNT_OF_SYNC];
static gptp_def_init_params_sync_t      sarInitParamsDomain0Syncs[GPTP_CFG_CNT_OF_SYNC] =
{
    /* Sync Machine index 0. */
    {
        .u8GptpPort = 0u,
        .bMachineRole = false,
        .s8SyncIntervalLog = (int8_t)-3,
        .pvTxCmdSyn = NULL,
        .pvTxCmdFup = NULL,
        .prFrameMapSync = &sarFrameMapSync[0u],
    },
};

#ifdef GPTP_COUNTERS
static gptp_def_countes_dom_t           sarDom0Cntrs[GPTP_CFG_CNT_OF_SYNC];
#endif /* GPTP_COUNTERS */

static gptp_def_init_params_domain_t    sarInitParamsDomains[GPTP_CFG_CNT_OF_DOMAINS] =
{
    /* gPTP domain index 0. */
    {
        .u8DomainNumber = 0u,
        .bDomainIsGm = false,
        .u8NumberOfSyncMachines = GPTP_CFG_CNT_OF_SYNC,
        .bSyncedGm = false,
        .u8RefDomForSynced = 0u,
        .u8StartupTimeoutS = 10u,
        .u8SyncReceiptTimeoutCnt = 3u,
        .u32SyncOutlierThrNs = (uint32_t)10000u,
        .u8OutlierIgnoreCnt = (uint8_t)2u,
        .pcrDomainSyncMachinesPtr = sarInitParamsDomain0Syncs,
        .prSync = sarSyncDom0,
        .pvTxCmdSig = NULL,
#ifdef GPTP_COUNTERS
        .prCntrsDom = sarDom0Cntrs,
#endif /* GPTP_COUNTERS */
        .u16VlanTci = (uint16_t)((1u << 13u) | (1u << 12u) | (0u))
    },
};

static gptp_def_tx_frame_map_t          sarFrameMapReq[GPTP_CFG_CNT_OF_PORTS];
static gptp_def_tx_frame_map_t          sarFrameMapResp[GPTP_CFG_CNT_OF_PORTS];

static const gptp_def_params_syn_lock_t scrInitParamsSyncLock =
{
    .i64SynTrigOfst = 60,
    .i64UnsTrigOfst = 100,
    .u16SynTrigCnt = 2u,
    .u16UnsTrigCnt = 4u

};

/* Pdelay machines configuration. */
static gptp_def_init_params_pdel_t      sarInitParamsPdel[GPTP_CFG_CNT_OF_PORTS] =
{
    /* gPTP port index 0. */
    {
        .u64ClockId = 0xE05124FFFE000001u,
        .bPdelayInitiatorEnabled = true,
        .bPdelayUnicastResp = false,
        .s8PdelIntervalLogInit = 0,
        .s8PdelIntervalLogOper = 3,
        .u64NeighborPropDelayThreshNs = 20000u,
        .u16MeasurementsTillSlowDown = 50u,
        .u16PdelLostReponsesAllowedCnt = 3u,
        .u32NvmAddressPdelay = 0u,
        .u32NvmAddressRratio = 0u,
        .pvTxCmdReq = NULL,
        .pvTxCmdResp = NULL,
        .pvTxCmdReFu= NULL,
        .prFrameMapReq = &sarFrameMapReq[0u],
        .prFrameMapResp = &sarFrameMapResp[0u],
        .s16DelayAsymmetry = 0
    },
};

/* Main configuration structure. */
gptp_def_init_params_t                  rGptpCfgParams =
{
    .u8EthFramePrio = 7,
    .pcrPortMapTable = sarPortMapTable,
    .u8GptpPortsCount = (uint8_t)GPTP_CFG_CNT_OF_PORTS,
    .pcrPdelayMachinesParam = sarInitParamsPdel,
    .pcrDomainsSyncsParam = sarInitParamsDomains,
    .rSynLockParam = scrInitParamsSyncLock,
    .u8GptpDomainsCount = (uint8_t)GPTP_CFG_CNT_OF_DOMAINS,
    .bSdoIdCompatibilityMode = false,
    .bSignalingEnabled = true,
    .f64PdelAvgWeight = (float64_t)0.9,
    .f64RratioAvgWeight= (float64_t)0.9,
    .f64RratioMaxDev = (float64_t)0.01,
    .f64PdelayNvmWriteThr = (float64_t)40,
    .f64RratioNvmWriteThr = (float64_t)0.0000001,
    .paPort = sarPort,
    .paDomain = sarDomain,
    .paPdelay = sarPdelay,
    .bVlanEnabled = false,
    .u16VlanTci = (uint16_t)((7u << 13u) | (1u << 12u) | (4095u)),
    .rPIControllerConfig.u32IntegralWindupLimit = 100000u,
    .rPIControllerConfig.f64DampingRatio = 1.0,
    .rPIControllerConfig.f64NatFreqRatio = 12.0,
    .u64PIControllerMaxThreshold = 50000u,
#ifdef GPTP_COUNTERS
    .prCntrsPort = sarPortCounters,
#endif /* GPTP_COUNTERS */
    .rPtpStackCallBacks.pfDomSelectionCB = GPTP_DomainSelectionCB,
    .rPtpStackCallBacks.pfNvmWriteCB = GPTP_NVMWriteCB,
    .rPtpStackCallBacks.pfNvmReadCB = GPTP_NVMReadCB,
    .rPtpStackCallBacks.pfErrNotify = GPTP_ErrorNotifyCB,
    .rPtpStackCallBacks.pfSynNotify = GPTP_SyncNotifyCB
};

/*******************************************************************************
 * Code
 ******************************************************************************/
/* None */

/*******************************************************************************
 * EOF
 ******************************************************************************/

