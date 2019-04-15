/*
 * Copyright (C) 2017-2019 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "cl_api_tests.h"

using namespace NEO;

typedef api_tests clGetExtensionFunctionAddressForPlatformTests;

namespace ULT {

TEST_F(clGetExtensionFunctionAddressForPlatformTests, GivenNullPlatformWhenGettingExtensionFunctionThenNullIsReturned) {
    auto retVal = clGetExtensionFunctionAddressForPlatform(nullptr, "clCreateAcceleratorINTEL");
    EXPECT_EQ(retVal, nullptr);
}

TEST_F(clGetExtensionFunctionAddressForPlatformTests, GivenNonExistentExtensionWhenGettingExtensionFunctionThenNullIsReturned) {
    auto retVal = clGetExtensionFunctionAddressForPlatform(pPlatform, "__some__function__");
    EXPECT_EQ(retVal, nullptr);
}

TEST_F(clGetExtensionFunctionAddressForPlatformTests, GivenClCreateAcceleratorINTELWhenGettingExtensionFunctionThenCorrectAddressIsReturned) {
    auto retVal = clGetExtensionFunctionAddressForPlatform(pPlatform, "clCreateAcceleratorINTEL");
    EXPECT_EQ(retVal, reinterpret_cast<void *>(clCreateAcceleratorINTEL));
}

TEST_F(clGetExtensionFunctionAddressForPlatformTests, GivenClGetAcceleratorInfoINTELWhenGettingExtensionFunctionThenCorrectAddressIsReturned) {
    auto retVal = clGetExtensionFunctionAddressForPlatform(pPlatform, "clGetAcceleratorInfoINTEL");
    EXPECT_EQ(retVal, reinterpret_cast<void *>(clGetAcceleratorInfoINTEL));
}

TEST_F(clGetExtensionFunctionAddressForPlatformTests, GivenClRetainAcceleratorINTELWhenGettingExtensionFunctionThenCorrectAddressIsReturned) {
    auto retVal = clGetExtensionFunctionAddressForPlatform(pPlatform, "clRetainAcceleratorINTEL");
    EXPECT_EQ(retVal, reinterpret_cast<void *>(clRetainAcceleratorINTEL));
}

TEST_F(clGetExtensionFunctionAddressForPlatformTests, GivenClReleaseAcceleratorINTELWhenGettingExtensionFunctionThenCorrectAddressIsReturned) {
    auto retVal = clGetExtensionFunctionAddressForPlatform(pPlatform, "clReleaseAcceleratorINTEL");
    EXPECT_EQ(retVal, reinterpret_cast<void *>(clReleaseAcceleratorINTEL));
}

TEST_F(clGetExtensionFunctionAddressForPlatformTests, GivenClCreateProgramWithILKHRWhenGettingExtensionFunctionThenCorrectAddressIsReturned) {
    auto retVal = clGetExtensionFunctionAddressForPlatform(pPlatform, "clCreateProgramWithILKHR");
    EXPECT_EQ(retVal, reinterpret_cast<void *>(clCreateProgramWithILKHR));
}
} // namespace ULT
