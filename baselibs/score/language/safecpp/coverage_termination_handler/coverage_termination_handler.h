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
#ifndef SCORE_LANGUAGE_SAFECPP_COVERAGE_TERMINATION_HANDLER_COVERAGE_TERMINATION_HANDLER_H
#define SCORE_LANGUAGE_SAFECPP_COVERAGE_TERMINATION_HANDLER_COVERAGE_TERMINATION_HANDLER_H

/// \file
/// \brief This library has no external linkage. Its sole purpose is to set a termination handler that will flush
/// buffers that are used by gcov.
///
/// \details When using GoogleTest DeathTests ala EXPECT_DEATH, it can happen that certain branches are covered by your
/// test, but seem to be uncovered in the coverage report. This can happen, because gcov (the coverage measurement tool)
/// holds internal buffers which are not flushed. The abnormal process termination causes a direct abort and thus, the
/// content of these buffers will not be written into the gcov files.
///
/// This library provides a solution for that problem. If we use `std::terminate()` for abnormal process ending - which
/// should be used anyhow according to MISRA - a termination handler will be invoked. Within the implementation of this
/// library, we will register such a termination handler which ensures that all the gcov buffers will be flushed prior
/// to termination.

#endif  // SCORE_LANGUAGE_SAFECPP_COVERAGE_TERMINATION_HANDLER_COVERAGE_TERMINATION_HANDLER_H
