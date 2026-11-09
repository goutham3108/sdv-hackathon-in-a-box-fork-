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
#ifndef SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_SCOPED_FUNCTION_SCOPE_H
#define SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_SCOPED_FUNCTION_SCOPE_H

#include "score/language/safecpp/scoped_function/details/allocator_wrapper.h"
#include "score/language/safecpp/scoped_function/details/scope_state.h"

#include <memory>
#include <mutex>

namespace score::safecpp
{

template <class Allocator = std::allocator<details::ScopeState>>
class Scope final
{
  public:
    using allocator_type = Allocator;

    Scope() : Scope{Allocator{}} {}

    explicit Scope(const Allocator& allocator)
        : allocator_{allocator}, scope_state_{std::allocate_shared<details::ScopeState>(*allocator_)}
    {
    }

    ~Scope()
    {
        Expire();
    }

    Scope(const Scope&) = delete;
    Scope& operator=(const Scope&) = delete;

    Scope(Scope&& other) noexcept : allocator_{std::move(other.allocator_)}, scope_state_{std::move(other.scope_state_)}
    {
    }

    // We explicitly do not provide an allocator-aware implementation of the move constructor, since a relocation of
    // the ScopeState would break the link to the ScopedFunctions.

    // Suppress "AUTOSAR C++14 A6-2-1" rule finding. This rule states: "Move and copy assignment operators shall either
    // move or respectively copy base classes and data members of a class, without any side effects.".
    // Rationale: This operator has side effects, as it expires the scope state and copies allocator, because the last
    // one does not support moving.
    // coverity[autosar_cpp14_a6_2_1_violation]
    Scope& operator=(Scope&& other) noexcept
    {
        static_assert(std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value,
                      "Allocators that do not propagate on move assignment are intentionally not supported as this "
                      "would require reallocation of the ScopeState.");
        if (this != &other)
        {
            Expire();
            allocator_ = other.allocator_;  // Allocators only support copying.
            scope_state_ = std::move(other.scope_state_);
        }
        return *this;
    }

    void Expire() noexcept
    {
        if (scope_state_ != nullptr)
        {
            scope_state_->Expire();
        }
    }

    [[nodiscard]] Allocator get_allocator() const noexcept
    {
        return *allocator_;
    }

  private:
    details::AllocatorWrapper<Allocator> allocator_;
    std::shared_ptr<details::ScopeState> scope_state_;

    // Suppres "AUTOSAR C++14 A11-3-1" rule finding: "Friend declarations shall not be used.".
    // Friendship is needed to allow CopyableScopedFunction to access the private GetScopeState() method.
    template <class, class>
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend class CopyableScopedFunction;

    // Suppres "AUTOSAR C++14 A11-3-1" rule finding: "Friend declarations shall not be used.".
    // Friendship is needed to allow MoveOnlyScopedFunction to access the private GetScopeState() method.
    template <class, class>
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend class MoveOnlyScopedFunction;

    [[nodiscard]] std::shared_ptr<details::ScopeState> GetScopeState() const noexcept
    {
        return scope_state_;
    }
};

}  // namespace score::safecpp

#endif  // SCORE_LANGUAGE_SAFECPP_SCOPED_FUNCTION_SCOPED_FUNCTION_SCOPE_H
