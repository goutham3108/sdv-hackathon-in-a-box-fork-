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
#include "score/language/safecpp/scoped_function/details/type_erasure_pointer.h"

score::safecpp::details::TypeErasurePointerDeleter& score::safecpp::details::TypeErasurePointerDeleter::operator=(
    TypeErasurePointerDeleter&& rhs) noexcept
{
    // Intentionally skipping memory resource since it is crucial that each unique pointer keeps the memory resource
    // that was assigned during first construction. Updating the resource with the move assignment, would lead to
    // segmentation faults.
    size_ = rhs.size_;
    align_ = rhs.align_;

    return *this;
}
