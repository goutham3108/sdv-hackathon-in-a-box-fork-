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
#include "score/mw/com/impl/methods/proxy_method.h"

namespace score::mw::com::impl::detail
{

score::Result<std::size_t> DetermineNextAvailableQueueSlot(containers::DynamicArray<bool>& return_type_ptr_flags)
{
    for (std::size_t i = 0U; i < return_type_ptr_flags.size(); ++i)
    {
        if (!return_type_ptr_flags[i])
        {
            return i;
        }
    }
    return score::MakeUnexpected(ComErrc::kCallQueueFull);
}

}  // namespace score::mw::com::impl::detail
