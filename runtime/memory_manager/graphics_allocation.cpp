/*
 * Copyright (C) 2017-2018 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "runtime/helpers/aligned_memory.h"
#include "runtime/memory_manager/graphics_allocation.h"

namespace OCLRT {
bool GraphicsAllocation::isL3Capable() {
    auto ptr = ptrOffset(cpuPtr, static_cast<size_t>(this->allocationOffset));
    if (alignUp(ptr, MemoryConstants::cacheLineSize) == ptr && alignUp(this->size, MemoryConstants::cacheLineSize) == this->size) {
        return true;
    }
    return false;
}
GraphicsAllocation::GraphicsAllocation(void *cpuPtrIn, uint64_t gpuAddress, uint64_t baseAddress, size_t sizeIn, uint32_t osContextCount, bool shareable) : gpuBaseAddress(baseAddress),
                                                                                                                                                            size(sizeIn),
                                                                                                                                                            cpuPtr(cpuPtrIn),
                                                                                                                                                            gpuAddress(gpuAddress),
                                                                                                                                                            shareable(shareable) {
    usageInfos.resize(maxOsContextCount);
}

GraphicsAllocation::GraphicsAllocation(void *cpuPtrIn, size_t sizeIn, osHandle sharedHandleIn, uint32_t osContextCount, bool shareable) : size(sizeIn),
                                                                                                                                          cpuPtr(cpuPtrIn),
                                                                                                                                          gpuAddress(castToUint64(cpuPtrIn)),
                                                                                                                                          sharedHandle(sharedHandleIn),
                                                                                                                                          shareable(shareable) {
    usageInfos.resize(maxOsContextCount);
}
GraphicsAllocation::~GraphicsAllocation() = default;

void GraphicsAllocation::updateTaskCount(uint32_t newTaskCount, uint32_t contextId) {
    if (usageInfos[contextId].taskCount == objectNotUsed) {
        registeredContextsNum++;
    }
    if (newTaskCount == objectNotUsed) {
        registeredContextsNum--;
    }
    usageInfos[contextId].taskCount = newTaskCount;
}
constexpr uint32_t GraphicsAllocation::objectNotUsed;
constexpr uint32_t GraphicsAllocation::objectNotResident;
} // namespace OCLRT
