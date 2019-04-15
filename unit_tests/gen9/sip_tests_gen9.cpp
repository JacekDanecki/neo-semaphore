/*
 * Copyright (C) 2017-2019 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "runtime/built_ins/built_ins.h"
#include "runtime/built_ins/sip.h"
#include "test.h"
#include "unit_tests/global_environment.h"
#include "unit_tests/helpers/test_files.h"
#include "unit_tests/mocks/mock_device.h"

#include "gtest/gtest.h"

using namespace NEO;

namespace SipKernelTests {
extern std::string getDebugSipKernelNameWithBitnessAndProductSuffix(std::string &base, const char *product);

typedef ::testing::Test gen9SipTests;

GEN9TEST_F(gen9SipTests, DISABLED_givenDebugCsrSipKernelWithLocalMemoryWhenAskedForDebugSurfaceBtiAndSizeThenBtiIsZeroAndSizeGreaterThanZero) {
    auto mockDevice = std::unique_ptr<MockDevice>(MockDevice::createWithNewExecutionEnvironment<MockDevice>(nullptr));
    EXPECT_NE(nullptr, mockDevice);
    MockCompilerDebugVars igcDebugVars;

    std::string name = "sip_dummy_kernel_debug";
    std::string builtInFileRoot = testFiles + getDebugSipKernelNameWithBitnessAndProductSuffix(name, binaryNameSuffix.c_str());
    std::string builtInGenFile = builtInFileRoot;
    builtInGenFile.append(".gen");

    igcDebugVars.fileName = builtInGenFile;
    gEnvironment->igcPushDebugVars(igcDebugVars);

    auto &builtins = *mockDevice->getExecutionEnvironment()->getBuiltIns();
    auto &sipKernel = builtins.getSipKernel(SipKernelType::DbgCsrLocal, *mockDevice);

    EXPECT_NE(nullptr, &sipKernel);
    EXPECT_EQ(SipKernelType::DbgCsrLocal, sipKernel.getType());

    gEnvironment->igcPopDebugVars();
}

GEN9TEST_F(gen9SipTests, givenDebuggingActiveWhenSipTypeIsQueriedThenDbgCsrLocalIsReturned) {
    auto sipType = SipKernel::getSipKernelType(renderCoreFamily, true);
    EXPECT_EQ(SipKernelType::DbgCsrLocal, sipType);
}
} // namespace SipKernelTests
