/*
 * Copyright (C) 2017-2019 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "runtime/aub/aub_helper_bdw_plus.inl"
#include "runtime/helpers/flat_batch_buffer_helper_hw.inl"
#include "runtime/helpers/hw_helper_bdw_plus.inl"

namespace NEO {
typedef CNLFamily Family;

template <>
void HwHelperHw<Family>::setCapabilityCoherencyFlag(const HardwareInfo *pHwInfo, bool &coherencyFlag) {
    if (pHwInfo->platform.usRevId < 0x4) {
        coherencyFlag = false;
    } else {
        coherencyFlag = true;
    }
}

template <>
bool HwHelperHw<Family>::setupPreemptionRegisters(HardwareInfo *pHwInfo, bool enable) {
    pHwInfo->capabilityTable.whitelistedRegisters.csChicken1_0x2580 = enable;
    return pHwInfo->capabilityTable.whitelistedRegisters.csChicken1_0x2580;
}

template class AubHelperHw<Family>;
template class HwHelperHw<Family>;
template class FlatBatchBufferHelperHw<Family>;
template struct PipeControlHelper<Family>;
} // namespace NEO
