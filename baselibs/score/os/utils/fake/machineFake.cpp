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
#include "score/os/utils/machine.h"

namespace
{

class MachineImpl final : public score::os::Machine
{
  public:
    bool is_qemu() const noexcept override
    {
        return false;
    }
};

}  // namespace

score::os::Machine& score::os::Machine::instance() noexcept
{
    static MachineImpl implInstance;
    return select_instance(implInstance);
}

bool score::os::is_sctf() noexcept
{
    return false;
}
