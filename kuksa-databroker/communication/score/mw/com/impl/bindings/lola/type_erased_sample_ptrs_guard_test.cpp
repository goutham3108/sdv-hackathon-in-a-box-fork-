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
#include "score/mw/com/impl/bindings/lola/type_erased_sample_ptrs_guard.h"

#include "score/mw/com/impl/bindings/mock_binding/tracing/tracing_runtime.h"
#include "score/mw/com/impl/runtime.h"
#include "score/mw/com/impl/runtime_mock.h"
#include "score/mw/com/impl/test/runtime_mock_guard.h"
#include "score/mw/com/impl/tracing/tracing_runtime_mock.h"

#include <gtest/gtest.h>

namespace score::mw::com::impl::lola::tracing
{
namespace
{

using namespace ::testing;

using SamplePointerIndex = LolaEventInstanceDeployment::SampleSlotCountType;
using TracingSlotSizeType = LolaEventInstanceDeployment::TracingSlotSizeType;

SamplePointerIndex kSamplePointerIndex{5U};
TracingSlotSizeType kNumberOfServiceElementTracingSlots{10U};
const impl::tracing::ServiceElementTracingData kDummyServiceElementTracingData{kSamplePointerIndex,
                                                                               kNumberOfServiceElementTracingSlots};

class TypeErasedSamplePtrsGuardFixture : public ::testing::Test
{
  public:
    TypeErasedSamplePtrsGuardFixture()
    {
        ON_CALL(runtime_mock_guard_.runtime_mock_, GetTracingRuntime()).WillByDefault(Return(&tracing_runtime_mock_));
        ON_CALL(tracing_runtime_mock_, GetBindingTracingRuntime(BindingType::kLoLa))
            .WillByDefault(ReturnRef(binding_tracing_runtime_mock_));
    }

    RuntimeMockGuard runtime_mock_guard_{};
    NiceMock<impl::tracing::TracingRuntimeMock> tracing_runtime_mock_{};
    impl::tracing::mock_binding::TracingRuntime binding_tracing_runtime_mock_{};
};

TEST_F(TypeErasedSamplePtrsGuardFixture, WillCallClearTypeErasedSamplePtrsOnDestruction)
{
    // Given a TypeErasedSamplePtrsGuard
    TypeErasedSamplePtrsGuard unit{kDummyServiceElementTracingData};

    // Expecting ClearTypeErasedSamplePtrs to be called on the TracingRuntime
    EXPECT_CALL(binding_tracing_runtime_mock_, ClearTypeErasedSamplePtrs(kDummyServiceElementTracingData));

    // When destroying the TypeErasedSamplePtrsGuard
}

TEST_F(TypeErasedSamplePtrsGuardFixture, WillNotCallClearTypeErasedSamplePtrsOnDestructionIfTracingRuntimeDoesNotExist)
{
    // Given a TypeErasedSamplePtrsGuard
    TypeErasedSamplePtrsGuard unit{kDummyServiceElementTracingData};

    // Expecting that GetTracingRuntime is called once and returns a nullptr
    EXPECT_CALL(runtime_mock_guard_.runtime_mock_, GetTracingRuntime()).WillOnce(Return(nullptr));

    // Expecting that ClearTypeErasedSamplePtrs won't be called on the TracingRuntime
    EXPECT_CALL(binding_tracing_runtime_mock_, ClearTypeErasedSamplePtrs(kDummyServiceElementTracingData)).Times(0);

    // When destroying the TypeErasedSamplePtrsGuard
}

}  // namespace
}  // namespace score::mw::com::impl::lola::tracing
