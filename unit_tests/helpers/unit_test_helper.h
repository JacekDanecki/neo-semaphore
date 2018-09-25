/*
 * Copyright (C) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

namespace OCLRT {

class Kernel;

template <typename GfxFamily>
struct UnitTestHelper {
    static bool isL3ConfigProgrammable();

    static bool evaluateDshUsage(size_t sizeBeforeEnqueue, size_t sizeAfterEnqueue, Kernel *kernel);
};
} // namespace OCLRT
