/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
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
#ifndef SCORE_LIB_UTILS_MEYER_SINGLETON_TEST_SINGLE_TEST_PER_PROCESS_FIXTURE_H
#define SCORE_LIB_UTILS_MEYER_SINGLETON_TEST_SINGLE_TEST_PER_PROCESS_FIXTURE_H

#include <gtest/gtest.h>

#include <functional>

namespace score::singleton::test
{

// This fixture forces every test to run within its own process.
// This is required to support resets of a Meyer-Singleton.
// This fixture should be used sparingly, since it has quite a big runtime penalty.
// Every test must be executed within TestInSeparateProcess()!
class SingleTestPerProcessFixture : public ::testing::Test
{
  public:
    void TearDown() override
    {
        // Safeguard against accidentially not using TestInSeparateProcess()
        ASSERT_TRUE(tested_in_separate_process_);
    }

    template <class TestFunction>
    void TestInSeparateProcess(TestFunction test_function)
    {
        const auto exit_test = [&test_function]() {
            std::invoke(std::forward<TestFunction>(test_function));

            PrintTestPartResults();

            if (HasFailure())
            {
                std::exit(1);
            }
            std::exit(0);
        };

        EXPECT_EXIT(exit_test(), ::testing::ExitedWithCode(0), ".*");

        tested_in_separate_process_ = true;
    }

  protected:
    bool tested_in_separate_process_{false};

  private:
    static void PrintTestPartResults()
    {
        auto* const instance = testing::UnitTest::GetInstance();
        auto* const test_result = instance->current_test_info()->result();
        auto* const listener = instance->listeners().default_result_printer();
        for (auto i = 0; i < test_result->total_part_count(); ++i)
        {
            listener->OnTestPartResult(test_result->GetTestPartResult(i));
        }
    }
};

}  // namespace score::singleton::test

#endif  // SCORE_LIB_UTILS_MEYER_SINGLETON_TEST_SINGLE_TEST_PER_PROCESS_FIXTURE_H
