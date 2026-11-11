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
#include "score/language/safecpp/coverage_termination_handler/coverage_termination_handler.h"

#include <score/utility.hpp>

#include <csignal>
#include <cstdlib>
#include <exception>

#if defined(__GNUC__) && not defined(__clang__)  // since gcov.h is only available in GCC

extern "C" {
#include <gcov.h>
}

namespace
{

void terminate_handler()
{
    // first, reset any potentially still registered abort handler
    score::cpp::ignore = std::signal(SIGABRT, SIG_DFL);

    // Even though it seems this is a private function of the STL (leading __), according to
    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=80435
    // https://bugs.llvm.org/show_bug.cgi?id=32555
    // it is allowed to invoke this function by an end-user for exactly our case.
    //
    // There is a significant change of API with GCC 11, where __gcov_flush() was removed in favor of __gcov_dump().
    // To make things worse, __gcov_dump() prior to GCC 11 assumes that it is called within a synchronized block
    // using __gcov_lock() and __gcov_unlock(). __gcov_flush() was nothing else than a call to __gcov_dump() and
    // __gcov_reset() within a synchronized block, using __gcov_lock() and __gcov_unlock(). After GCC 11, __gcov_dump()
    // will synchronize by its own.
    //
    // What does this mean for this termination handler:
    // Because C++ does not promise that the termination handler is called in a serialized fashion, we need to
    // ensure that the following GCOV call is correctly synchronized with any other calls to GCOV. For this we must use
    // the locking mechanism provided by GCOV. But __gcov_lock() and __gcov_unlock() are not exposed. Thus, we must use
    // __gcov_flush() or __gcov_dump() depending on which GCC version we use.
#if __GNUC__ >= 11
    __gcov_dump();
#else
    __gcov_flush();
#endif

    // NOLINTNEXTLINE(score-banned-function): This code will only be used for unit-testing.
    std::abort();
}

}  // namespace

extern "C" void signal_handler(int /*signal*/)
{
    terminate_handler();
}

namespace
{

// We create an object that will be constructed during start-up and upon its construction,
// the respective terminate as well as abort handlers will be set
class CoverageTerminateAndAbortHandlerSetter
{
  public:
    CoverageTerminateAndAbortHandlerSetter() noexcept
    {
        score::cpp::ignore = std::set_terminate(terminate_handler);
        score::cpp::ignore = std::signal(SIGABRT, signal_handler);
    }
};

const CoverageTerminateAndAbortHandlerSetter coverage_terminate_and_abort_handler_setter{};

}  // namespace

#endif  // #if defined(__GNUC__) && not defined(__clang__)
