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
#ifndef SCORE_LIB_OS_QNX_QNX_VERSION_H
#define SCORE_LIB_OS_QNX_QNX_VERSION_H

#if __QNX__ >= 800
// QNX 8 or later
#define _SPP_QNX_VERSION_ __QNX__
#else
#if __has_include(<sys/nto_version.h>)
// QNX 7.x or earlier
#include <sys/nto_version.h>
#define _SPP_QNX_VERSION_ _NTO_VERSION
#else
#define _SPP_QNX_VERSION_ 0u
#endif
#endif

#endif  // SCORE_LIB_OS_QNX_QNX_VERSION_H
