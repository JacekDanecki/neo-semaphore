#
# Copyright (C) 2018-2019 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

set(MAX_GEN 64)

set(ALL_GEN_TYPES "")

list(APPEND ALL_GEN_TYPES "GEN8")
list(APPEND ALL_GEN_TYPES "GEN9")
list(APPEND ALL_GEN_TYPES "GEN10")
list(APPEND ALL_GEN_TYPES "GEN11")

set(ALL_GEN_TYPES_REVERSED ${ALL_GEN_TYPES})
list(REVERSE ALL_GEN_TYPES_REVERSED)

macro(FIND_IDX_FOR_GEN_TYPE GEN_TYPE GEN_IDX)
  list(FIND ALL_GEN_TYPES "${GEN_TYPE}" GEN_IDX)
  if(${GEN_IDX} EQUAL -1)
    message(FATAL_ERROR "No ${GEN_TYPE} allowed, exiting")
  endif()
endmacro()

macro(INIT_LIST LIST_TYPE ELEMENT_TYPE)
  foreach(IT RANGE 0 ${MAX_GEN} 1)
    list(APPEND ALL_${ELEMENT_TYPE}_${LIST_TYPE} " ")
  endforeach()
endmacro()

macro(GET_LIST_FOR_GEN LIST_TYPE ELEMENT_TYPE GEN_IDX OUT_LIST)
  list(GET ALL_${ELEMENT_TYPE}_${LIST_TYPE} ${GEN_IDX} GEN_X_${LIST_TYPE})
  string(REPLACE "_" ";" ${OUT_LIST} ${GEN_X_${LIST_TYPE}})
endmacro()

macro(ADD_ITEM_FOR_GEN LIST_TYPE ELEMENT_TYPE GEN_TYPE ITEM)
  FIND_IDX_FOR_GEN_TYPE(${GEN_TYPE} GEN_IDX)
  list(GET ALL_${ELEMENT_TYPE}_${LIST_TYPE} ${GEN_IDX} GEN_X_LIST)
  string(REPLACE " " "" GEN_X_LIST ${GEN_X_LIST})
  if("${GEN_X_LIST}" STREQUAL "")
    set(GEN_X_LIST "${ITEM}")
  else()
    set(GEN_X_LIST "${GEN_X_LIST}_${ITEM}")
  endif()
  list(REMOVE_AT ALL_${ELEMENT_TYPE}_${LIST_TYPE} ${GEN_IDX})
  list(INSERT ALL_${ELEMENT_TYPE}_${LIST_TYPE} ${GEN_IDX} ${GEN_X_LIST})
endmacro()

macro(GEN_CONTAINS_PLATFORMS TYPE GEN_TYPE OUT_FLAG)
  FIND_IDX_FOR_GEN_TYPE(${GEN_TYPE} GEN_IDX)
  GET_LIST_FOR_GEN("PLATFORMS" ${TYPE} ${GEN_IDX} GEN_X_PLATFORMS)
  string(REPLACE " " "" GEN_X_PLATFORMS ${GEN_X_PLATFORMS})
  if("${GEN_X_PLATFORMS}" STREQUAL "")
    set(${OUT_FLAG} FALSE)
  else()
    set(${OUT_FLAG} TRUE)
  endif()
endmacro()

macro(GET_AVAILABLE_PLATFORMS TYPE FLAG_NAME OUT_STR)
  set(${TYPE}_PLATFORM_LIST)
  set(${TYPE}_GEN_FLAGS_DEFINITONS)
  if(NOT DEFAULT_${TYPE}_PLATFORM AND DEFINED PREFERRED_PLATFORM AND ${FLAG_NAME}_${PREFERRED_PLATFORM})
    set(DEFAULT_${TYPE}_PLATFORM ${PREFERRED_PLATFORM})
  endif()
  foreach(GEN_TYPE ${ALL_GEN_TYPES_REVERSED})
    GEN_CONTAINS_PLATFORMS(${TYPE} ${GEN_TYPE} GENX_HAS_PLATFORMS)
    if(${GENX_HAS_PLATFORMS})
      FIND_IDX_FOR_GEN_TYPE(${GEN_TYPE} GEN_IDX)
      list(APPEND ${TYPE}_GEN_FLAGS_DEFINITONS ${FLAG_NAME}_${GEN_TYPE})
      GET_LIST_FOR_GEN("PLATFORMS" ${TYPE} ${GEN_IDX} ${TYPE}_GENX_PLATFORMS)
      list(APPEND ${TYPE}_PLATFORM_LIST ${${TYPE}_GENX_PLATFORMS})
      if(NOT DEFAULT_${TYPE}_PLATFORM)
        list(GET ${TYPE}_PLATFORM_LIST 0 DEFAULT_${TYPE}_PLATFORM ${PLATFORM_IT})
      endif()
      if(NOT DEFAULT_${TYPE}_${GEN_TYPE}_PLATFORM)
        list(GET ${TYPE}_GENX_PLATFORMS 0 DEFAULT_${TYPE}_${GEN_TYPE}_PLATFORM)
      endif()
    endif()
  endforeach()
  foreach(PLATFORM_IT ${${TYPE}_PLATFORM_LIST})
    set(${OUT_STR} "${${OUT_STR}} ${PLATFORM_IT}")
    list(APPEND ${TYPE}_GEN_FLAGS_DEFINITONS ${FLAG_NAME}_${PLATFORM_IT})
  endforeach()
endmacro()

macro(GET_PLATFORMS_FOR_GEN TYPE GEN_TYPE OUT_LIST)
  FIND_IDX_FOR_GEN_TYPE(${GEN_TYPE} GEN_IDX)
  GET_LIST_FOR_GEN("PLATFORMS" ${TYPE} ${GEN_IDX} ${OUT_LIST})
endmacro()

macro(GET_TEST_CONFIGURATIONS_FOR_PLATFORM TYPE GEN_TYPE PLATFORM OUT_LIST)
  FIND_IDX_FOR_GEN_TYPE(${GEN_TYPE} GEN_IDX)
  set(${OUT_LIST})
  string(TOLOWER ${PLATFORM} PLATFORM_LOWER)
  GET_LIST_FOR_GEN("CONFIGURATIONS" ${TYPE} ${GEN_IDX} ALL_CONFIGURATIONS_FOR_GEN)
  foreach(CONFIGURATION ${ALL_CONFIGURATIONS_FOR_GEN})
    string(REPLACE "/" ";" CONFIGURATION_PARAMS ${CONFIGURATION})
    list(GET CONFIGURATION_PARAMS 0 CONFIGURATION_PLATFORM)
    if(${CONFIGURATION_PLATFORM} STREQUAL ${PLATFORM_LOWER})
      list(APPEND ${OUT_LIST} ${CONFIGURATION})
    endif()
  endforeach()
endmacro()

macro(PLATFORM_HAS_2_0 GEN_TYPE PLATFORM_NAME OUT_FLAG)
  FIND_IDX_FOR_GEN_TYPE(${GEN_TYPE} GEN_IDX)
  GET_LIST_FOR_GEN("PLATFORMS" "SUPPORTED_2_0" ${GEN_IDX} GEN_X_PLATFORMS)
  list(FIND GEN_X_PLATFORMS ${PLATFORM_NAME} PLATFORM_EXISTS)
  if("${PLATFORM_EXISTS}" LESS 0)
    set(${OUT_FLAG} FALSE)
  else()
    set(${OUT_FLAG} TRUE)
  endif()
endmacro()

macro(PLATFORM_TESTED_WITH_APPVERIFIER GEN_TYPE PLATFORM_NAME OUT_FLAG)
  FIND_IDX_FOR_GEN_TYPE(${GEN_TYPE} GEN_IDX)
  GET_LIST_FOR_GEN("PLATFORMS" "TESTED_APPVERIFIER" ${GEN_IDX} GEN_X_PLATFORMS)
  list(FIND GEN_X_PLATFORMS ${PLATFORM_NAME} PLATFORM_EXISTS)
  if("${PLATFORM_EXISTS}" LESS 0)
    set(${OUT_FLAG} FALSE)
  else()
    set(${OUT_FLAG} TRUE)
  endif()
endmacro()

# default flag for GenX devices support
set(SUPPORT_GEN_DEFAULT TRUE CACHE BOOL "default value for SUPPORT_GENx")
# default flag for platform support
set(SUPPORT_PLATFORM_DEFAULT TRUE CACHE BOOL "default value for support platform")

# Define the hardware configurations we support and test
macro(SET_FLAGS_FOR GEN_TYPE)
  set(SUPPORT_${GEN_TYPE} ${SUPPORT_GEN_DEFAULT} CACHE BOOL "Support ${GEN_TYPE} devices")
  set(TESTS_${GEN_TYPE} ${SUPPORT_${GEN_TYPE}} CACHE BOOL "Build ULTs for ${GEN_TYPE} devices")
  if(NOT SUPPORT_${GEN_TYPE} OR SKIP_UNIT_TESTS)
    set(TESTS_${GEN_TYPE} FALSE)
  endif()
  if(SUPPORT_${GEN_TYPE})
    foreach(${GEN_TYPE}_PLATFORM ${ARGN})
      set(SUPPORT_${${GEN_TYPE}_PLATFORM} ${SUPPORT_PLATFORM_DEFAULT} CACHE BOOL "Support ${${GEN_TYPE}_PLATFORM}")
      if(TESTS_${GEN_TYPE})
        set(TESTS_${${GEN_TYPE}_PLATFORM} ${SUPPORT_${${GEN_TYPE}_PLATFORM}} CACHE BOOL "Build ULTs for ${${GEN_TYPE}_PLATFORM}")
      endif()
      if(NOT SUPPORT_${${GEN_TYPE}_PLATFORM} OR NOT TESTS_${GEN_TYPE} OR SKIP_UNIT_TESTS)
        set(TESTS_${${GEN_TYPE}_PLATFORM} FALSE)
      endif()
    endforeach()
  endif()
endmacro()
macro(ADD_PLATFORM_FOR_GEN LIST_TYPE GEN_TYPE PLATFORM_NAME PLATFORM_TYPE)
  ADD_ITEM_FOR_GEN("PLATFORMS" ${LIST_TYPE} ${GEN_TYPE} ${PLATFORM_NAME})
  set(${GEN_TYPE}_HAS_${PLATFORM_TYPE} TRUE)
  set(${PLATFORM_NAME}_IS_${PLATFORM_TYPE} TRUE)
  if(NOT DEFAULT_${LIST_TYPE}_${GEN_TYPE}_${PLATFORM_TYPE}_PLATFORM)
    string(TOLOWER ${PLATFORM_NAME} DEFAULT_${LIST_TYPE}_${GEN_TYPE}_${PLATFORM_TYPE}_PLATFORM)
  endif()
endmacro()

SET_FLAGS_FOR("GEN8" "BDW")
SET_FLAGS_FOR("GEN9" "SKL" "KBL" "BXT" "GLK" "CFL")
if(SUPPORT_GEN10)
  SET_FLAGS_FOR("GEN10" "CNL")
endif()
SET_FLAGS_FOR("GEN11" "ICLLP" "LKF")

# Init lists
INIT_LIST("FAMILY_NAME" "TESTED")
INIT_LIST("PLATFORMS" "SUPPORTED")
INIT_LIST("PLATFORMS" "SUPPORTED_2_0")
INIT_LIST("PLATFORMS" "TESTED")
INIT_LIST("PLATFORMS" "TESTED_APPVERIFIER")
INIT_LIST("CONFIGURATIONS" "UNIT_TESTS")
INIT_LIST("CONFIGURATIONS" "AUB_TESTS")
INIT_LIST("CONFIGURATIONS" "MT_TESTS")

# Add supported and tested platforms
if(SUPPORT_GEN8)
  if(TESTS_GEN8)
    ADD_ITEM_FOR_GEN("FAMILY_NAME" "TESTED" "GEN8" "BDWFamily")
  endif()
  if(SUPPORT_BDW)
    ADD_PLATFORM_FOR_GEN("SUPPORTED" "GEN8" "BDW" "CORE")
    ADD_PLATFORM_FOR_GEN("SUPPORTED_2_0" "GEN8" "BDW" "CORE")
    if(TESTS_BDW)
      ADD_ITEM_FOR_GEN("PLATFORMS" "TESTED" "GEN8" "BDW")
      ADD_ITEM_FOR_GEN("PLATFORMS" "TESTED_APPVERIFIER" "GEN8" "BDW")
      ADD_ITEM_FOR_GEN("CONFIGURATIONS" "AUB_TESTS" "GEN8" "bdw/1/3/8")
      ADD_ITEM_FOR_GEN("CONFIGURATIONS" "MT_TESTS" "GEN8" "bdw/1/3/8")
      ADD_ITEM_FOR_GEN("CONFIGURATIONS" "UNIT_TESTS" "GEN8" "bdw/1/3/8")
    endif()
  endif()
endif()

if(SUPPORT_GEN9)
  if(TESTS_GEN9)
    ADD_ITEM_FOR_GEN("FAMILY_NAME" "TESTED" "GEN9" "SKLFamily")
  endif()
  if(SUPPORT_SKL)
    ADD_PLATFORM_FOR_GEN("SUPPORTED" "GEN9" "SKL" "CORE")
    ADD_PLATFORM_FOR_GEN("SUPPORTED_2_0" "GEN9" "SKL" "CORE")
    set(PREFERRED_PLATFORM "SKL")
    if(TESTS_SKL)
      set(PREFERRED_FAMILY_NAME "SKLFamily")
      ADD_ITEM_FOR_GEN("PLATFORMS" "TESTED" "GEN9" "SKL")
      ADD_ITEM_FOR_GEN("PLATFORMS" "TESTED_APPVERIFIER" "GEN9" "SKL")
      ADD_ITEM_FOR_GEN("CONFIGURATIONS" "AUB_TESTS" "GEN9" "skl/1/3/8")
      ADD_ITEM_FOR_GEN("CONFIGURATIONS" "MT_TESTS" "GEN9" "skl/1/3/8")
      ADD_ITEM_FOR_GEN("CONFIGURATIONS" "UNIT_TESTS" "GEN9" "skl/1/3/8")
    endif()
  endif()

  if(SUPPORT_KBL)
    ADD_PLATFORM_FOR_GEN("SUPPORTED" "GEN9" "KBL" "CORE")
    ADD_PLATFORM_FOR_GEN("SUPPORTED_2_0" "GEN9" "KBL" "CORE")
    if(TESTS_KBL)
      ADD_ITEM_FOR_GEN("PLATFORMS" "TESTED" "GEN9" "KBL")
      ADD_ITEM_FOR_GEN("CONFIGURATIONS" "UNIT_TESTS" "GEN9" "kbl/1/3/6")
    endif()
  endif()

  if(SUPPORT_GLK)
    ADD_PLATFORM_FOR_GEN("SUPPORTED" "GEN9" "GLK" "LP")
    if(TESTS_GLK)
      ADD_ITEM_FOR_GEN("PLATFORMS" "TESTED" "GEN9" "GLK")
      ADD_ITEM_FOR_GEN("CONFIGURATIONS" "UNIT_TESTS" "GEN9" "glk/1/3/6")
    endif()
  endif()

  if(SUPPORT_CFL)
    ADD_PLATFORM_FOR_GEN("SUPPORTED" "GEN9" "CFL" "CORE")
    ADD_ITEM_FOR_GEN("PLATFORMS" "SUPPORTED_2_0" "GEN9" "CFL")
    if(TESTS_CFL)
      ADD_ITEM_FOR_GEN("PLATFORMS" "TESTED" "GEN9" "CFL")
      ADD_ITEM_FOR_GEN("CONFIGURATIONS" "UNIT_TESTS" "GEN9" "cfl/1/3/6")
    endif()
  endif()

  if(SUPPORT_BXT)
    ADD_PLATFORM_FOR_GEN("SUPPORTED" "GEN9" "BXT" "LP")
    if(TESTS_BXT)
      ADD_ITEM_FOR_GEN("PLATFORMS" "TESTED" "GEN9" "BXT")
      ADD_ITEM_FOR_GEN("CONFIGURATIONS" "AUB_TESTS" "GEN9" "bxt/1/3/6")
      ADD_ITEM_FOR_GEN("CONFIGURATIONS" "UNIT_TESTS" "GEN9" "bxt/1/3/6")
    endif()
  endif()
endif()

if(SUPPORT_GEN10)
  ADD_PLATFORM_FOR_GEN("SUPPORTED" "GEN10" "CNL" "CORE")
  ADD_PLATFORM_FOR_GEN("SUPPORTED_2_0" "GEN10" "CNL" "CORE")
  if(TESTS_GEN10)
    ADD_ITEM_FOR_GEN("FAMILY_NAME" "TESTED" "GEN10" "CNLFamily")
    ADD_ITEM_FOR_GEN("PLATFORMS" "TESTED" "GEN10" "CNL")
    ADD_ITEM_FOR_GEN("PLATFORMS" "TESTED_APPVERIFIER" "GEN10" "CNL")
    ADD_ITEM_FOR_GEN("CONFIGURATIONS" "AUB_TESTS" "GEN10" "cnl/2/5/8")
    ADD_ITEM_FOR_GEN("CONFIGURATIONS" "MT_TESTS" "GEN10" "cnl/2/5/8")
    ADD_ITEM_FOR_GEN("CONFIGURATIONS" "UNIT_TESTS" "GEN10" "cnl/2/5/8")
  endif()
endif()

if(SUPPORT_GEN11)
  if(TESTS_GEN11)
    ADD_ITEM_FOR_GEN("FAMILY_NAME" "TESTED" "GEN11" "ICLFamily")
  endif()

  if(SUPPORT_ICLLP)
    ADD_PLATFORM_FOR_GEN("SUPPORTED" "GEN11" "ICLLP" "LP")
    ADD_PLATFORM_FOR_GEN("SUPPORTED_2_0" "GEN11" "ICLLP" "LP")
    if(TESTS_ICLLP)
      ADD_ITEM_FOR_GEN("PLATFORMS" "TESTED" "GEN11" "ICLLP")
      ADD_ITEM_FOR_GEN("PLATFORMS" "TESTED_APPVERIFIER" "GEN11" "ICLLP")
      ADD_ITEM_FOR_GEN("CONFIGURATIONS" "AUB_TESTS" "GEN11" "icllp/1/8/8")
      ADD_ITEM_FOR_GEN("CONFIGURATIONS" "MT_TESTS" "GEN11" "icllp/1/8/8")
      ADD_ITEM_FOR_GEN("CONFIGURATIONS" "UNIT_TESTS" "GEN11" "icllp/1/8/8")
    endif()
  endif()
  if(SUPPORT_LKF)
    ADD_PLATFORM_FOR_GEN("SUPPORTED" "GEN11" "LKF" "LP")
    if(TESTS_LKF)
      ADD_ITEM_FOR_GEN("PLATFORMS" "TESTED" "GEN11" "LKF")
      ADD_ITEM_FOR_GEN("CONFIGURATIONS" "UNIT_TESTS" "GEN11" "lkf/1/8/8")
    endif()
  endif()

endif()


# Get platform lists, flag definition and set default platforms
GET_AVAILABLE_PLATFORMS("SUPPORTED" "SUPPORT" ALL_AVAILABLE_SUPPORTED_PLATFORMS)
GET_AVAILABLE_PLATFORMS("TESTED" "TESTS" ALL_AVAILABLE_TESTED_PLATFORMS)

message(STATUS "All supported platforms: ${ALL_AVAILABLE_SUPPORTED_PLATFORMS}")
message(STATUS "All tested platforms: ${ALL_AVAILABLE_TESTED_PLATFORMS}")

message(STATUS "Default supported platform: ${DEFAULT_SUPPORTED_PLATFORM}")

list(FIND SUPPORTED_PLATFORM_LIST ${DEFAULT_SUPPORTED_PLATFORM} VALID_DEFAULT_SUPPORTED_PLATFORM)
if(VALID_DEFAULT_SUPPORTED_PLATFORM LESS 0)
  message(FATAL_ERROR "Not a valid supported platform: ${DEFAULT_SUPPORTED_PLATFORM}")
endif()

message(STATUS "Default tested platform: ${DEFAULT_TESTED_PLATFORM}")

if(DEFAULT_TESTED_PLATFORM)
  list(FIND TESTED_PLATFORM_LIST ${DEFAULT_TESTED_PLATFORM} VALID_DEFAULT_TESTED_PLATFORM)
  if(VALID_DEFAULT_TESTED_PLATFORM LESS 0)
    message(FATAL_ERROR "Not a valid tested platform: ${DEFAULT_TESTED_PLATFORM}")
  endif()
endif()

if(NOT DEFAULT_TESTED_FAMILY_NAME)
  if(DEFINED PREFERRED_FAMILY_NAME)
    list(FIND ALL_TESTED_FAMILY_NAME ${PREFERRED_FAMILY_NAME} GEN_IDX)
    if(${GEN_IDX} GREATER -1)
      set(DEFAULT_TESTED_FAMILY_NAME ${PREFERRED_FAMILY_NAME})
    endif()
  endif()
  if(NOT DEFINED DEFAULT_TESTED_FAMILY_NAME)
    foreach(GEN_TYPE ${ALL_GEN_TYPES_REVERSED})
      FIND_IDX_FOR_GEN_TYPE(${GEN_TYPE} GEN_IDX)
      list(GET ALL_TESTED_FAMILY_NAME ${GEN_IDX} GEN_FAMILY_NAME)
      if(NOT GEN_FAMILY_NAME STREQUAL " ")
        set(DEFAULT_TESTED_FAMILY_NAME ${GEN_FAMILY_NAME})
        break()
      endif()
    endforeach()
  endif()
endif()
message(STATUS "Default tested family name: ${DEFAULT_TESTED_FAMILY_NAME}")
