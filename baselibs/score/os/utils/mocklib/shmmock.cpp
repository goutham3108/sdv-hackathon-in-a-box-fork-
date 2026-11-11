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
#include "score/os/utils/mocklib/shmmock.h"

#include <functional>

namespace score
{
namespace os
{

namespace
{

std::function<void(const std::string&, std::size_t)> constructorCb;
std::function<void()> destructorCb;
std::function<void()> unlinkCb;
std::function<uint8_t*()> getCb;

} /* namespace */

ShmMock::ShmMock()
{
    using namespace std::placeholders;
    constructorCb = std::bind(&ShmMock::ShmWrapperMock_constructor, this, _1, _2);
    destructorCb = std::bind(&ShmMock::ShmWrapperMock_destructor, this);
    unlinkCb = std::bind(&ShmMock::unlink, this);
    getCb = std::bind(&ShmMock::get, this);
}

struct Shm::ShmPrivate
{
};

Shm::Shm(const std::string& name, std::size_t size)
{
    constructorCb(name, size);
}

Shm::~Shm()
{
    destructorCb();
}

void Shm::unlink() const
{
    unlinkCb();
}

uint8_t* Shm::get() const
{
    return getCb();
}

}  // namespace os
}  // namespace score
