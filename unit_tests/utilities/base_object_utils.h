/*
 * Copyright (C) 2018-2019 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once
#include <memory>

namespace OCLRT {

template <typename T>
struct ReleaseObject {
    void operator()(T *t) {
        if (t != nullptr) {
            t->release();
        }
    }
};

template <typename T>
using ReleaseableObjectPtr = std::unique_ptr<T, ReleaseObject<T>>;

template <typename T>
static ReleaseableObjectPtr<T> clUniquePtr(T *object) {
    return ReleaseableObjectPtr<T>{object};
}
} // namespace OCLRT
