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
#ifndef SCORE_TEST_OS_QNX_RESMGR_TEST_FIXTURE_H
#define SCORE_TEST_OS_QNX_RESMGR_TEST_FIXTURE_H

#include "score/os/qnx/channel_impl.h"
#include "score/os/qnx/dispatch_impl.h"
#include "score/os/qnx/iofunc.h"
#include "score/os/qnx/resmgr_impl.h"
#include "score/os/utils/qnx/resource_manager/tests/stubs/simple_read_function_stub.h"

#include <gtest/gtest.h>
#include <sys/iomsg.h>

class SimpleReadFunctionTestFixture : public ::testing::Test
{
  public:
    SimpleReadFunctionTestFixture();
    // no failures could be diagnosed here
    void InitResmgrStructures() noexcept;
    static score::cpp::expected<bool, std::int32_t> NextServiceRequest(dispatch_context_t* ctp) noexcept;
    // resmgr IO_READ callback
    static std::int32_t io_read(resmgr_context_t* ctp, io_read_t* msg, RESMGR_OCB_T* ocb) noexcept;
    // resmgr IO_SEEK callback
    static std::int32_t io_seek(resmgr_context_t* ctp, io_lseek_t* msg, iofunc_ocb_t* ocb) noexcept;

    // resmgr private message callback
    static std::int32_t private_message_handler(message_context_t* ctp,
                                                std::int32_t code,
                                                std::uint32_t flags,
                                                void* handle) noexcept;
    // statistics
    score::cpp::expected<size_t, score::os::Error> status;
    std::string read_data;

    // exit flag
    bool to_exit{};

    // resmgr-specific user-provided structures with long lifetimes
    resmgr_attr_t resmgr_attr{};
    resmgr_connect_funcs_t connect_funcs{};
    resmgr_io_funcs_t io_funcs{};
    IOFUNC_ATTR_T attr{};

    // qnx wrappers intances
    score::os::ResMgrImpl resmgr;
    score::os::ChannelImpl channel;
    score::os::DispatchImpl dispatch;
    score::os::IoFuncQnx iofunc;

    SimpleReadFunctionStub simple_read_function_stub_;
};

#endif  // SCORE_TEST_OS_QNX_RESMGR_TEST_FIXTURE_H
