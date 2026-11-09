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
#ifndef SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_PARAMETRIZATION_HELPER_H
#define SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_PARAMETRIZATION_HELPER_H

namespace score::safecpp
{

template <template <class, class, template <class> class...> class T>
struct ParametrizationHelper
{
    template <class... Args>
    using Type = T<Args...>;
};

}  // namespace score::safecpp

#endif  // SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_PARAMETRIZATION_HELPER_H
