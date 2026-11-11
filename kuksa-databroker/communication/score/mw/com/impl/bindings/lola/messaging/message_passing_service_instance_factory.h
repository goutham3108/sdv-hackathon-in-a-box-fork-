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
#ifndef SCORE_MW_COM_IMPL_BINDINGS_LOLA_MESSAGEPASSINGSERVICEINSTANCEFACTORY_H
#define SCORE_MW_COM_IMPL_BINDINGS_LOLA_MESSAGEPASSINGSERVICEINSTANCEFACTORY_H

#include "score/mw/com/impl/bindings/lola/messaging/i_message_passing_service_instance.h"
#include "score/mw/com/impl/bindings/lola/messaging/i_message_passing_service_instance_factory.h"

#include <memory>

namespace score::mw::com::impl::lola
{

class MessagePassingServiceInstanceFactory final : public IMessagePassingServiceInstanceFactory
{
  public:
    [[nodiscard]] std::unique_ptr<IMessagePassingServiceInstance> Create(
        ClientQualityType client_quality_type,
        AsilSpecificCfg config,
        score::message_passing::IServerFactory& server_factory,
        score::message_passing::IClientFactory& client_factory,
        score::concurrency::Executor& executor) const noexcept override;
};

}  // namespace score::mw::com::impl::lola

#endif  // SCORE_MW_COM_IMPL_BINDINGS_LOLA_MESSAGEPASSINGSERVICEINSTANCEFACTORY_H
