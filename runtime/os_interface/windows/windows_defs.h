/*
 * Copyright (C) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <d3dkmthk.h>
#include <cstdint>

namespace OCLRT {

constexpr uintptr_t windowsMinAddress = 0x200000;

struct MonitoredFence {
    D3DKMT_HANDLE fenceHandle = 0;
    D3DGPU_VIRTUAL_ADDRESS gpuAddress = 0;
    volatile uint64_t *cpuAddress = nullptr;
    volatile uint64_t currentFenceValue = 0;
    uint64_t lastSubmittedFence = 0;
};

} // namespace OCLRT
