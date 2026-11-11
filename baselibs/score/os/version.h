/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/
#ifndef SCORE_LIB_OS_VERSION_H
#define SCORE_LIB_OS_VERSION_H

/**
 * @name SPP OS Version Comparison Macros
 * Helper macros for comparing QNX versions
 * @{
 */
#define SPP_OS_VERSION_GTE(version, major, minor) ((version) >= ((major * 100) + minor))
#define SPP_OS_VERSION_LT(version, major, minor) ((version) < ((major * 100) + minor))
/** @} */

#if defined(__QNX__)
#include "score/os/qnx/qnx_version.h"
/**
 * @name SPP OS QNX Version Macro
 * A unified macro to retrieve the QNX version across different QNX variants,
 * abstracting version specific retrieval methods.
 * @default 0
 * @{
 */
#define SPP_OS_QNX_VERSION _SPP_QNX_VERSION_
/** @} */

/**
 * @name QNX Major Version Macros
 * Macros for detecting major QNX versions
 * @{
 */
#if SPP_OS_VERSION_GTE(SPP_OS_QNX_VERSION, 8, 0) && SPP_OS_VERSION_LT(SPP_OS_QNX_VERSION, 9, 0)
#define SPP_OS_QNX8
#endif

#if SPP_OS_VERSION_GTE(SPP_OS_QNX_VERSION, 7, 0) && SPP_OS_VERSION_LT(SPP_OS_QNX_VERSION, 8, 0)
#define SPP_OS_QNX7
#endif
/** @} */

#else
#define SPP_OS_QNX_VERSION 0
#endif  // __QNX__

/**
 * @example
 * // Example usage:
 *
 * void some_function() {
 * #ifdef SPP_OS_QNX8
 *     // QNX 8.x specific code
 * #endif
 * }
 *
 * void some_function() {
 * #if SPP_OS_VERSION_LT(SPP_OS_QNX_VERSION, 8, 0)
 *     // Code specific to QNX 7.x and earlier versions
 * #endif
 * }
 */

#endif  // SCORE_LIB_OS_VERSION_H
