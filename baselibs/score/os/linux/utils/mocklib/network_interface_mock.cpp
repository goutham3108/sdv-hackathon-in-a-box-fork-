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
#include "network_interface_mock.h"
#include "score/os/linux/ifaddrs.h"
#include <functional>
#include <set>
#include <string>
#include <vector>

namespace score
{
namespace os
{

namespace
{
std::function<bool(network_settings&)> bFillInterfaceInformation_callback;
std::function<
    bool(network_settings&, std::set<std::string>&, const std::vector<std::function<bool(const network_interface&)>>&)>
    bFilterInterfaces_callback;
std::function<bool(const std::string&)> bIsInterfaceBridge_callback;
std::function<bool(const std::string&)> bIsInterfacePhyDevice_callback;
} /* namespace */

LinuxNetworkInterfaceMock::LinuxNetworkInterfaceMock()
{
    bFillInterfaceInformation_callback = [this](network_settings& ns) {
        return this->bFillInterfaceInformation(ns);
    };
    bIsInterfaceBridge_callback = [this](const std::string& n) {
        return this->bIsInterfaceBridge(n);
    };
    bIsInterfacePhyDevice_callback = [this](const std::string& n) {
        return this->bIsInterfaceBridge(n);
    };
};
} /* namespace os */
} /* namespace score */
