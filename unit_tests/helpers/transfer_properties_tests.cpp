/*
 * Copyright (C) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "runtime/helpers/properties_helper.h"
#include "unit_tests/helpers/debug_manager_state_restore.h"
#include "unit_tests/mocks/mock_buffer.h"
#include "gtest/gtest.h"

using namespace OCLRT;

TEST(TransferPropertiesTest, givenTransferPropertiesCreatedWhenDefaultDebugSettingThenLockPtrIsNotSet) {
    MockBuffer buffer;

    size_t offset = 0;
    size_t size = 4096u;
    TransferProperties transferProperties(&buffer, CL_COMMAND_MAP_BUFFER, 0, false, &offset, &size, nullptr);
    EXPECT_EQ(nullptr, transferProperties.lockedPtr);
}

TEST(TransferPropertiesTest, givenTransferPropertiesCreatedWhenForceResourceLockOnTransferCallsSetThenLockPtrIsSet) {
    DebugManagerStateRestore dbgRestore;
    DebugManager.flags.ForceResourceLockOnTransferCalls.set(true);
    ExecutionEnvironment executionEnvironment;
    OsAgnosticMemoryManager memoryManager(false, true, executionEnvironment);

    MockContext ctx;
    ctx.setMemoryManager(&memoryManager);
    cl_int retVal;
    std::unique_ptr<Buffer> buffer(Buffer::create(&ctx, 0, 1, nullptr, retVal));
    static_cast<MemoryAllocation *>(buffer->getGraphicsAllocation())->overrideMemoryPool(MemoryPool::SystemCpuInaccessible);

    size_t offset = 0;
    size_t size = 4096u;

    TransferProperties transferProperties(buffer.get(), CL_COMMAND_MAP_BUFFER, 0, false, &offset, &size, nullptr);
    EXPECT_NE(nullptr, transferProperties.lockedPtr);
}

TEST(TransferPropertiesTest, givenTransferPropertiesCreatedWhenForceResourceLockOnTransferCallsSetAndMemoryPoolIsSystemMemoryThenLockPtrIsNotSet) {
    DebugManagerStateRestore dbgRestore;
    DebugManager.flags.ForceResourceLockOnTransferCalls.set(true);
    ExecutionEnvironment executionEnvironment;
    OsAgnosticMemoryManager memoryManager(false, true, executionEnvironment);

    MockContext ctx;
    ctx.setMemoryManager(&memoryManager);
    cl_int retVal;
    std::unique_ptr<Buffer> buffer(Buffer::create(&ctx, 0, 1, nullptr, retVal));
    static_cast<MemoryAllocation *>(buffer->getGraphicsAllocation())->overrideMemoryPool(MemoryPool::System4KBPages);

    size_t offset = 0;
    size_t size = 4096u;

    TransferProperties transferProperties(buffer.get(), CL_COMMAND_MAP_BUFFER, 0, false, &offset, &size, nullptr);
    EXPECT_EQ(nullptr, transferProperties.lockedPtr);
}

TEST(TransferPropertiesTest, givenTransferPropertiesCreatedWhenForceResourceLockOnTransferCallsSetAndMemoryManagerInMemObjectIsNotSetThenLockPtrIsNotSet) {
    DebugManagerStateRestore dbgRestore;
    DebugManager.flags.ForceResourceLockOnTransferCalls.set(true);

    MockBuffer buffer;

    size_t offset = 0;
    size_t size = 4096u;
    TransferProperties transferProperties(&buffer, CL_COMMAND_MAP_BUFFER, 0, false, &offset, &size, nullptr);
    EXPECT_EQ(nullptr, transferProperties.lockedPtr);
}

TEST(TransferPropertiesTest, givenTransferPropertiesWhenLockedPtrIsSetThenItIsReturnedForReadWrite) {
    DebugManagerStateRestore dbgRestore;
    DebugManager.flags.ForceResourceLockOnTransferCalls.set(true);
    ExecutionEnvironment executionEnvironment;
    OsAgnosticMemoryManager memoryManager(false, true, executionEnvironment);

    MockContext ctx;
    ctx.setMemoryManager(&memoryManager);
    cl_int retVal;
    std::unique_ptr<Buffer> buffer(Buffer::create(&ctx, 0, 1, nullptr, retVal));
    static_cast<MemoryAllocation *>(buffer->getGraphicsAllocation())->overrideMemoryPool(MemoryPool::SystemCpuInaccessible);

    size_t offset = 0;
    size_t size = 4096u;

    TransferProperties transferProperties(buffer.get(), CL_COMMAND_MAP_BUFFER, 0, false, &offset, &size, nullptr);
    ASSERT_NE(nullptr, transferProperties.lockedPtr);
    EXPECT_EQ(transferProperties.lockedPtr, transferProperties.getCpuPtrForReadWrite());
}

TEST(TransferPropertiesTest, givenTransferPropertiesWhenLockedPtrIsNotSetThenItIsNotReturnedForReadWrite) {
    MockBuffer buffer;

    size_t offset = 0;
    size_t size = 4096u;
    TransferProperties transferProperties(&buffer, CL_COMMAND_MAP_BUFFER, 0, false, &offset, &size, nullptr);
    ASSERT_EQ(nullptr, transferProperties.lockedPtr);
    EXPECT_NE(transferProperties.lockedPtr, transferProperties.getCpuPtrForReadWrite());
}
