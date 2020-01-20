/*
 * Copyright (C) 2017-2019 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "core/unit_tests/helpers/debug_manager_state_restore.h"
#include "core/unit_tests/page_fault_manager/mock_cpu_page_fault_manager.h"
#include "runtime/command_stream/command_stream_receiver.h"
#include "runtime/mem_obj/mem_obj_helper.h"
#include "test.h"
#include "unit_tests/mocks/mock_command_queue.h"
#include "unit_tests/mocks/mock_execution_environment.h"
#include "unit_tests/mocks/mock_memory_manager.h"
#include "unit_tests/mocks/mock_svm_manager.h"

#include "gtest/gtest.h"

using namespace NEO;

template <bool enableLocalMemory>
struct SVMMemoryAllocatorFixture {
    SVMMemoryAllocatorFixture() : executionEnvironment(*platformDevices) {}

    virtual void SetUp() {
        bool svmSupported = executionEnvironment.getHardwareInfo()->capabilityTable.ftrSvm;
        if (!svmSupported) {
            GTEST_SKIP();
        }
        executionEnvironment.initGmm();
        memoryManager = std::make_unique<MockMemoryManager>(false, enableLocalMemory, executionEnvironment);
        svmManager = std::make_unique<MockSVMAllocsManager>(memoryManager.get());
        if (enableLocalMemory) {
            memoryManager->pageFaultManager.reset(new MockPageFaultManager);
        }
    }
    virtual void TearDown() {
    }

    MockExecutionEnvironment executionEnvironment;
    std::unique_ptr<MockMemoryManager> memoryManager;
    std::unique_ptr<MockSVMAllocsManager> svmManager;
};

using SVMMemoryAllocatorTest = Test<SVMMemoryAllocatorFixture<false>>;

using SVMLocalMemoryAllocatorTest = Test<SVMMemoryAllocatorFixture<true>>;

TEST_F(SVMMemoryAllocatorTest, whenCreateZeroSizedSVMAllocationThenReturnNullptr) {
    auto ptr = svmManager->createSVMAlloc(0, 0, {});

    EXPECT_EQ(0u, svmManager->SVMAllocs.getNumAllocs());
    EXPECT_EQ(ptr, nullptr);
}

TEST_F(SVMMemoryAllocatorTest, whenRequestSVMAllocsThenReturnNonNullptr) {
    auto svmAllocs = svmManager->getSVMAllocs();
    EXPECT_NE(svmAllocs, nullptr);
}

TEST_F(SVMMemoryAllocatorTest, whenSVMAllocationIsFreedThenCannotBeGotAgain) {
    auto ptr = svmManager->createSVMAlloc(0, MemoryConstants::pageSize, {});
    EXPECT_NE(nullptr, ptr);
    auto svmData = svmManager->getSVMAlloc(ptr);
    ASSERT_NE(nullptr, svmData);
    EXPECT_NE(nullptr, svmData->gpuAllocation);
    svmData = svmManager->getSVMAlloc(ptr);
    ASSERT_NE(nullptr, svmData);
    EXPECT_NE(nullptr, svmData->gpuAllocation);
    EXPECT_EQ(1u, svmManager->SVMAllocs.getNumAllocs());
    auto svmAllocation = svmManager->getSVMAlloc(ptr)->gpuAllocation;
    EXPECT_FALSE(svmAllocation->isCoherent());

    svmManager->freeSVMAlloc(ptr);
    EXPECT_EQ(nullptr, svmManager->getSVMAlloc(ptr));
    EXPECT_EQ(0u, svmManager->SVMAllocs.getNumAllocs());
}

TEST_F(SVMMemoryAllocatorTest, whenGetSVMAllocationFromReturnedPointerAreaThenReturnSameAllocation) {
    auto ptr = svmManager->createSVMAlloc(0, MemoryConstants::pageSize, {});
    EXPECT_NE(ptr, nullptr);
    auto svmData = svmManager->getSVMAlloc(ptr);
    ASSERT_NE(nullptr, svmData);
    GraphicsAllocation *graphicsAllocation = svmData->gpuAllocation;
    EXPECT_NE(nullptr, graphicsAllocation);

    auto ptrInRange = ptrOffset(ptr, MemoryConstants::pageSize - 4);
    svmData = svmManager->getSVMAlloc(ptrInRange);
    ASSERT_NE(nullptr, svmData);
    GraphicsAllocation *graphicsAllocationInRange = svmData->gpuAllocation;
    EXPECT_NE(nullptr, graphicsAllocationInRange);

    EXPECT_EQ(graphicsAllocation, graphicsAllocationInRange);

    svmManager->freeSVMAlloc(ptr);
}

TEST_F(SVMMemoryAllocatorTest, whenGetSVMAllocationFromOutsideOfReturnedPointerAreaThenDontReturnThisAllocation) {
    auto ptr = svmManager->createSVMAlloc(0, MemoryConstants::pageSize, {});
    EXPECT_NE(ptr, nullptr);
    auto svmData = svmManager->getSVMAlloc(ptr);
    ASSERT_NE(nullptr, svmData);
    GraphicsAllocation *graphicsAllocation = svmData->gpuAllocation;
    EXPECT_NE(nullptr, graphicsAllocation);

    auto ptrBefore = ptrOffset(ptr, -4);
    svmData = svmManager->getSVMAlloc(ptrBefore);
    EXPECT_EQ(nullptr, svmData);

    auto ptrAfter = ptrOffset(ptr, MemoryConstants::pageSize);
    svmData = svmManager->getSVMAlloc(ptrAfter);
    EXPECT_EQ(nullptr, svmData);

    svmManager->freeSVMAlloc(ptr);
}

TEST_F(SVMMemoryAllocatorTest, whenCouldNotAllocateInMemoryManagerThenReturnsNullAndDoesNotChangeAllocsMap) {
    FailMemoryManager failMemoryManager(executionEnvironment);
    svmManager->memoryManager = &failMemoryManager;
    auto ptr = svmManager->createSVMAlloc(0, MemoryConstants::pageSize, {});
    EXPECT_EQ(nullptr, ptr);
    EXPECT_EQ(0u, svmManager->SVMAllocs.getNumAllocs());
    svmManager->freeSVMAlloc(ptr);
}

TEST_F(SVMMemoryAllocatorTest, whenCouldNotAllocateInMemoryManagerThenCreateUnifiedMemoryAllocationReturnsNullAndDoesNotChangeAllocsMap) {
    FailMemoryManager failMemoryManager(executionEnvironment);
    svmManager->memoryManager = &failMemoryManager;

    SVMAllocsManager::UnifiedMemoryProperties unifiedMemoryProperties;
    unifiedMemoryProperties.memoryType = InternalMemoryType::DEVICE_UNIFIED_MEMORY;
    auto ptr = svmManager->createUnifiedMemoryAllocation(0, 4096u, unifiedMemoryProperties);
    EXPECT_EQ(nullptr, ptr);
    EXPECT_EQ(0u, svmManager->SVMAllocs.getNumAllocs());
    svmManager->freeSVMAlloc(ptr);
}

TEST_F(SVMMemoryAllocatorTest, given64kbAllowedWhenAllocatingSvmMemoryThenDontPreferRenderCompression) {
    MockMemoryManager memoryManager64Kb(true, false, executionEnvironment);
    svmManager->memoryManager = &memoryManager64Kb;
    auto ptr = svmManager->createSVMAlloc(0, MemoryConstants::pageSize, {});
    EXPECT_FALSE(memoryManager64Kb.preferRenderCompressedFlagPassed);
    svmManager->freeSVMAlloc(ptr);
}

TEST_F(SVMMemoryAllocatorTest, given64kbAllowedwhenAllocatingSvmMemoryThenAllocationIsIn64kbPagePool) {
    MockMemoryManager memoryManager64Kb(true, false, executionEnvironment);
    svmManager->memoryManager = &memoryManager64Kb;
    auto ptr = svmManager->createSVMAlloc(0, MemoryConstants::pageSize, {});
    EXPECT_EQ(MemoryPool::System64KBPages, svmManager->getSVMAlloc(ptr)->gpuAllocation->getMemoryPool());
    svmManager->freeSVMAlloc(ptr);
}

TEST_F(SVMMemoryAllocatorTest, given64kbDisallowedWhenAllocatingSvmMemoryThenAllocationIsIn4kbPagePool) {
    auto ptr = svmManager->createSVMAlloc(0, MemoryConstants::pageSize, {});
    EXPECT_EQ(MemoryPool::System4KBPages, svmManager->getSVMAlloc(ptr)->gpuAllocation->getMemoryPool());
    svmManager->freeSVMAlloc(ptr);
}

TEST_F(SVMMemoryAllocatorTest, whenCoherentFlagIsPassedThenAllocationIsCoherent) {
    SVMAllocsManager::SvmAllocationProperties svmProperties;
    svmProperties.coherent = true;
    auto ptr = svmManager->createSVMAlloc(0, MemoryConstants::pageSize, svmProperties);
    EXPECT_TRUE(svmManager->getSVMAlloc(ptr)->gpuAllocation->isCoherent());
    svmManager->freeSVMAlloc(ptr);
}

TEST_F(SVMLocalMemoryAllocatorTest, whenDeviceAllocationIsCreatedThenItIsStoredWithWriteCombinedTypeInAllocationMap) {
    SVMAllocsManager::UnifiedMemoryProperties unifiedMemoryProperties;
    unifiedMemoryProperties.memoryType = InternalMemoryType::DEVICE_UNIFIED_MEMORY;
    unifiedMemoryProperties.allocationFlags.allocFlags.allocWriteCombined = true;
    auto allocationSize = 4000u;
    auto ptr = svmManager->createUnifiedMemoryAllocation(0, 4000u, unifiedMemoryProperties);
    EXPECT_NE(nullptr, ptr);
    auto allocation = svmManager->getSVMAlloc(ptr);
    EXPECT_EQ(nullptr, allocation->cpuAllocation);
    EXPECT_NE(nullptr, allocation->gpuAllocation);
    EXPECT_EQ(InternalMemoryType::DEVICE_UNIFIED_MEMORY, allocation->memoryType);
    EXPECT_EQ(allocationSize, allocation->size);
    EXPECT_EQ(allocation->gpuAllocation->getMemoryPool(), MemoryPool::LocalMemory);

    EXPECT_EQ(alignUp(allocationSize, MemoryConstants::pageSize64k), allocation->gpuAllocation->getUnderlyingBufferSize());
    EXPECT_EQ(GraphicsAllocation::AllocationType::WRITE_COMBINED, allocation->gpuAllocation->getAllocationType());

    svmManager->freeSVMAlloc(ptr);
}

TEST_F(SVMMemoryAllocatorTest, givenNoWriteCombinedFlagwhenDeviceAllocationIsCreatedThenItIsStoredWithProperTypeInAllocationMap) {
    if (is32bit) {
        GTEST_SKIP();
    }
    SVMAllocsManager::UnifiedMemoryProperties unifiedMemoryProperties;
    unifiedMemoryProperties.memoryType = InternalMemoryType::DEVICE_UNIFIED_MEMORY;
    unifiedMemoryProperties.allocationFlags.allocFlags.allocWriteCombined = false;
    auto allocationSize = 4096u;
    auto ptr = svmManager->createUnifiedMemoryAllocation(0, 4096u, unifiedMemoryProperties);
    EXPECT_NE(nullptr, ptr);
    auto allocation = svmManager->getSVMAlloc(ptr);
    EXPECT_EQ(nullptr, allocation->cpuAllocation);
    EXPECT_NE(nullptr, allocation->gpuAllocation);
    EXPECT_EQ(InternalMemoryType::DEVICE_UNIFIED_MEMORY, allocation->memoryType);
    EXPECT_EQ(allocationSize, allocation->size);

    EXPECT_EQ(alignUp(allocationSize, MemoryConstants::pageSize64k), allocation->gpuAllocation->getUnderlyingBufferSize());
    EXPECT_EQ(GraphicsAllocation::AllocationType::BUFFER, allocation->gpuAllocation->getAllocationType());

    svmManager->freeSVMAlloc(ptr);
}

TEST_F(SVMMemoryAllocatorTest, whenHostAllocationIsCreatedThenItIsStoredWithProperTypeInAllocationMap) {
    SVMAllocsManager::UnifiedMemoryProperties unifiedMemoryProperties;
    unifiedMemoryProperties.memoryType = InternalMemoryType::HOST_UNIFIED_MEMORY;
    auto allocationSize = 4096u;
    auto ptr = svmManager->createUnifiedMemoryAllocation(0, 4096u, unifiedMemoryProperties);
    EXPECT_NE(nullptr, ptr);
    auto allocation = svmManager->getSVMAlloc(ptr);
    EXPECT_EQ(nullptr, allocation->cpuAllocation);
    EXPECT_NE(nullptr, allocation->gpuAllocation);
    EXPECT_EQ(InternalMemoryType::HOST_UNIFIED_MEMORY, allocation->memoryType);
    EXPECT_EQ(allocationSize, allocation->size);

    EXPECT_EQ(alignUp(allocationSize, MemoryConstants::pageSize64k), allocation->gpuAllocation->getUnderlyingBufferSize());
    EXPECT_EQ(GraphicsAllocation::AllocationType::BUFFER_HOST_MEMORY, allocation->gpuAllocation->getAllocationType());
    EXPECT_NE(allocation->gpuAllocation->getMemoryPool(), MemoryPool::LocalMemory);
    EXPECT_NE(nullptr, allocation->gpuAllocation->getUnderlyingBuffer());
    svmManager->freeSVMAlloc(ptr);
}

TEST_F(SVMMemoryAllocatorTest, whenCouldNotAllocateInMemoryManagerThenCreateSharedUnifiedMemoryAllocationReturnsNullAndDoesNotChangeAllocsMap) {
    MockCommandQueue cmdQ;
    DebugManagerStateRestore restore;
    DebugManager.flags.AllocateSharedAllocationsWithCpuAndGpuStorage.set(true);
    FailMemoryManager failMemoryManager(executionEnvironment);
    svmManager->memoryManager = &failMemoryManager;

    SVMAllocsManager::UnifiedMemoryProperties unifiedMemoryProperties;
    unifiedMemoryProperties.memoryType = InternalMemoryType::SHARED_UNIFIED_MEMORY;
    auto ptr = svmManager->createSharedUnifiedMemoryAllocation(0, 4096u, unifiedMemoryProperties, &cmdQ);
    EXPECT_EQ(nullptr, ptr);
    EXPECT_EQ(0u, svmManager->SVMAllocs.getNumAllocs());
    svmManager->freeSVMAlloc(ptr);
}

TEST_F(SVMMemoryAllocatorTest, whenSharedAllocationIsCreatedThenItIsStoredWithProperTypeInAllocationMap) {
    MockCommandQueue cmdQ;
    SVMAllocsManager::UnifiedMemoryProperties unifiedMemoryProperties;
    unifiedMemoryProperties.memoryType = InternalMemoryType::SHARED_UNIFIED_MEMORY;
    auto allocationSize = 4096u;
    auto ptr = svmManager->createSharedUnifiedMemoryAllocation(0, 4096u, unifiedMemoryProperties, &cmdQ);
    EXPECT_NE(nullptr, ptr);
    auto allocation = svmManager->getSVMAlloc(ptr);
    EXPECT_EQ(nullptr, allocation->cpuAllocation);
    EXPECT_NE(nullptr, allocation->gpuAllocation);
    EXPECT_EQ(InternalMemoryType::SHARED_UNIFIED_MEMORY, allocation->memoryType);
    EXPECT_EQ(allocationSize, allocation->size);

    EXPECT_EQ(alignUp(allocationSize, MemoryConstants::pageSize64k), allocation->gpuAllocation->getUnderlyingBufferSize());
    EXPECT_EQ(GraphicsAllocation::AllocationType::BUFFER_HOST_MEMORY, allocation->gpuAllocation->getAllocationType());
    EXPECT_NE(allocation->gpuAllocation->getMemoryPool(), MemoryPool::LocalMemory);
    EXPECT_NE(nullptr, allocation->gpuAllocation->getUnderlyingBuffer());
    svmManager->freeSVMAlloc(ptr);
}

TEST_F(SVMLocalMemoryAllocatorTest, whenSharedAllocationIsCreatedWithDebugFlagSetThenItIsStoredWithProperTypeInAllocationMapAndHasCpuAndGpuStorage) {
    MockCommandQueue cmdQ;
    DebugManagerStateRestore restore;
    DebugManager.flags.AllocateSharedAllocationsWithCpuAndGpuStorage.set(true);

    SVMAllocsManager::UnifiedMemoryProperties unifiedMemoryProperties;
    unifiedMemoryProperties.memoryType = InternalMemoryType::SHARED_UNIFIED_MEMORY;
    auto allocationSize = 4096u;
    auto ptr = svmManager->createSharedUnifiedMemoryAllocation(0, 4096u, unifiedMemoryProperties, &cmdQ);
    EXPECT_NE(nullptr, ptr);
    auto allocation = svmManager->getSVMAlloc(ptr);
    EXPECT_NE(nullptr, allocation->cpuAllocation);
    EXPECT_NE(nullptr, allocation->gpuAllocation);
    EXPECT_EQ(InternalMemoryType::SHARED_UNIFIED_MEMORY, allocation->memoryType);
    EXPECT_EQ(allocationSize, allocation->size);

    EXPECT_EQ(alignUp(allocationSize, 2u * MB), allocation->gpuAllocation->getUnderlyingBufferSize());
    EXPECT_EQ(alignUp(allocationSize, 2u * MB), allocation->cpuAllocation->getUnderlyingBufferSize());

    EXPECT_EQ(GraphicsAllocation::AllocationType::SVM_GPU, allocation->gpuAllocation->getAllocationType());
    EXPECT_EQ(GraphicsAllocation::AllocationType::SVM_CPU, allocation->cpuAllocation->getAllocationType());

    EXPECT_EQ(allocation->gpuAllocation->getMemoryPool(), MemoryPool::LocalMemory);
    EXPECT_NE(allocation->cpuAllocation->getMemoryPool(), MemoryPool::LocalMemory);

    EXPECT_NE(nullptr, allocation->gpuAllocation->getUnderlyingBuffer());
    svmManager->freeSVMAlloc(ptr);
}

TEST_F(SVMLocalMemoryAllocatorTest, whenSharedAllocationIsCreatedWithLocalMemoryAndRegisteredPageFaultHandlerThenItIsStoredWithProperTypeInAllocationMapAndHasCpuAndGpuStorage) {
    MockCommandQueue cmdQ;
    DebugManagerStateRestore restore;
    DebugManager.flags.EnableLocalMemory.set(1);

    SVMAllocsManager::UnifiedMemoryProperties unifiedMemoryProperties;
    unifiedMemoryProperties.memoryType = InternalMemoryType::SHARED_UNIFIED_MEMORY;
    auto allocationSize = 4096u;
    auto ptr = svmManager->createSharedUnifiedMemoryAllocation(0, 4096u, unifiedMemoryProperties, &cmdQ);
    EXPECT_NE(nullptr, ptr);
    auto allocation = svmManager->getSVMAlloc(ptr);
    EXPECT_NE(nullptr, allocation->cpuAllocation);
    EXPECT_NE(nullptr, allocation->gpuAllocation);
    EXPECT_EQ(InternalMemoryType::SHARED_UNIFIED_MEMORY, allocation->memoryType);
    EXPECT_EQ(allocationSize, allocation->size);

    EXPECT_EQ(alignUp(allocationSize, 2u * MB), allocation->gpuAllocation->getUnderlyingBufferSize());
    EXPECT_EQ(alignUp(allocationSize, 2u * MB), allocation->cpuAllocation->getUnderlyingBufferSize());

    EXPECT_EQ(GraphicsAllocation::AllocationType::SVM_GPU, allocation->gpuAllocation->getAllocationType());
    EXPECT_EQ(GraphicsAllocation::AllocationType::SVM_CPU, allocation->cpuAllocation->getAllocationType());

    EXPECT_EQ(allocation->gpuAllocation->getMemoryPool(), MemoryPool::LocalMemory);
    EXPECT_NE(allocation->cpuAllocation->getMemoryPool(), MemoryPool::LocalMemory);

    EXPECT_NE(nullptr, allocation->gpuAllocation->getUnderlyingBuffer());
    svmManager->freeSVMAlloc(ptr);
}

TEST_F(SVMMemoryAllocatorTest, givenSharedAllocationsDebugFlagWhenDeviceMemoryIsAllocatedThenOneStorageIsProduced) {
    DebugManagerStateRestore restore;
    DebugManager.flags.AllocateSharedAllocationsWithCpuAndGpuStorage.set(true);

    SVMAllocsManager::UnifiedMemoryProperties unifiedMemoryProperties;
    unifiedMemoryProperties.memoryType = InternalMemoryType::DEVICE_UNIFIED_MEMORY;
    auto allocationSize = 4096u;
    auto ptr = svmManager->createUnifiedMemoryAllocation(0, 4096u, unifiedMemoryProperties);
    EXPECT_NE(nullptr, ptr);
    auto allocation = svmManager->getSVMAlloc(ptr);
    EXPECT_EQ(nullptr, allocation->cpuAllocation);
    EXPECT_NE(nullptr, allocation->gpuAllocation);
    EXPECT_EQ(InternalMemoryType::DEVICE_UNIFIED_MEMORY, allocation->memoryType);
    EXPECT_EQ(allocationSize, allocation->size);

    EXPECT_EQ(alignUp(allocationSize, MemoryConstants::pageSize64k), allocation->gpuAllocation->getUnderlyingBufferSize());
    EXPECT_EQ(GraphicsAllocation::AllocationType::BUFFER, allocation->gpuAllocation->getAllocationType());

    svmManager->freeSVMAlloc(ptr);
}
TEST(SvmAllocationPropertiesTests, givenDifferentMemFlagsWhenGettingSvmAllocationPropertiesThenPropertiesAreCorrectlySet) {
    SVMAllocsManager::SvmAllocationProperties allocationProperties = MemObjHelper::getSvmAllocationProperties(0);
    EXPECT_FALSE(allocationProperties.coherent);
    EXPECT_FALSE(allocationProperties.hostPtrReadOnly);
    EXPECT_FALSE(allocationProperties.readOnly);

    allocationProperties = MemObjHelper::getSvmAllocationProperties(CL_MEM_SVM_FINE_GRAIN_BUFFER);
    EXPECT_TRUE(allocationProperties.coherent);
    EXPECT_FALSE(allocationProperties.hostPtrReadOnly);
    EXPECT_FALSE(allocationProperties.readOnly);

    allocationProperties = MemObjHelper::getSvmAllocationProperties(CL_MEM_HOST_READ_ONLY);
    EXPECT_FALSE(allocationProperties.coherent);
    EXPECT_TRUE(allocationProperties.hostPtrReadOnly);
    EXPECT_FALSE(allocationProperties.readOnly);

    allocationProperties = MemObjHelper::getSvmAllocationProperties(CL_MEM_HOST_NO_ACCESS);
    EXPECT_FALSE(allocationProperties.coherent);
    EXPECT_TRUE(allocationProperties.hostPtrReadOnly);
    EXPECT_FALSE(allocationProperties.readOnly);

    allocationProperties = MemObjHelper::getSvmAllocationProperties(CL_MEM_READ_ONLY);
    EXPECT_FALSE(allocationProperties.coherent);
    EXPECT_FALSE(allocationProperties.hostPtrReadOnly);
    EXPECT_TRUE(allocationProperties.readOnly);

    allocationProperties = MemObjHelper::getSvmAllocationProperties(CL_MEM_SVM_FINE_GRAIN_BUFFER | CL_MEM_HOST_READ_ONLY);
    EXPECT_TRUE(allocationProperties.coherent);
    EXPECT_TRUE(allocationProperties.hostPtrReadOnly);
    EXPECT_FALSE(allocationProperties.readOnly);

    allocationProperties = MemObjHelper::getSvmAllocationProperties(CL_MEM_SVM_FINE_GRAIN_BUFFER | CL_MEM_READ_ONLY);
    EXPECT_TRUE(allocationProperties.coherent);
    EXPECT_FALSE(allocationProperties.hostPtrReadOnly);
    EXPECT_TRUE(allocationProperties.readOnly);
}

TEST_F(SVMMemoryAllocatorTest, whenReadOnlySvmAllocationCreatedThenGraphicsAllocationHasWriteableFlagFalse) {
    SVMAllocsManager::SvmAllocationProperties svmProperties;
    svmProperties.readOnly = true;
    void *svm = svmManager->createSVMAlloc(0, 4096, svmProperties);
    EXPECT_NE(nullptr, svm);

    auto svmData = svmManager->getSVMAlloc(svm);
    ASSERT_NE(nullptr, svmData);
    GraphicsAllocation *svmAllocation = svmData->gpuAllocation;
    EXPECT_NE(nullptr, svmAllocation);
    EXPECT_FALSE(svmAllocation->isMemObjectsAllocationWithWritableFlags());

    svmManager->freeSVMAlloc(svm);
}

TEST_F(SVMLocalMemoryAllocatorTest, whenAllocatingSvmThenExpectCpuAllocationWithPointerAndGpuAllocationWithSameGpuAddress) {
    auto ptr = svmManager->createSVMAlloc(0, MemoryConstants::pageSize, {});
    EXPECT_NE(ptr, nullptr);
    auto svmData = svmManager->getSVMAlloc(ptr);
    ASSERT_NE(nullptr, svmData);
    GraphicsAllocation *cpuAllocation = svmData->cpuAllocation;
    EXPECT_NE(nullptr, cpuAllocation);
    EXPECT_EQ(ptr, cpuAllocation->getUnderlyingBuffer());

    GraphicsAllocation *gpuAllocation = svmData->gpuAllocation;
    EXPECT_NE(nullptr, gpuAllocation);
    EXPECT_EQ(reinterpret_cast<uint64_t>(ptr), gpuAllocation->getGpuAddress());

    svmManager->freeSVMAlloc(ptr);
}

TEST_F(SVMLocalMemoryAllocatorTest, whenGetSVMAllocationFromOutsideOfReturnedPointerAreaThenDontReturnThisAllocation) {
    auto ptr = svmManager->createSVMAlloc(0, MemoryConstants::pageSize, {});
    EXPECT_NE(ptr, nullptr);
    auto svmData = svmManager->getSVMAlloc(ptr);
    ASSERT_NE(nullptr, svmData);
    GraphicsAllocation *graphicsAllocation = svmData->gpuAllocation;
    EXPECT_NE(nullptr, graphicsAllocation);

    auto ptrBefore = ptrOffset(ptr, -4);
    svmData = svmManager->getSVMAlloc(ptrBefore);
    EXPECT_EQ(nullptr, svmData);

    auto ptrAfter = ptrOffset(ptr, MemoryConstants::pageSize);
    svmData = svmManager->getSVMAlloc(ptrAfter);
    EXPECT_EQ(nullptr, svmData);

    svmManager->freeSVMAlloc(ptr);
}

TEST_F(SVMLocalMemoryAllocatorTest, whenCouldNotAllocateCpuAllocationInMemoryManagerThenReturnsNullAndDoesNotChangeAllocsMap) {
    FailMemoryManager failMemoryManager(false, true, executionEnvironment);
    svmManager->memoryManager = &failMemoryManager;
    auto ptr = svmManager->createSVMAlloc(0, MemoryConstants::pageSize, {});
    EXPECT_EQ(nullptr, ptr);
    EXPECT_EQ(0u, svmManager->SVMAllocs.getNumAllocs());
    svmManager->freeSVMAlloc(ptr);
}

TEST_F(SVMLocalMemoryAllocatorTest, whenCouldNotAllocateGpuAllocationInMemoryManagerThenReturnsNullAndDoesNotChangeAllocsMap) {
    FailMemoryManager failMemoryManager(1, executionEnvironment, true);
    svmManager->memoryManager = &failMemoryManager;
    auto ptr = svmManager->createSVMAlloc(0, MemoryConstants::pageSize, {});
    EXPECT_EQ(nullptr, ptr);
    EXPECT_EQ(0u, svmManager->SVMAllocs.getNumAllocs());
    svmManager->freeSVMAlloc(ptr);
}

TEST_F(SVMLocalMemoryAllocatorTest, whenCouldNotReserveCpuAddressRangeInMemoryManagerThenReturnsNullAndDoesNotChangeAllocsMap) {
    memoryManager->failReserveAddress = true;
    auto ptr = svmManager->createSVMAlloc(0, MemoryConstants::pageSize, {});
    EXPECT_EQ(nullptr, ptr);
    EXPECT_EQ(0u, svmManager->SVMAllocs.getNumAllocs());
}

struct MemoryManagerPropertiesCheck : public MockMemoryManager {
    using MockMemoryManager::MockMemoryManager;

    GraphicsAllocation *allocateGraphicsMemoryWithProperties(const AllocationProperties &properties) override {
        return this->allocateGraphicsMemoryWithProperties(properties, nullptr);
    }

    GraphicsAllocation *allocateGraphicsMemoryWithProperties(const AllocationProperties &properties, const void *ptr) override {
        this->multiOsContextCapablePassed = properties.flags.multiOsContextCapable;
        this->multiStorageResourcePassed = properties.multiStorageResource;
        this->subDevicesBitfieldPassed = properties.subDevicesBitfield;
        return MockMemoryManager::allocateGraphicsMemoryWithProperties(properties, ptr);
    }

    bool multiOsContextCapablePassed;
    bool multiStorageResourcePassed;
    DeviceBitfield subDevicesBitfieldPassed;
};

struct UnifiedMemoryManagerPropertiesTest : public ::testing::Test {
    void SetUp() override {
        bool svmSupported = executionEnvironment.getHardwareInfo()->capabilityTable.ftrSvm;
        if (!svmSupported) {
            GTEST_SKIP();
        }
        memoryManager = std::make_unique<MemoryManagerPropertiesCheck>(false, true, executionEnvironment);
        svmManager = std::make_unique<MockSVMAllocsManager>(memoryManager.get());
        memoryManager->pageFaultManager.reset(new MockPageFaultManager);
    }

    MockExecutionEnvironment executionEnvironment;
    std::unique_ptr<MemoryManagerPropertiesCheck> memoryManager;
    std::unique_ptr<MockSVMAllocsManager> svmManager;
};

TEST_F(UnifiedMemoryManagerPropertiesTest, givenDeviceBitfieldWithMultipleBitsSetWhenSharedUnifiedMemoryAllocationIsCreatedThenProperPropertiesArePassedToMemoryManager) {
    MockCommandQueue cmdQ;

    SVMAllocsManager::UnifiedMemoryProperties unifiedMemoryProperties;
    unifiedMemoryProperties.memoryType = InternalMemoryType::SHARED_UNIFIED_MEMORY;
    unifiedMemoryProperties.subdeviceBitfield = DeviceBitfield(0xf);

    auto ptr = svmManager->createSharedUnifiedMemoryAllocation(0, 4096u, unifiedMemoryProperties, &cmdQ);

    EXPECT_TRUE(memoryManager->multiOsContextCapablePassed);
    EXPECT_FALSE(memoryManager->multiStorageResourcePassed);
    EXPECT_EQ(unifiedMemoryProperties.subdeviceBitfield, memoryManager->subDevicesBitfieldPassed);

    svmManager->freeSVMAlloc(ptr);
}

TEST_F(UnifiedMemoryManagerPropertiesTest, givenDeviceBitfieldWithSingleBitSetWhenSharedUnifiedMemoryAllocationIsCreatedThenProperPropertiesArePassedToMemoryManager) {
    MockCommandQueue cmdQ;

    SVMAllocsManager::UnifiedMemoryProperties unifiedMemoryProperties;
    unifiedMemoryProperties.memoryType = InternalMemoryType::SHARED_UNIFIED_MEMORY;
    unifiedMemoryProperties.subdeviceBitfield = DeviceBitfield(0x8);

    auto ptr = svmManager->createSharedUnifiedMemoryAllocation(0, 4096u, unifiedMemoryProperties, &cmdQ);

    EXPECT_FALSE(memoryManager->multiOsContextCapablePassed);
    EXPECT_FALSE(memoryManager->multiStorageResourcePassed);
    EXPECT_EQ(unifiedMemoryProperties.subdeviceBitfield, memoryManager->subDevicesBitfieldPassed);

    svmManager->freeSVMAlloc(ptr);
}

TEST_F(UnifiedMemoryManagerPropertiesTest, givenDeviceBitfieldWithMultipleBitsSetWhenDeviceUnifiedMemoryAllocationIsCreatedThenProperPropertiesArePassedToMemoryManager) {
    SVMAllocsManager::UnifiedMemoryProperties unifiedMemoryProperties;
    unifiedMemoryProperties.memoryType = InternalMemoryType::SHARED_UNIFIED_MEMORY;
    unifiedMemoryProperties.subdeviceBitfield = DeviceBitfield(0xf);

    auto ptr = svmManager->createUnifiedMemoryAllocation(0, 4096u, unifiedMemoryProperties);

    EXPECT_TRUE(memoryManager->multiOsContextCapablePassed);
    EXPECT_TRUE(memoryManager->multiStorageResourcePassed);
    EXPECT_EQ(unifiedMemoryProperties.subdeviceBitfield, memoryManager->subDevicesBitfieldPassed);

    svmManager->freeSVMAlloc(ptr);
}

TEST_F(UnifiedMemoryManagerPropertiesTest, givenDeviceBitfieldWithSingleBitSetWhenDeviceUnifiedMemoryAllocationIsCreatedThenProperPropertiesArePassedToMemoryManager) {
    SVMAllocsManager::UnifiedMemoryProperties unifiedMemoryProperties;
    unifiedMemoryProperties.memoryType = InternalMemoryType::SHARED_UNIFIED_MEMORY;
    unifiedMemoryProperties.subdeviceBitfield = DeviceBitfield(0x8);

    auto ptr = svmManager->createUnifiedMemoryAllocation(0, 4096u, unifiedMemoryProperties);

    EXPECT_FALSE(memoryManager->multiOsContextCapablePassed);
    EXPECT_FALSE(memoryManager->multiStorageResourcePassed);
    EXPECT_EQ(unifiedMemoryProperties.subdeviceBitfield, memoryManager->subDevicesBitfieldPassed);

    svmManager->freeSVMAlloc(ptr);
}