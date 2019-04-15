/*
 * Copyright (C) 2019 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "runtime/execution_environment/execution_environment.h"
#include "runtime/memory_manager/os_agnostic_memory_manager.h"
#include "runtime/os_interface/os_interface.h"
#include "runtime/os_interface/windows/os_interface.h"
#include "runtime/os_interface/windows/wddm_memory_manager.h"
#include "unit_tests/libult/create_command_stream.h"

namespace NEO {
bool overrideMemoryManagerCreation = true;

std::unique_ptr<MemoryManager> MemoryManager::createMemoryManager(ExecutionEnvironment &executionEnvironment) {
    if (overrideMemoryManagerCreation) {
        return std::make_unique<OsAgnosticMemoryManager>(executionEnvironment);
    }
    return std::make_unique<WddmMemoryManager>(executionEnvironment);
}
} // namespace NEO
