/*
 * Copyright (C) 2019 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once
#include "runtime/os_interface/os_context.h"

namespace NEO {
class MockOsContext : public OsContext {
  public:
    MockOsContext(uint32_t contextId, DeviceBitfield deviceBitfield,
                  aub_stream::EngineType engineType, PreemptionMode preemptionMode, bool lowPriority)
        : OsContext(contextId, deviceBitfield, engineType, preemptionMode, lowPriority) {}
};
} // namespace NEO
