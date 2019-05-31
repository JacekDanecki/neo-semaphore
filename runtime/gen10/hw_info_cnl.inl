/*
 * Copyright (C) 2017-2019 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "runtime/aub_mem_dump/aub_services.h"
#include "runtime/memory_manager/memory_constants.h"

#include "engine_node.h"
#include "hw_cmds.h"
#include "hw_info.h"

namespace NEO {

const char *HwMapper<IGFX_CANNONLAKE>::abbreviation = "cnl";

bool isSimulationCNL(unsigned short deviceId) {
    switch (deviceId) {
    case ICNL_3x8_DESK_DEVICE_F0_ID:
    case ICNL_5x8_DESK_DEVICE_F0_ID:
    case ICNL_9x8_DESK_DEVICE_F0_ID:
    case ICNL_13x8_DESK_DEVICE_F0_ID:
        return true;
    }
    return false;
};

const PLATFORM CNL::platform = {
    IGFX_CANNONLAKE,
    PCH_UNKNOWN,
    IGFX_GEN10_CORE,
    IGFX_GEN10_CORE,
    PLATFORM_NONE, // default init
    0,             // usDeviceID
    0,             // usRevId. 0 sets the stepping to A0
    0,             // usDeviceID_PCH
    0,             // usRevId_PCH
    GTTYPE_UNDEFINED};

const RuntimeCapabilityTable CNL::capabilityTable{
    {0, 0, 0, false, false, false},                // kmdNotifyProperties
    {true, true},                                  // whitelistedRegisters
    MemoryConstants::max48BitAddress,              // gpuAddressSpace
    83.333,                                        // defaultProfilingTimerResolution
    MemoryConstants::pageSize,                     // requiredPreemptionSurfaceSize
    &isSimulationCNL,                              // isSimulation
    PreemptionMode::MidThread,                     // defaultPreemptionMode
    aub_stream::ENGINE_RCS,                        // defaultEngineType
    0,                                             // maxRenderFrequency
    21,                                            // clVersionSupport
    CmdServicesMemTraceVersion::DeviceValues::Cnl, // aubDeviceId
    0,                                             // extraQuantityThreadsPerEU
    64,                                            // slmSize
    false,                                         // blitterOperationsSupported
    true,                                          // ftrSupportsFP64
    true,                                          // ftrSupports64BitMath
    true,                                          // ftrSvm
    true,                                          // ftrSupportsCoherency
    true,                                          // ftrSupportsVmeAvcTextureSampler
    true,                                          // ftrSupportsVmeAvcPreemption
    false,                                         // ftrRenderCompressedBuffers
    false,                                         // ftrRenderCompressedImages
    true,                                          // ftr64KBpages
    true,                                          // instrumentationEnabled
    true,                                          // forceStatelessCompilationFor32Bit
    true,                                          // isCore
    true,                                          // sourceLevelDebuggerSupported
    true,                                          // supportsVme
    false                                          // supportCacheFlushAfterWalker
};

WorkaroundTable CNL::workaroundTable = {};
FeatureTable CNL::featureTable = {};

void CNL::setupFeatureAndWorkaroundTable(HardwareInfo *hwInfo) {
    FeatureTable *featureTable = &hwInfo->featureTable;
    WorkaroundTable *workaroundTable = &hwInfo->workaroundTable;

    featureTable->ftrL3IACoherency = true;
    featureTable->ftrGpGpuMidBatchPreempt = true;
    featureTable->ftrGpGpuThreadGroupLevelPreempt = true;
    featureTable->ftrGpGpuMidThreadLevelPreempt = true;
    featureTable->ftr3dMidBatchPreempt = true;
    featureTable->ftr3dObjectLevelPreempt = true;
    featureTable->ftrPerCtxtPreemptionGranularityControl = true;
    featureTable->ftrPPGTT = true;
    featureTable->ftrSVM = true;
    featureTable->ftrIA32eGfxPTEs = true;
    featureTable->ftrStandardMipTailFormat = true;
    featureTable->ftrDisplayYTiling = true;
    featureTable->ftrTranslationTable = true;
    featureTable->ftrUserModeTranslationTable = true;
    featureTable->ftrTileMappedResource = true;
    featureTable->ftrEnableGuC = true;
    featureTable->ftrFbc = true;
    featureTable->ftrFbc2AddressTranslation = true;
    featureTable->ftrFbcBlitterTracking = true;
    featureTable->ftrFbcCpuTracking = true;
    featureTable->ftrAstcHdr2D = true;
    featureTable->ftrAstcLdr2D = true;
    featureTable->ftrTileY = true;

    workaroundTable->wa4kAlignUVOffsetNV12LinearSurface = true;
    workaroundTable->waSendMIFLUSHBeforeVFE = true;
    workaroundTable->waReportPerfCountUseGlobalContextID = true;
    workaroundTable->waSamplerCacheFlushBetweenRedescribedSurfaceReads = true;

    if ((1 << hwInfo->platform.usRevId) & 0x3) {
        workaroundTable->waFbcLinearSurfaceStride = true;
    }
    if ((1 << hwInfo->platform.usRevId) & 0x1) {
        workaroundTable->waEncryptedEdramOnlyPartials = true;
    }
}
const HardwareInfo CNL_2x5x8::hwInfo = {
    &CNL::platform,
    &CNL::featureTable,
    &CNL::workaroundTable,
    &CNL_2x5x8::gtSystemInfo,
    CNL::capabilityTable,
};
GT_SYSTEM_INFO CNL_2x5x8::gtSystemInfo = {0};
void CNL_2x5x8::setupHardwareInfo(HardwareInfo *hwInfo, bool setupFeatureTableAndWorkaroundTable) {
    GT_SYSTEM_INFO *gtSysInfo = &hwInfo->gtSystemInfo;
    gtSysInfo->EUCount = 39;
    gtSysInfo->ThreadCount = 39 * CNL::threadsPerEu;
    gtSysInfo->SliceCount = 2;
    gtSysInfo->SubSliceCount = 10;
    gtSysInfo->L3CacheSizeInKb = 1536;
    gtSysInfo->L3BankCount = 6;
    gtSysInfo->MaxFillRate = 16;
    gtSysInfo->TotalVsThreads = 336;
    gtSysInfo->TotalHsThreads = 336;
    gtSysInfo->TotalDsThreads = 336;
    gtSysInfo->TotalGsThreads = 336;
    gtSysInfo->TotalPsThreadsWindowerRange = 64;
    gtSysInfo->CsrSizeInMb = 8;
    gtSysInfo->MaxEuPerSubSlice = CNL::maxEuPerSubslice;
    gtSysInfo->MaxSlicesSupported = CNL::maxSlicesSupported;
    gtSysInfo->MaxSubSlicesSupported = CNL::maxSubslicesSupported;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    if (setupFeatureTableAndWorkaroundTable) {
        setupFeatureAndWorkaroundTable(hwInfo);
    }
};
const HardwareInfo CNL_2x4x8::hwInfo = {
    &CNL::platform,
    &CNL::featureTable,
    &CNL::workaroundTable,
    &CNL_2x4x8::gtSystemInfo,
    CNL::capabilityTable,
};
GT_SYSTEM_INFO CNL_2x4x8::gtSystemInfo = {0};
void CNL_2x4x8::setupHardwareInfo(HardwareInfo *hwInfo, bool setupFeatureTableAndWorkaroundTable) {
    GT_SYSTEM_INFO *gtSysInfo = &hwInfo->gtSystemInfo;
    gtSysInfo->EUCount = 31;
    gtSysInfo->ThreadCount = 31 * CNL::threadsPerEu;
    gtSysInfo->SliceCount = 2;
    gtSysInfo->SubSliceCount = 8;
    gtSysInfo->L3CacheSizeInKb = 1536;
    gtSysInfo->L3BankCount = 6;
    gtSysInfo->MaxFillRate = 16;
    gtSysInfo->TotalVsThreads = 336;
    gtSysInfo->TotalHsThreads = 336;
    gtSysInfo->TotalDsThreads = 336;
    gtSysInfo->TotalGsThreads = 336;
    gtSysInfo->TotalPsThreadsWindowerRange = 64;
    gtSysInfo->CsrSizeInMb = 8;
    gtSysInfo->MaxEuPerSubSlice = CNL::maxEuPerSubslice;
    gtSysInfo->MaxSlicesSupported = CNL::maxSlicesSupported;
    gtSysInfo->MaxSubSlicesSupported = CNL::maxSubslicesSupported;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    if (setupFeatureTableAndWorkaroundTable) {
        setupFeatureAndWorkaroundTable(hwInfo);
    }
};
const HardwareInfo CNL_1x3x8::hwInfo = {
    &CNL::platform,
    &CNL::featureTable,
    &CNL::workaroundTable,
    &CNL_1x3x8::gtSystemInfo,
    CNL::capabilityTable,
};
GT_SYSTEM_INFO CNL_1x3x8::gtSystemInfo = {0};
void CNL_1x3x8::setupHardwareInfo(HardwareInfo *hwInfo, bool setupFeatureTableAndWorkaroundTable) {
    GT_SYSTEM_INFO *gtSysInfo = &hwInfo->gtSystemInfo;
    gtSysInfo->EUCount = 23;
    gtSysInfo->ThreadCount = 23 * CNL::threadsPerEu;
    gtSysInfo->SliceCount = 1;
    gtSysInfo->SubSliceCount = 3;
    gtSysInfo->L3CacheSizeInKb = 1536;
    gtSysInfo->L3BankCount = 6;
    gtSysInfo->MaxFillRate = 16;
    gtSysInfo->TotalVsThreads = 336;
    gtSysInfo->TotalHsThreads = 336;
    gtSysInfo->TotalDsThreads = 336;
    gtSysInfo->TotalGsThreads = 336;
    gtSysInfo->TotalPsThreadsWindowerRange = 64;
    gtSysInfo->CsrSizeInMb = 8;
    gtSysInfo->MaxEuPerSubSlice = CNL::maxEuPerSubslice;
    gtSysInfo->MaxSlicesSupported = CNL::maxSlicesSupported;
    gtSysInfo->MaxSubSlicesSupported = CNL::maxSubslicesSupported;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    if (setupFeatureTableAndWorkaroundTable) {
        setupFeatureAndWorkaroundTable(hwInfo);
    }
};
const HardwareInfo CNL_1x2x8::hwInfo = {
    &CNL::platform,
    &CNL::featureTable,
    &CNL::workaroundTable,
    &CNL_1x2x8::gtSystemInfo,
    CNL::capabilityTable,
};
GT_SYSTEM_INFO CNL_1x2x8::gtSystemInfo = {0};
void CNL_1x2x8::setupHardwareInfo(HardwareInfo *hwInfo, bool setupFeatureTableAndWorkaroundTable) {
    GT_SYSTEM_INFO *gtSysInfo = &hwInfo->gtSystemInfo;
    gtSysInfo->EUCount = 15;
    gtSysInfo->ThreadCount = 15 * CNL::threadsPerEu;
    gtSysInfo->SliceCount = 1;
    gtSysInfo->SubSliceCount = 2;
    gtSysInfo->L3CacheSizeInKb = 1536;
    gtSysInfo->L3BankCount = 6;
    gtSysInfo->MaxFillRate = 16;
    gtSysInfo->TotalVsThreads = 336;
    gtSysInfo->TotalHsThreads = 336;
    gtSysInfo->TotalDsThreads = 336;
    gtSysInfo->TotalGsThreads = 336;
    gtSysInfo->TotalPsThreadsWindowerRange = 64;
    gtSysInfo->CsrSizeInMb = 8;
    gtSysInfo->MaxEuPerSubSlice = CNL::maxEuPerSubslice;
    gtSysInfo->MaxSlicesSupported = CNL::maxSlicesSupported;
    gtSysInfo->MaxSubSlicesSupported = CNL::maxSubslicesSupported;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    if (setupFeatureTableAndWorkaroundTable) {
        setupFeatureAndWorkaroundTable(hwInfo);
    }
};
const HardwareInfo CNL_4x9x8::hwInfo = {
    &CNL::platform,
    &CNL::featureTable,
    &CNL::workaroundTable,
    &CNL_4x9x8::gtSystemInfo,
    CNL::capabilityTable,
};
GT_SYSTEM_INFO CNL_4x9x8::gtSystemInfo = {0};
void CNL_4x9x8::setupHardwareInfo(HardwareInfo *hwInfo, bool setupFeatureTableAndWorkaroundTable) {
    GT_SYSTEM_INFO *gtSysInfo = &hwInfo->gtSystemInfo;
    gtSysInfo->EUCount = 71;
    gtSysInfo->ThreadCount = 71 * CNL::threadsPerEu;
    gtSysInfo->SliceCount = 4;
    gtSysInfo->SubSliceCount = 36;
    gtSysInfo->L3CacheSizeInKb = 1536;
    gtSysInfo->L3BankCount = 6;
    gtSysInfo->MaxFillRate = 16;
    gtSysInfo->TotalVsThreads = 336;
    gtSysInfo->TotalHsThreads = 336;
    gtSysInfo->TotalDsThreads = 336;
    gtSysInfo->TotalGsThreads = 336;
    gtSysInfo->TotalPsThreadsWindowerRange = 64;
    gtSysInfo->CsrSizeInMb = 8;
    gtSysInfo->MaxEuPerSubSlice = CNL::maxEuPerSubslice;
    gtSysInfo->MaxSlicesSupported = CNL::maxSlicesSupported;
    gtSysInfo->MaxSubSlicesSupported = CNL::maxSubslicesSupported;
    gtSysInfo->IsL3HashModeEnabled = false;
    gtSysInfo->IsDynamicallyPopulated = false;
    if (setupFeatureTableAndWorkaroundTable) {
        setupFeatureAndWorkaroundTable(hwInfo);
    }
}

const HardwareInfo CNL::hwInfo = CNL_2x5x8::hwInfo;

void setupCNLHardwareInfoImpl(HardwareInfo *hwInfo, bool setupFeatureTableAndWorkaroundTable, const std::string &hwInfoConfig) {
    if (hwInfoConfig == "1x2x8") {
        CNL_1x2x8::setupHardwareInfo(hwInfo, setupFeatureTableAndWorkaroundTable);
    } else if (hwInfoConfig == "1x3x8") {
        CNL_1x3x8::setupHardwareInfo(hwInfo, setupFeatureTableAndWorkaroundTable);
    } else if (hwInfoConfig == "2x5x8") {
        CNL_2x5x8::setupHardwareInfo(hwInfo, setupFeatureTableAndWorkaroundTable);
    } else if (hwInfoConfig == "2x4x8") {
        CNL_2x4x8::setupHardwareInfo(hwInfo, setupFeatureTableAndWorkaroundTable);
    } else if (hwInfoConfig == "4x9x8") {
        CNL_4x9x8::setupHardwareInfo(hwInfo, setupFeatureTableAndWorkaroundTable);
    } else if (hwInfoConfig == "default") {
        // Default config
        CNL_2x5x8::setupHardwareInfo(hwInfo, setupFeatureTableAndWorkaroundTable);
    } else {
        UNRECOVERABLE_IF(true);
    }
}

void (*CNL::setupHardwareInfo)(HardwareInfo *, bool, const std::string &) = setupCNLHardwareInfoImpl;
} // namespace NEO
