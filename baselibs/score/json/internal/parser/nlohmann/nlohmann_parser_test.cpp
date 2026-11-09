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
#include "gtest/gtest.h"
#ifdef NLOHMANN
#include "score/json/internal/parser/nlohmann/nlohmann_parser.h"
#include "score/json/internal/parser/parsers_test_suite.h"

namespace score
{
namespace json
{
namespace
{
INSTANTIATE_TYPED_TEST_SUITE_P(Test, ParserTest, NlohmannParser, /*unused*/);
}  // namespace
}  // namespace json
}  // namespace score
#else
GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(ParserTest);
#endif
