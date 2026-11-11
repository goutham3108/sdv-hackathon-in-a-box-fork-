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
#ifndef SCORE_LANGUAGE_SAFECPP_STRING_VIEW_NULL_TERMINATION_VIOLATION_POLICIES_H
#define SCORE_LANGUAGE_SAFECPP_STRING_VIEW_NULL_TERMINATION_VIOLATION_POLICIES_H

#include <cstdlib>
#include <type_traits>

// NOLINTBEGIN(readability-identifier-naming): STL-style notation is intended here
// in order to facilitate interoperability with other STL-like containters/algorithms

namespace score::safecpp
{

///
/// @brief empty class containing all permitted policies to be applicable upon
///        detection of a violation of the null-termination guarantee
///
class null_termination_violation_policies
{
    template <typename T, typename = void>
    class has_member_value_type : public std::false_type
    {
    };
    template <typename T>
    class has_member_value_type<T, std::void_t<typename T::value_type>> : public std::true_type
    {
    };

  public:
    ///
    /// @brief abort policy
    /// @details simply calls `std::abort`
    ///
    class abort
    {
      public:
        using value_type = void;

        [[noreturn]] void operator()(const char*) const noexcept
        {
            // Calling `std::abort()` here is intended since an intermediate terminate handler could do things that we
            // don't want. In safety-related applications we want to terminate as immediate as possible for reaching
            // the safe state. And that is in this case the abortion of the violating application.
            //
            // coverity[autosar_cpp14_m18_0_3_violation] see above justification
            std::abort();
        }
    };

    ///
    /// @brief set empty policy
    /// @details does nothing and its caller is required to take action accordingly
    ///
    class set_empty
    {
      public:
        using value_type = void;

        constexpr void operator()(const char*) const noexcept
        {
            // do nothing
        }
    };

    ///
    /// @brief throw exception policy
    /// @details throws an exception of type \p ExceptionType; initialized with provided \p reason;
    /// @note when using this policy, carefully read and make sure you adhere to `operator()()`'s note below
    ///
    template <typename ExceptionType>
    class throw_exception
    {
      public:
        using value_type = ExceptionType;

        /// @note This is not the default violation policy and users would have to actively select
        ///       it first in order for it to take effect. Hence, this code is not an issue as is.
        ///       It could become one once developers actually start using it. And in this case,
        ///       each single usage must be justified individually by developers in the scope
        ///       of their specific use case(s) as well as requirement(s)!
        [[noreturn]] void operator()(const char* reason) const
        {
            // NOLINTNEXTLINE(score-no-exceptions) see above justification
            throw ExceptionType{reason};
        }
    };

    ///
    /// @brief helper trait for checking whether type \p T; is one of the provided types \p Policies;
    ///
    template <typename T, typename... Policies>
    [[nodiscard]] constexpr static bool is_one_of() noexcept
    {
        return std::disjunction_v<std::is_same<T, Policies>...>;
    }

    ///
    /// @brief helper trait for checking whether type \p T; is one of the policies defined in
    ///        `class null_termination_violation_policies`
    ///
    template <typename T>
    [[nodiscard]] constexpr static bool is_valid_one() noexcept
    {
        if constexpr (has_member_value_type<T>::value)
        {
            return is_one_of<T, abort, set_empty, throw_exception<typename T::value_type>>();
        }
        else
        {
            return false;
        }
    }

    ///
    /// @brief default violation policy
    ///
    using default_policy = abort;
};

}  // namespace score::safecpp

// NOLINTEND(readability-identifier-naming)

#endif  // SCORE_LANGUAGE_SAFECPP_STRING_VIEW_NULL_TERMINATION_VIOLATION_POLICIES_H
