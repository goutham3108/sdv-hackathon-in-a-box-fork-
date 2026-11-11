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
#include "score/language/safecpp/scoped_function/details/scope_state.h"

namespace score
{
namespace safecpp
{
namespace details
{

void ScopeState::Expire() noexcept
{
    // Exclusively lock the mutex to make sure that there is no invocation ongoing.
    std::lock_guard<std::shared_mutex> lock{expiration_mutex_};
    expired_ = true;
}

}  // namespace details
}  // namespace safecpp
}  // namespace score
