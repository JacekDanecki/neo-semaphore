/*
 * Copyright (C) 2019 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "runtime/helpers/hw_helper.h"
#include "test.h"

using namespace NEO;

template <typename FamilyType>
void whenGetGpgpuEnginesThenReturnTwoRcsEngines() {
    auto gpgpuEngines = HwHelperHw<FamilyType>::get().getGpgpuEngineInstances();
    EXPECT_EQ(2u, gpgpuEngines.size());
    EXPECT_EQ(aub_stream::ENGINE_RCS, gpgpuEngines[0]);
    EXPECT_EQ(aub_stream::ENGINE_RCS, gpgpuEngines[1]);
}
