/*
 * Copyright (C) 2017-2019 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "runtime/os_interface/linux/drm_neo.h"

namespace NEO {

Drm::~Drm() = default;

Drm *Drm::get(int32_t deviceOrdinal) {
    return nullptr;
}

Drm *Drm::create(int32_t deviceOrdinal) {
    return nullptr;
}

void Drm::closeDevice(int32_t deviceOrdinal) {
    return;
}
} // namespace NEO
