/*
 * Copyright (C) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

namespace OCLRT {
extern bool isGTPinInitialized;

void gtpinNotifyContextCreate(cl_context context);
void gtpinNotifyContextDestroy(cl_context context);
void gtpinNotifyKernelCreate(cl_kernel kernel);
void gtpinNotifyKernelSubmit(cl_kernel kernel, void *pCmdQueue);
void gtpinNotifyPreFlushTask(void *pCmdQueue);
void gtpinNotifyFlushTask(uint32_t flushedTaskCount);
void gtpinNotifyTaskCompletion(uint32_t completedTaskCount);
void gtpinNotifyMakeResident(void *pKernel, void *pCommandStreamReceiver);
void gtpinNotifyUpdateResidencyList(void *pKernel, void *pResidencyVector);
void gtpinNotifyPlatformShutdown();
inline bool gtpinIsGTPinInitialized() { return isGTPinInitialized; }
void *gtpinGetIgcInit();
} // namespace OCLRT
