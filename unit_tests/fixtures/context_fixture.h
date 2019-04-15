/*
 * Copyright (C) 2017-2019 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once
#include "CL/cl.h"

namespace NEO {
class MockContext;

class ContextFixture {
  public:
    ContextFixture();

  protected:
    virtual void SetUp(cl_uint numDevices, cl_device_id *pDeviceList);
    virtual void TearDown();

    MockContext *pContext;
};
} // namespace NEO
