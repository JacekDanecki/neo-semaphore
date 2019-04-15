/*
 * Copyright (C) 2019 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "runtime/os_interface/linux/drm_neo.h"

#include "drm/i915_drm.h"

class DrmWrap : public NEO::Drm {
  public:
    static NEO::Drm *createDrm(int32_t deviceOrdinal) {
        return NEO::Drm::create(deviceOrdinal);
    }
    static void closeDevice(int32_t deviceOrdinal) {
        NEO::Drm::closeDevice(deviceOrdinal);
    };
};
