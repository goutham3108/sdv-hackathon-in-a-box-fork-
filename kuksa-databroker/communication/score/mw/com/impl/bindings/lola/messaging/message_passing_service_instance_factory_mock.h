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
#ifndef SCORE_MW_COM_IMPL_BINDINGS_LOLA_MESSAGEPASSINGSERVICEINSTANCEFACTORYMOCK_H
#define SCORE_MW_COM_IMPL_BINDINGS_LOLA_MESSAGEPASSINGSERVICEINSTANCEFACTORYMOCK_H

#include "score/mw/com/impl/bindings/lola/messaging/i_message_passing_service_instance_factory.h"

#include "gmock/gmock.h"

namespace score::mw::com::impl::lola::test
{

class MessagePassingServiceInstanceFactoryMock final : public IMessagePassingServiceInstanceFactory
{
  public:
    MOCK_METHOD((std::unique_ptr<IMessagePassingServiceInstance>),
                Create,
                (const ClientQualityType,
                 const AsilSpecificCfg,
                 score::message_passing::IServerFactory&,
                 score::message_passing::IClientFactory&,
                 score::concurrency::Executor&),
                (const, noexcept, override));
};

}  // namespace score::mw::com::impl::lola::test

#endif  // SCORE_MW_COM_IMPL_BINDINGS_LOLA_MESSAGEPASSINGSERVICEINSTANCEFACTORYMOCK_H
