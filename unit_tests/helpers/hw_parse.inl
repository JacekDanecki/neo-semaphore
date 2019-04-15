/*
 * Copyright (C) 2018-2019 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "unit_tests/helpers/hw_parse.h"

namespace NEO {

template <typename FamilyType>
void HardwareParse::findHardwareCommands(IndirectHeap *dsh) {
    typedef typename FamilyType::GPGPU_WALKER GPGPU_WALKER;
    typedef typename FamilyType::PIPELINE_SELECT PIPELINE_SELECT;
    typedef typename FamilyType::STATE_BASE_ADDRESS STATE_BASE_ADDRESS;
    typedef typename FamilyType::MEDIA_INTERFACE_DESCRIPTOR_LOAD MEDIA_INTERFACE_DESCRIPTOR_LOAD;
    typedef typename FamilyType::MEDIA_VFE_STATE MEDIA_VFE_STATE;
    typedef typename FamilyType::INTERFACE_DESCRIPTOR_DATA INTERFACE_DESCRIPTOR_DATA;
    typedef typename FamilyType::MI_BATCH_BUFFER_START MI_BATCH_BUFFER_START;
    typedef typename FamilyType::MI_LOAD_REGISTER_IMM MI_LOAD_REGISTER_IMM;

    itorWalker = find<GPGPU_WALKER *>(cmdList.begin(), cmdList.end());
    if (itorWalker != cmdList.end()) {
        cmdWalker = *itorWalker;
    }

    itorBBStartAfterWalker = find<MI_BATCH_BUFFER_START *>(itorWalker, cmdList.end());
    if (itorBBStartAfterWalker != cmdList.end()) {
        cmdBBStartAfterWalker = *itorBBStartAfterWalker;
    }
    for (auto it = cmdList.begin(); it != cmdList.end(); it++) {
        auto lri = genCmdCast<MI_LOAD_REGISTER_IMM *>(*it);
        if (lri) {
            lriList.push_back(*it);
        }
    }

    MEDIA_INTERFACE_DESCRIPTOR_LOAD *cmdMIDL = nullptr;
    itorMediaInterfaceDescriptorLoad = find<MEDIA_INTERFACE_DESCRIPTOR_LOAD *>(cmdList.begin(), itorWalker);
    if (itorMediaInterfaceDescriptorLoad != itorWalker) {
        cmdMIDL = (MEDIA_INTERFACE_DESCRIPTOR_LOAD *)*itorMediaInterfaceDescriptorLoad;
        cmdMediaInterfaceDescriptorLoad = *itorMediaInterfaceDescriptorLoad;
    }

    itorPipelineSelect = find<PIPELINE_SELECT *>(cmdList.begin(), itorWalker);
    if (itorPipelineSelect != itorWalker) {
        cmdPipelineSelect = *itorPipelineSelect;
    }

    itorMediaVfeState = find<MEDIA_VFE_STATE *>(itorPipelineSelect, itorWalker);
    if (itorMediaVfeState != itorWalker) {
        cmdMediaVfeState = *itorMediaVfeState;
    }

    STATE_BASE_ADDRESS *cmdSBA = nullptr;
    uint64_t dynamicStateHeap = 0;
    itorStateBaseAddress = find<STATE_BASE_ADDRESS *>(cmdList.begin(), itorWalker);
    if (itorStateBaseAddress != itorWalker) {
        cmdSBA = (STATE_BASE_ADDRESS *)*itorStateBaseAddress;
        cmdStateBaseAddress = *itorStateBaseAddress;

        // Extract the dynamicStateHeap
        dynamicStateHeap = cmdSBA->getDynamicStateBaseAddress();
        if (dsh && (dsh->getHeapGpuBase() == dynamicStateHeap)) {
            dynamicStateHeap = reinterpret_cast<uint64_t>(dsh->getCpuBase());
        }
        ASSERT_NE(0u, dynamicStateHeap);
    }

    // interfaceDescriptorData should be located within dynamicStateHeap
    if (cmdMIDL && cmdSBA) {
        auto iddStart = cmdMIDL->getInterfaceDescriptorDataStartAddress();
        auto iddEnd = iddStart + cmdMIDL->getInterfaceDescriptorTotalLength();
        ASSERT_LE(iddEnd, cmdSBA->getDynamicStateBufferSize() * MemoryConstants::pageSize);

        // Extract the interfaceDescriptorData
        cmdInterfaceDescriptorData = (INTERFACE_DESCRIPTOR_DATA *)(dynamicStateHeap + iddStart);
    }
}

template <typename FamilyType>
void HardwareParse::findHardwareCommands() {
    findHardwareCommands<FamilyType>(nullptr);
}

template <typename FamilyType>
const void *HardwareParse::getStatelessArgumentPointer(const Kernel &kernel, uint32_t indexArg, IndirectHeap &ioh) {
    typedef typename FamilyType::GPGPU_WALKER GPGPU_WALKER;
    typedef typename FamilyType::STATE_BASE_ADDRESS STATE_BASE_ADDRESS;

    auto cmdWalker = (GPGPU_WALKER *)this->cmdWalker;
    EXPECT_NE(nullptr, cmdWalker);

    auto cmdSBA = (STATE_BASE_ADDRESS *)cmdStateBaseAddress;
    EXPECT_NE(nullptr, cmdSBA);

    auto offsetCrossThreadData = cmdWalker->getIndirectDataStartAddress();
    EXPECT_LT(offsetCrossThreadData, cmdSBA->getIndirectObjectBufferSize() * MemoryConstants::pageSize);

    offsetCrossThreadData -= static_cast<uint32_t>(ioh.getGraphicsAllocation()->getGpuAddressToPatch());

    // Get the base of cross thread
    auto pCrossThreadData = ptrOffset(
        reinterpret_cast<const void *>(ioh.getCpuBase()),
        offsetCrossThreadData);

    // Determine where the argument is
    auto &patchInfo = kernel.getKernelInfo().patchInfo;
    for (auto &arg : patchInfo.statelessGlobalMemObjKernelArgs) {
        if (arg->ArgumentNumber == indexArg) {
            return ptrOffset(pCrossThreadData, arg->DataParamOffset);
        }
    }
    return nullptr;
}

} // namespace NEO
