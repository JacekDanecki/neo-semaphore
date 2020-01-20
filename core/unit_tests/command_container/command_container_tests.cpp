/*
 * Copyright (C) 2017-2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "core/command_container/cmdcontainer.h"
#include "test.h"
#include "unit_tests/fixtures/device_fixture.h"
#include "unit_tests/mocks/mock_graphics_allocation.h"

using namespace NEO;

class CommandContainerTest : public DeviceFixture,
                             public ::testing::Test {

  public:
    void SetUp() override {
        ::testing::Test::SetUp();
        DeviceFixture::SetUp();
    }
    void TearDown() override {
        DeviceFixture::TearDown();
        ::testing::Test::TearDown();
    }
};

struct CommandContainerHeapStateTests : public ::testing::Test {
    class MyMockCommandContainer : public CommandContainer {
      public:
        using CommandContainer::dirtyHeaps;
    };

    MyMockCommandContainer myCommandContainer;
};

TEST_F(CommandContainerHeapStateTests, givenDirtyHeapsWhenSettingStateForAllThenValuesAreCorrect) {
    EXPECT_EQ(std::numeric_limits<uint32_t>::max(), myCommandContainer.dirtyHeaps);
    EXPECT_TRUE(myCommandContainer.isAnyHeapDirty());

    myCommandContainer.setDirtyStateForAllHeaps(false);
    EXPECT_EQ(0u, myCommandContainer.dirtyHeaps);
    EXPECT_FALSE(myCommandContainer.isAnyHeapDirty());

    for (uint32_t i = 0; i < HeapType::NUM_TYPES; i++) {
        HeapType heapType = static_cast<HeapType>(i);
        EXPECT_FALSE(myCommandContainer.isHeapDirty(heapType));
    }

    myCommandContainer.setDirtyStateForAllHeaps(true);
    EXPECT_EQ(std::numeric_limits<uint32_t>::max(), myCommandContainer.dirtyHeaps);

    for (uint32_t i = 0; i < HeapType::NUM_TYPES; i++) {
        HeapType heapType = static_cast<HeapType>(i);
        EXPECT_TRUE(myCommandContainer.isHeapDirty(heapType));
    }
}

TEST_F(CommandContainerHeapStateTests, givenDirtyHeapsWhenSettingStateForSingleHeapThenValuesAreCorrect) {
    myCommandContainer.dirtyHeaps = 0;
    EXPECT_FALSE(myCommandContainer.isAnyHeapDirty());

    uint32_t controlVariable = 0;
    for (uint32_t i = 0; i < HeapType::NUM_TYPES; i++) {
        HeapType heapType = static_cast<HeapType>(i);

        EXPECT_FALSE(myCommandContainer.isHeapDirty(heapType));
        myCommandContainer.setHeapDirty(heapType);
        EXPECT_TRUE(myCommandContainer.isHeapDirty(heapType));
        EXPECT_TRUE(myCommandContainer.isAnyHeapDirty());

        controlVariable |= (1 << i);
        EXPECT_EQ(controlVariable, myCommandContainer.dirtyHeaps);
    }

    for (uint32_t i = 0; i < HeapType::NUM_TYPES; i++) {
        HeapType heapType = static_cast<HeapType>(i);
        EXPECT_TRUE(myCommandContainer.isHeapDirty(heapType));
    }
}

TEST_F(CommandContainerTest, givenCmdContainerWhenAllocatingHeapsThenSetCorrectAllocationTypes) {
    CommandContainer cmdContainer;
    cmdContainer.initialize(pDevice);

    for (uint32_t i = 0; i < HeapType::NUM_TYPES; i++) {
        HeapType heapType = static_cast<HeapType>(i);
        auto heap = cmdContainer.getIndirectHeap(heapType);

        if (HeapType::INDIRECT_OBJECT == heapType) {
            EXPECT_EQ(GraphicsAllocation::AllocationType::INTERNAL_HEAP, heap->getGraphicsAllocation()->getAllocationType());
            EXPECT_NE(0u, heap->getHeapGpuStartOffset());
        } else {
            EXPECT_EQ(GraphicsAllocation::AllocationType::LINEAR_STREAM, heap->getGraphicsAllocation()->getAllocationType());
            EXPECT_EQ(0u, heap->getHeapGpuStartOffset());
        }
    }
}

TEST_F(CommandContainerTest, givenCommandContainerWhenInitializeThenEverythingIsInitialized) {
    CommandContainer cmdContainer;
    auto status = cmdContainer.initialize(pDevice);
    EXPECT_TRUE(status);

    EXPECT_EQ(pDevice, cmdContainer.getDevice());
    EXPECT_NE(cmdContainer.getHeapHelper(), nullptr);
    EXPECT_EQ(cmdContainer.getCmdBufferAllocations().size(), 1u);
    EXPECT_NE(cmdContainer.getCommandStream(), nullptr);

    for (uint32_t i = 0; i < HeapType::NUM_TYPES; i++) {
        auto indirectHeap = cmdContainer.getIndirectHeap(static_cast<HeapType>(i));
        auto heapAllocation = cmdContainer.getIndirectHeapAllocation(static_cast<HeapType>(i));
        EXPECT_EQ(indirectHeap->getGraphicsAllocation(), heapAllocation);
    }
    EXPECT_EQ(cmdContainer.getInstructionHeapBaseAddress(), pDevice->getMemoryManager()->getInternalHeapBaseAddress(0));
}

TEST_F(CommandContainerTest, givenCommandContainerWhenInitializeWithoutDeviceThenReturnedFalse) {
    CommandContainer cmdContainer;
    auto status = cmdContainer.initialize(nullptr);
    EXPECT_FALSE(status);
}

TEST_F(CommandContainerTest, givenCommandContainerWhenSettingIndirectHeapAllocationThenAllocationIsSet) {
    CommandContainer cmdContainer;
    MockGraphicsAllocation mockAllocation;
    auto heapType = HeapType::DYNAMIC_STATE;
    cmdContainer.setIndirectHeapAllocation(heapType, &mockAllocation);
    EXPECT_EQ(cmdContainer.getIndirectHeapAllocation(heapType), &mockAllocation);
}

TEST_F(CommandContainerTest, givenHeapAllocationsWhenDestroyCommandContainerThenHeapAllocationsAreReused) {
    std::unique_ptr<CommandContainer> cmdContainer(new CommandContainer);
    cmdContainer->initialize(pDevice);
    auto heapAllocationsAddress = cmdContainer->getIndirectHeapAllocation(HeapType::DYNAMIC_STATE)->getUnderlyingBuffer();
    cmdContainer.reset(new CommandContainer);
    cmdContainer->initialize(pDevice);
    bool status = false;
    for (uint32_t i = 0; i < HeapType::NUM_TYPES && !status; i++) {
        status = cmdContainer->getIndirectHeapAllocation(static_cast<HeapType>(i))->getUnderlyingBuffer() == heapAllocationsAddress;
    }
    EXPECT_TRUE(status);
}

TEST_F(CommandContainerTest, givenCommandContainerWhenResetTheanStreamsAreNotUsed) {
    CommandContainer cmdContainer;
    cmdContainer.initialize(pDevice);
    LinearStream stream;
    uint32_t usedSize = 1;
    cmdContainer.getCommandStream()->getSpace(usedSize);
    EXPECT_EQ(usedSize, cmdContainer.getCommandStream()->getUsed());
    cmdContainer.reset();
    EXPECT_NE(usedSize, cmdContainer.getCommandStream()->getUsed());
    EXPECT_EQ(0u, cmdContainer.getCommandStream()->getUsed());
}

TEST_F(CommandContainerTest, givenCommandContainerWhenWantToAddNullPtrToResidencyContainerThenNothingIsAdded) {
    CommandContainer cmdContainer;
    cmdContainer.initialize(pDevice);
    auto size = cmdContainer.getResidencyContainer().size();
    cmdContainer.addToResidencyContainer(nullptr);
    EXPECT_EQ(cmdContainer.getResidencyContainer().size(), size);
}

TEST_F(CommandContainerTest, givenCommandContainerWhenWantToAddAleradyAddedAllocationThenNothingIsAdded) {
    CommandContainer cmdContainer;
    cmdContainer.initialize(pDevice);
    MockGraphicsAllocation mockAllocation;

    auto sizeBefore = cmdContainer.getResidencyContainer().size();

    cmdContainer.addToResidencyContainer(&mockAllocation);
    auto sizeAfterFirstAdd = cmdContainer.getResidencyContainer().size();

    EXPECT_NE(sizeBefore, sizeAfterFirstAdd);

    cmdContainer.addToResidencyContainer(&mockAllocation);
    auto sizeAfterSecondAdd = cmdContainer.getResidencyContainer().size();

    EXPECT_EQ(sizeAfterFirstAdd, sizeAfterSecondAdd);
}

TEST_F(CommandContainerTest, givenAvailableSpaceWhenGetHeapWithRequiredSizeAndAlignmentCalledThenExistingAllocationIsReturned) {
    std::unique_ptr<CommandContainer> cmdContainer(new CommandContainer);
    cmdContainer->initialize(pDevice);
    cmdContainer->setDirtyStateForAllHeaps(false);
    auto heapAllocation = cmdContainer->getIndirectHeapAllocation(HeapType::SURFACE_STATE);
    auto heap = cmdContainer->getIndirectHeap(HeapType::SURFACE_STATE);

    const size_t sizeRequested = 32;
    const size_t alignment = 32;

    EXPECT_GE(heap->getAvailableSpace(), sizeRequested + alignment);

    auto heapRequested = cmdContainer->getHeapWithRequiredSizeAndAlignment(HeapType::SURFACE_STATE, sizeRequested, alignment);
    auto newAllocation = heapRequested->getGraphicsAllocation();

    EXPECT_EQ(heap, heapRequested);
    EXPECT_EQ(heapAllocation, newAllocation);

    EXPECT_TRUE((reinterpret_cast<size_t>(heapRequested->getSpace(0)) & (alignment - 1)) == 0);
    EXPECT_FALSE(cmdContainer->isHeapDirty(HeapType::SURFACE_STATE));
}

TEST_F(CommandContainerTest, givenUnalignedAvailableSpaceWhenGetHeapWithRequiredSizeAndAlignmentCalledThenHeapReturnedIsCorrectlyAligned) {
    std::unique_ptr<CommandContainer> cmdContainer(new CommandContainer);
    cmdContainer->initialize(pDevice);
    cmdContainer->setDirtyStateForAllHeaps(false);
    auto heapAllocation = cmdContainer->getIndirectHeapAllocation(HeapType::SURFACE_STATE);
    auto heap = cmdContainer->getIndirectHeap(HeapType::SURFACE_STATE);

    const size_t sizeRequested = 32;
    const size_t alignment = 32;

    heap->getSpace(sizeRequested / 2);

    EXPECT_GE(heap->getAvailableSpace(), sizeRequested + alignment);

    auto heapRequested = cmdContainer->getHeapWithRequiredSizeAndAlignment(HeapType::SURFACE_STATE, sizeRequested, alignment);
    auto newAllocation = heapRequested->getGraphicsAllocation();

    EXPECT_EQ(heap, heapRequested);
    EXPECT_EQ(heapAllocation, newAllocation);

    EXPECT_TRUE((reinterpret_cast<size_t>(heapRequested->getSpace(0)) & (alignment - 1)) == 0);
    EXPECT_FALSE(cmdContainer->isHeapDirty(HeapType::SURFACE_STATE));
}

TEST_F(CommandContainerTest, givenNoAlignmentAndAvailableSpaceWhenGetHeapWithRequiredSizeAndAlignmentCalledThenHeapReturnedIsNotAligned) {
    std::unique_ptr<CommandContainer> cmdContainer(new CommandContainer);
    cmdContainer->initialize(pDevice);
    cmdContainer->setDirtyStateForAllHeaps(false);
    auto heapAllocation = cmdContainer->getIndirectHeapAllocation(HeapType::SURFACE_STATE);
    auto heap = cmdContainer->getIndirectHeap(HeapType::SURFACE_STATE);

    const size_t sizeRequested = 32;
    const size_t alignment = 0;

    heap->getSpace(sizeRequested / 2);

    EXPECT_GE(heap->getAvailableSpace(), sizeRequested + alignment);

    auto heapRequested = cmdContainer->getHeapWithRequiredSizeAndAlignment(HeapType::SURFACE_STATE, sizeRequested, alignment);
    auto newAllocation = heapRequested->getGraphicsAllocation();

    EXPECT_EQ(heap, heapRequested);
    EXPECT_EQ(heapAllocation, newAllocation);

    EXPECT_TRUE((reinterpret_cast<size_t>(heapRequested->getSpace(0)) & (sizeRequested / 2)) == sizeRequested / 2);
    EXPECT_FALSE(cmdContainer->isHeapDirty(HeapType::SURFACE_STATE));
}

TEST_F(CommandContainerTest, givenNotEnoughSpaceWhenGetHeapWithRequiredSizeAndAlignmentCalledThenNewAllocationIsReturned) {
    std::unique_ptr<CommandContainer> cmdContainer(new CommandContainer);
    cmdContainer->initialize(pDevice);
    cmdContainer->setDirtyStateForAllHeaps(false);
    auto heapAllocation = cmdContainer->getIndirectHeapAllocation(HeapType::SURFACE_STATE);
    auto heap = cmdContainer->getIndirectHeap(HeapType::SURFACE_STATE);

    const size_t sizeRequested = 32;
    const size_t alignment = 32;
    size_t availableSize = heap->getAvailableSpace();

    heap->getSpace(availableSize - sizeRequested / 2);

    EXPECT_LT(heap->getAvailableSpace(), sizeRequested + alignment);

    auto heapRequested = cmdContainer->getHeapWithRequiredSizeAndAlignment(HeapType::SURFACE_STATE, sizeRequested, alignment);
    auto newAllocation = heapRequested->getGraphicsAllocation();

    EXPECT_EQ(heap, heapRequested);
    EXPECT_NE(heapAllocation, newAllocation);

    EXPECT_TRUE((reinterpret_cast<size_t>(heapRequested->getSpace(0)) & (alignment - 1)) == 0);
    EXPECT_TRUE(cmdContainer->isHeapDirty(HeapType::SURFACE_STATE));

    for (auto deallocation : cmdContainer->getDeallocationContainer()) {
        cmdContainer->getDevice()->getMemoryManager()->freeGraphicsMemory(deallocation);
    }
}

TEST_F(CommandContainerTest, whenAllocateNextCmdBufferIsCalledThenNewAllocationIsCreatedAndCommandStreamReplaced) {
    std::unique_ptr<CommandContainer> cmdContainer(new CommandContainer);
    cmdContainer->initialize(pDevice);
    auto stream = cmdContainer->getCommandStream();
    ASSERT_NE(nullptr, stream);

    auto initialBuffer = stream->getSpace(0);
    EXPECT_NE(nullptr, initialBuffer);

    cmdContainer->allocateNextCommandBuffer();

    auto nextBuffer = stream->getSpace(0);
    auto sizeUsed = stream->getUsed();
    auto availableSize = stream->getMaxAvailableSpace();

    EXPECT_NE(nullptr, nextBuffer);
    EXPECT_EQ(0u, sizeUsed);
    EXPECT_NE(initialBuffer, nextBuffer);
    const size_t cmdBufSize = CommandContainer::defaultListCmdBufferSize;
    EXPECT_EQ(cmdBufSize, availableSize);

    ASSERT_EQ(2u, cmdContainer->getCmdBufferAllocations().size());
    EXPECT_EQ(cmdContainer->getCmdBufferAllocations()[1], cmdContainer->getCommandStream()->getGraphicsAllocation());

    EXPECT_EQ(cmdContainer->getCmdBufferAllocations()[1], cmdContainer->getResidencyContainer().back());
}

TEST_F(CommandContainerTest, whenResettingCommandContainerThenStoredCmdBuffersAreFreedAndStreamIsReplacedWithInitialBuffer) {
    std::unique_ptr<CommandContainer> cmdContainer(new CommandContainer);
    cmdContainer->initialize(pDevice);

    cmdContainer->allocateNextCommandBuffer();
    cmdContainer->allocateNextCommandBuffer();

    EXPECT_EQ(3u, cmdContainer->getCmdBufferAllocations().size());

    cmdContainer->reset();

    ASSERT_EQ(1u, cmdContainer->getCmdBufferAllocations().size());

    auto stream = cmdContainer->getCommandStream();
    ASSERT_NE(nullptr, stream);

    auto buffer = stream->getSpace(0);
    const size_t cmdBufSize = CommandContainer::defaultListCmdBufferSize;

    EXPECT_EQ(cmdContainer->getCmdBufferAllocations()[0]->getUnderlyingBuffer(), buffer);
    EXPECT_EQ(cmdBufSize, stream->getMaxAvailableSpace());
}
