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
#include "score/result/error.h"

bool score::result::operator==(const score::result::Error& lhs, const score::result::Error& rhs) noexcept
{
    return (lhs.code_ == rhs.code_) && (lhs.domain_ == rhs.domain_);
}

bool score::result::operator!=(const score::result::Error& lhs, const score::result::Error& rhs) noexcept
{
    return (lhs.code_ != rhs.code_) || (lhs.domain_ != rhs.domain_);
}

std::ostream& score::result::operator<<(std::ostream& out, const score::result::Error& value) noexcept
{
    out << "Error ";
    out << value.Message();
    out << " occurred";
    if (!value.UserMessage().empty())
    {
        out << " with message ";
        out << value.UserMessage();
    }
    return out;
}
