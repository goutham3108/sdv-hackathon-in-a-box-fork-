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
#include "score/os/utils/qnx/resource_manager/include/static_c_wrappers.h"
#include "score/os/utils/qnx/resource_manager/tests/mocks/open_function_mock.h"
#include "score/os/utils/qnx/resource_manager/tests/mocks/read_function_mock.h"
#include "score/os/utils/qnx/resource_manager/tests/mocks/seek_function_mock.h"
#include "score/os/utils/qnx/resource_manager/tests/mocks/write_function_mock.h"

#include "gtest/gtest.h"
#include <gmock/gmock.h>

#include <string>

#define CAST_WRITE(smart_ptr) (*dynamic_cast<score::os::test::WriteFunctionMock*>(smart_ptr.get()))
#define CAST_READ(smart_ptr) (*dynamic_cast<score::os::test::ReadFunctionMock*>(smart_ptr.get()))
#define CAST_OPEN(smart_ptr) (*dynamic_cast<score::os::test::OpenFunctionMock*>(smart_ptr.get()))
#define CAST_LSEEK(smart_ptr) (*dynamic_cast<score::os::test::SeekFunctionMock*>(smart_ptr.get()))
namespace score
{
namespace os
{
namespace test
{

TEST(ReadHandlerTest, registered_read_handler)
{
    std::shared_ptr<IReadFunction> read_mock = std::make_shared<ReadFunctionMock>();
    resmgr_context_t ctp{};
    RESMGR_OCB_T ocb{};
    io_read_t msg{};
    extended_dev_attr_t attr_temp{};

    ocb.attr = &attr_temp;
    ocb.attr->handlers.read_handler = read_mock;

    EXPECT_CALL(CAST_READ(read_mock), CallOperator(&ctp, &msg, &ocb)).WillOnce(::testing::Return(0));

    int ret_value = io_read_handler(&ctp, &msg, &ocb);
    EXPECT_EQ(ret_value, 0);
}

TEST(ReadHandlerTest, non_registered_read_handler)
{
    resmgr_context_t ctp{};
    RESMGR_OCB_T ocb{};
    io_read_t msg{};
    extended_dev_attr_t attr_temp{};

    ocb.attr = &attr_temp;

    int ret_value = io_read_handler(&ctp, &msg, &ocb);

    EXPECT_EQ(ret_value, ENOSYS);
}

TEST(WriteHandlerTest, registered_write_handler)
{
    std::shared_ptr<IWriteFunction> write_mock = std::make_shared<WriteFunctionMock>();
    resmgr_context_t ctp{};
    RESMGR_OCB_T ocb{};
    io_write_t msg{};
    extended_dev_attr_t attr_temp{};

    ocb.attr = &attr_temp;
    ocb.attr->handlers.write_handler = write_mock;

    EXPECT_CALL(CAST_WRITE(write_mock), CallOperator(&ctp, &msg, &ocb)).WillOnce(::testing::Return(0));

    int ret_value = io_write_handler(&ctp, &msg, &ocb);

    EXPECT_EQ(ret_value, 0);
}

TEST(WriteHandlerTest, non_registered_write_handler)
{
    resmgr_context_t ctp{};
    RESMGR_OCB_T ocb{};
    io_write_t msg{};
    extended_dev_attr_t attr_temp{};

    ocb.attr = &attr_temp;

    int ret_value = io_write_handler(&ctp, &msg, &ocb);

    EXPECT_EQ(ret_value, ENOSYS);
}

TEST(SeekHandlerTest, registered_lseek_handler)
{
    std::shared_ptr<ISeekFunction> lseek_mock = std::make_shared<SeekFunctionMock>();
    resmgr_context_t ctp{};
    RESMGR_OCB_T ocb{};
    io_lseek_t msg{};
    extended_dev_attr_t attr_temp{};

    ocb.attr = &attr_temp;
    ocb.attr->handlers.lseek_handler = lseek_mock;

    EXPECT_CALL(CAST_LSEEK(lseek_mock), CallOperator(&ctp, &msg, &ocb)).WillOnce(::testing::Return(0));

    int ret_value = io_lseek_handler(&ctp, &msg, &ocb);

    EXPECT_EQ(ret_value, 0);
}

TEST(SeekHandlerTest, non_registered_lseek_handler)
{
    resmgr_context_t ctp{};
    RESMGR_OCB_T ocb{};
    io_lseek_t msg{};
    extended_dev_attr_t attr_temp{};

    ocb.attr = &attr_temp;
    int ret_value = io_lseek_handler(&ctp, &msg, &ocb);

    EXPECT_EQ(ret_value, ENOSYS);
}

TEST(OpenHandlerTest, registered_open_handler)
{
    std::shared_ptr<IOpenFunction> open_mock = std::make_shared<OpenFunctionMock>();
    resmgr_context_t ctp{};
    RESMGR_HANDLE_T handle{};
    io_open_t msg{};
    void* extra = nullptr;
    handle.handlers.open_handler = open_mock;

    EXPECT_CALL(CAST_OPEN(open_mock), CallOperator(&ctp, &msg, &handle, extra)).WillOnce(::testing::Return(0));

    int ret_value = connect_open_handler(&ctp, &msg, &handle, extra);

    EXPECT_EQ(ret_value, 0);
}

TEST(OpenHandlerTest, non_registered_open_handler)
{
    resmgr_context_t ctp{};
    RESMGR_HANDLE_T handle{};
    io_open_t msg{};
    void* extra = nullptr;

    int ret_value = connect_open_handler(&ctp, &msg, &handle, extra);

    EXPECT_EQ(ret_value, ENOSYS);
}

}  // namespace test
}  // namespace os
}  // namespace score
