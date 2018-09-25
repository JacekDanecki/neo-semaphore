/*
 * Copyright (C) 2017-2018 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "runtime/helpers/completion_stamp.h"
#include "runtime/utilities/reference_tracked_object.h"
#include "runtime/utilities/stackvec.h"

namespace OCLRT {
struct FlushStampTrackingObj : public ReferenceTrackedObject<FlushStampTrackingObj> {
    FlushStamp flushStamp = 0;
    std::atomic<bool> initialized{false};
};

class FlushStampTracker {
  public:
    FlushStampTracker() = delete;
    FlushStampTracker(bool allocateStamp);
    ~FlushStampTracker();

    FlushStamp peekStamp() const;
    void setStamp(FlushStamp stamp);
    void replaceStampObject(FlushStampTrackingObj *stampObj);

    // Temporary. Method will be removed
    FlushStampTrackingObj *getStampReference() {
        return flushStampSharedHandle;
    }

  protected:
    FlushStampTrackingObj *flushStampSharedHandle = nullptr;
};

class FlushStampUpdateHelper {
  public:
    void insert(FlushStampTrackingObj *stampObj);
    void updateAll(FlushStamp &flushStamp);
    size_t size() const;

  private:
    StackVec<FlushStampTrackingObj *, 64> flushStampsToUpdate;
};
} // namespace OCLRT
