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
#ifndef SCORE_LIB_OS_LINUX_PTHREAD_H
#define SCORE_LIB_OS_LINUX_PTHREAD_H

#include <cstddef>

namespace score
{
namespace os
{

[[deprecated("Use score::os::set_thread_affinity() instead.")]] bool pthread_setaffinity_np(const std::size_t cpu);

}  // namespace os
}  // namespace score

#endif  // SCORE_LIB_OS_LINUX_PTHREAD_H
