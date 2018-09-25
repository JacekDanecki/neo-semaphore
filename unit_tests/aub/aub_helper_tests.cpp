/*
 * Copyright (C) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "gtest/gtest.h"
#include "runtime/aub/aub_helper.h"
#include "runtime/aub_mem_dump/aub_mem_dump.h"
#include "runtime/aub_mem_dump/page_table_entry_bits.h"
#include "unit_tests/fixtures/device_fixture.h"
#include "test.h"

using namespace OCLRT;

TEST(AubHelper, WhenGetMemTraceIsCalledWithZeroPDEntryBitsThenTraceNonLocalIsReturned) {
    int hint = AubHelper::getMemTrace(0u);
    EXPECT_EQ(AubMemDump::AddressSpaceValues::TraceNonlocal, hint);
}

TEST(AubHelper, WhenGetPTEntryBitsIsCalledThenEntryBitsAreNotMasked) {
    uint64_t entryBits = BIT(PageTableEntry::presentBit) |
                         BIT(PageTableEntry::writableBit) |
                         BIT(PageTableEntry::userSupervisorBit);
    uint64_t maskedEntryBits = AubHelper::getPTEntryBits(entryBits);
    EXPECT_EQ(entryBits, maskedEntryBits);
}

typedef Test<DeviceFixture> AubHelperHwTest;

HWTEST_F(AubHelperHwTest, GivenDisabledLocalMemoryWhenGetDataHintForPml4EntryIsCalledThenTraceNotypeIsReturned) {
    AubHelperHw<FamilyType> aubHelper(false);
    int dataHint = aubHelper.getDataHintForPml4Entry();
    EXPECT_EQ(AubMemDump::DataTypeHintValues::TraceNotype, dataHint);
}

HWTEST_F(AubHelperHwTest, GivenDisabledLocalMemoryWhenGetDataHintForPdpEntryIsCalledThenTraceNotypeIsReturned) {
    AubHelperHw<FamilyType> aubHelper(false);
    int dataHint = aubHelper.getDataHintForPdpEntry();
    EXPECT_EQ(AubMemDump::DataTypeHintValues::TraceNotype, dataHint);
}

HWTEST_F(AubHelperHwTest, GivenDisabledLocalMemoryWhenGetDataHintForPdEntryIsCalledThenTraceNotypeIsReturned) {
    AubHelperHw<FamilyType> aubHelper(false);
    int dataHint = aubHelper.getDataHintForPdEntry();
    EXPECT_EQ(AubMemDump::DataTypeHintValues::TraceNotype, dataHint);
}

HWTEST_F(AubHelperHwTest, GivenDisabledLocalMemoryWhenGetDataHintForPtEntryIsCalledThenTraceNotypeIsReturned) {
    AubHelperHw<FamilyType> aubHelper(false);
    int dataHint = aubHelper.getDataHintForPtEntry();
    EXPECT_EQ(AubMemDump::DataTypeHintValues::TraceNotype, dataHint);
}

HWTEST_F(AubHelperHwTest, GivenDisabledLocalMemoryWhenGetMemTraceForPml4EntryIsCalledThenTracePml4EntryIsReturned) {
    AubHelperHw<FamilyType> aubHelper(false);
    int addressSpace = aubHelper.getMemTraceForPml4Entry();
    EXPECT_EQ(AubMemDump::AddressSpaceValues::TracePml4Entry, addressSpace);
}

HWTEST_F(AubHelperHwTest, GivenDisabledLocalMemoryWhenGetMemTraceForPdpEntryIsCalledThenTracePhysicalPdpEntryIsReturned) {
    AubHelperHw<FamilyType> aubHelper(false);
    int addressSpace = aubHelper.getMemTraceForPdpEntry();
    EXPECT_EQ(AubMemDump::AddressSpaceValues::TracePhysicalPdpEntry, addressSpace);
}

HWTEST_F(AubHelperHwTest, GivenDisabledLocalMemoryWhenGetMemTraceForPd4EntryIsCalledThenTracePpgttPdEntryIsReturned) {
    AubHelperHw<FamilyType> aubHelper(false);
    int addressSpace = aubHelper.getMemTraceForPdEntry();
    EXPECT_EQ(AubMemDump::AddressSpaceValues::TracePpgttPdEntry, addressSpace);
}

HWTEST_F(AubHelperHwTest, GivenDisabledLocalMemoryWhenGetMemTraceForPtEntryIsCalledThenTracePpgttEntryIsReturned) {
    AubHelperHw<FamilyType> aubHelper(false);
    int addressSpace = aubHelper.getMemTraceForPtEntry();
    EXPECT_EQ(AubMemDump::AddressSpaceValues::TracePpgttEntry, addressSpace);
}

HWTEST_F(AubHelperHwTest, GivenEnabledLocalMemoryWhenGetMemTraceForPml4EntryIsCalledThenTraceNonlocalIsReturned) {
    AubHelperHw<FamilyType> aubHelper(true);
    int addressSpace = aubHelper.getMemTraceForPml4Entry();
    EXPECT_EQ(AubMemDump::AddressSpaceValues::TraceLocal, addressSpace);
}

HWTEST_F(AubHelperHwTest, GivenEnabledLocalMemoryWhenGetMemTraceForPdpEntryIsCalledThenTraceNonlocalIsReturned) {
    AubHelperHw<FamilyType> aubHelper(true);
    int addressSpace = aubHelper.getMemTraceForPdpEntry();
    EXPECT_EQ(AubMemDump::AddressSpaceValues::TraceLocal, addressSpace);
}

HWTEST_F(AubHelperHwTest, GivenEnabledLocalMemoryWhenGetMemTraceForPd4EntryIsCalledThenTraceNonlocalIsReturned) {
    AubHelperHw<FamilyType> aubHelper(true);
    int addressSpace = aubHelper.getMemTraceForPdEntry();
    EXPECT_EQ(AubMemDump::AddressSpaceValues::TraceLocal, addressSpace);
}

HWTEST_F(AubHelperHwTest, GivenEnabledLocalMemoryWhenGetMemTraceForPtEntryIsCalledThenTraceNonlocalIsReturned) {
    AubHelperHw<FamilyType> aubHelper(true);
    int addressSpace = aubHelper.getMemTraceForPtEntry();
    EXPECT_EQ(AubMemDump::AddressSpaceValues::TraceLocal, addressSpace);
}
