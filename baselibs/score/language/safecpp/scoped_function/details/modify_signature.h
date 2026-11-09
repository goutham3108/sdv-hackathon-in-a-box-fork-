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
#ifndef SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_MODIFY_SIGNATURE_H
#define SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_MODIFY_SIGNATURE_H

#include "score/language/safecpp/scoped_function/details/modify_return_type.h"

#include <score/optional.hpp>

namespace score::safecpp::details
{

template <class T>
struct ModifySignature;

template <class ReturnType, class... Args>
struct ModifySignature<ReturnType(Args...)>
{
    using Type = score::cpp::optional<ModifyReturnTypeT<ReturnType>>(Args...);
};

template <class ReturnType, class... Args>
struct ModifySignature<ReturnType(Args...) const>
{
    using Type = score::cpp::optional<ModifyReturnTypeT<ReturnType>>(Args...) const;
};

template <class ReturnType, class... Args>
struct ModifySignature<ReturnType(Args...) noexcept>
{
    using Type = score::cpp::optional<ModifyReturnTypeT<ReturnType>>(Args...) noexcept;
};

template <class ReturnType, class... Args>
struct ModifySignature<ReturnType(Args...) const noexcept>
{
    using Type = score::cpp::optional<ModifyReturnTypeT<ReturnType>>(Args...) const noexcept;
};

template <class T>
using ModifySignatureT = typename ModifySignature<T>::Type;

}  // namespace score::safecpp::details

#endif  // SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_DETAILS_MODIFY_SIGNATURE_H
