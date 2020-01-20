/*
 * Copyright (C) 2018-2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once
#include "core/os_interface/windows/wddm_allocation.h"

namespace NEO {

class MockWddmAllocation : public WddmAllocation {
  public:
    MockWddmAllocation() : WddmAllocation(0, GraphicsAllocation::AllocationType::UNKNOWN, nullptr, 0, nullptr, MemoryPool::MemoryNull), gpuPtr(gpuAddress), handle(handles[0]) {
    }
    using WddmAllocation::cpuPtr;
    using WddmAllocation::memoryPool;
    using WddmAllocation::size;

    D3DGPU_VIRTUAL_ADDRESS &gpuPtr;
    D3DKMT_HANDLE &handle;
};

} // namespace NEO
