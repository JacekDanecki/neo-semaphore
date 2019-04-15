/*
 * Copyright (C) 2018-2019 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "runtime/source_level_debugger/source_level_debugger.h"

#include "unit_tests/libult/source_level_debugger_library.h"

namespace NEO {
OsLibrary *SourceLevelDebugger::loadDebugger() {
    return DebuggerLibrary::load(SourceLevelDebugger::dllName);
}

} // namespace NEO
