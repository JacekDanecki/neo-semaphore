/*
 * Copyright (C) 2017-2019 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once
#include "runtime/os_interface/windows/os_time_win.h"

namespace NEO {
class MockOSTimeWin : public OSTimeWin {
  public:
    MockOSTimeWin(Wddm *inWddm) {
        wddm = inWddm;
    }

    double getDynamicDeviceTimerResolution(HardwareInfo const &hwInfo) const override {
        return OSTimeWin::getDynamicDeviceTimerResolution(hwInfo);
    };
};
} // namespace NEO
