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

#ifndef SCORE_LIB_STRING_MANIPULATION_ARGUMENTS_ARGUMENTS_H
#define SCORE_LIB_STRING_MANIPULATION_ARGUMENTS_ARGUMENTS_H

#include "score/language/safecpp/string_view/zstring_view.h"
#include <vector>

namespace score::string_manipulation
{
std::vector<safecpp::zstring_view> GetArguments(int argc, const char* argv[]);
}

#endif  // SCORE_LIB_STRING_MANIPULATION_ARGUMENTS_ARGUMENTS_H
