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
#include "score/os/utils/qnx/resource_manager/tests/stubs/open_function_stub.h"
#include "score/os/utils/qnx/resource_manager/tests/stubs/read_function_stub.h"
#include "score/os/utils/qnx/resource_manager/tests/stubs/seek_function_stub.h"
#include "score/os/utils/qnx/resource_manager/tests/stubs/write_function_stub.h"

#include "gtest/gtest.h"
#include <gmock/gmock.h>

#include <memory>
#include <string>

namespace score
{
namespace os
{
namespace test
{

const std::string kPathname = "/dev/test/test1/test2";
const std::uint32_t kFlags = 0666;
const _file_type kFileType = _FTYPE_ANY;
const extended_dev_attr_t kResourceAttributes{};

struct ResourceTestFixture : public ::testing::Test
{
    std::unique_ptr<Resource> CreateResource()
    {
        return std::make_unique<Resource>(kPathname, kFlags, kFileType, kResourceAttributes);
    }

    std::unique_ptr<Resource> CreateResourceWithNoParameters()
    {
        return std::make_unique<Resource>();
    }
};

TEST_F(ResourceTestFixture, resource_path_getter)
{
    const auto resource = CreateResource();
    EXPECT_STREQ(resource->GetResourcePath().c_str(), kPathname.c_str());
}
TEST_F(ResourceTestFixture, resource_flags_getter)
{
    const auto resource = CreateResource();
    EXPECT_EQ(resource->GetResourceFlags(), kFlags);
}
TEST_F(ResourceTestFixture, resource_file_type_getter)
{
    const auto resource = CreateResource();
    EXPECT_EQ(resource->GetResourceFileType(), kFileType);
}
TEST_F(ResourceTestFixture, resource_attributes_getter)
{
    const auto resource = CreateResource();
    EXPECT_EQ(resource->GetResourceAttributes().handlers.lseek_handler.get(), nullptr);
    EXPECT_EQ(resource->GetResourceAttributes().handlers.open_handler.get(), nullptr);
    EXPECT_EQ(resource->GetResourceAttributes().handlers.read_handler.get(), nullptr);
    EXPECT_EQ(resource->GetResourceAttributes().handlers.write_handler.get(), nullptr);
}
TEST_F(ResourceTestFixture, resource_path_setter)
{
    const auto resource = CreateResource();
    const std::string test_path = "/dev/dev/dev/dev";
    resource->SetResourcePath(test_path);
    EXPECT_STREQ(resource->GetResourcePath().c_str(), test_path.c_str());
}
TEST_F(ResourceTestFixture, resource_flags_setter)
{
    const auto resource = CreateResource();
    const std::uint32_t test_flags = 0444;
    resource->SetResourceFlags(test_flags);
    EXPECT_EQ(resource->GetResourceFlags(), test_flags);
}
TEST_F(ResourceTestFixture, resource_file_type_setter)
{
    const auto resource = CreateResource();
    const _file_type test_ftype = _FTYPE_FILE;
    resource->SetResourceFileType(test_ftype);
    EXPECT_EQ(resource->GetResourceFileType(), test_ftype);
}
TEST_F(ResourceTestFixture, resource_attributes_setter)
{
    const auto resource = CreateResource();
    std::shared_ptr<IWriteFunction> write_interface_instance = std::make_shared<WriteFunctionStub>();
    std::shared_ptr<IReadFunction> read_interface_instance = std::make_shared<ReadFunctionStub>();
    std::shared_ptr<ISeekFunction> lseek_interface_instance = std::make_shared<SeekFunctionStub>();
    std::shared_ptr<IOpenFunction> open_interface_instance = std::make_shared<OpenFunctionStub>();

    resource->SetRequestHandler(read_interface_instance);
    resource->SetRequestHandler(write_interface_instance);
    resource->SetRequestHandler(lseek_interface_instance);
    resource->SetRequestHandler(open_interface_instance);

    EXPECT_EQ(resource->GetResourceAttributes().handlers.lseek_handler.get(), lseek_interface_instance.get());
    EXPECT_EQ(resource->GetResourceAttributes().handlers.open_handler.get(), open_interface_instance.get());
    EXPECT_EQ(resource->GetResourceAttributes().handlers.read_handler.get(), read_interface_instance.get());
    EXPECT_EQ(resource->GetResourceAttributes().handlers.write_handler.get(), write_interface_instance.get());
}
TEST_F(ResourceTestFixture, register_io_funcs)
{
    const auto resource = CreateResource();

    std::shared_ptr<IWriteFunction> write_interface_instance = std::make_shared<WriteFunctionStub>();
    std::shared_ptr<IReadFunction> read_interface_instance = std::make_shared<ReadFunctionStub>();
    std::shared_ptr<ISeekFunction> lseek_interface_instance = std::make_shared<SeekFunctionStub>();

    resource->SetRequestHandler(read_interface_instance);
    resource->SetRequestHandler(write_interface_instance);
    resource->SetRequestHandler(lseek_interface_instance);

    resource->AttachRegisteredHandlers();

    EXPECT_EQ(resource->GetResourceIoFunctions().lseek, io_lseek_handler);
    EXPECT_EQ(resource->GetResourceIoFunctions().read, io_read_handler);
    EXPECT_EQ(resource->GetResourceIoFunctions().write, io_write_handler);
}
TEST_F(ResourceTestFixture, register_connect_funcs)
{
    const auto resource = CreateResource();

    std::shared_ptr<IOpenFunction> open_interface_instance = std::make_shared<OpenFunctionStub>();

    resource->SetRequestHandler(open_interface_instance);

    resource->AttachRegisteredHandlers();

    EXPECT_EQ(resource->GetResourceConnectFunctions().open, connect_open_handler);
}
TEST_F(ResourceTestFixture, non_registered_io_funcs)
{
    const auto resource = CreateResource();

    EXPECT_EQ(resource->GetResourceIoFunctions().lseek, nullptr);
    EXPECT_EQ(resource->GetResourceIoFunctions().read, nullptr);
    EXPECT_EQ(resource->GetResourceIoFunctions().write, nullptr);

    resource->AttachRegisteredHandlers();

    EXPECT_EQ(resource->GetResourceIoFunctions().lseek, nullptr);
    EXPECT_EQ(resource->GetResourceIoFunctions().read, &io_read_handler);
    EXPECT_EQ(resource->GetResourceIoFunctions().write, &io_write_handler);
}
TEST_F(ResourceTestFixture, non_registered_connect_funcs)
{
    const auto resource = CreateResource();

    EXPECT_EQ(resource->GetResourceConnectFunctions().open, nullptr);

    resource->AttachRegisteredHandlers();

    EXPECT_EQ(resource->GetResourceConnectFunctions().open, nullptr);
}
TEST_F(ResourceTestFixture, complete_use_case)
{
    const auto resource = CreateResourceWithNoParameters();
    const std::string test_path = "/dev/dev/dev/dev";
    const _file_type test_ftype = _FTYPE_FILE;
    const std::uint32_t test_flags = 0444;

    std::shared_ptr<IWriteFunction> write_interface_instance = std::make_shared<WriteFunctionStub>();
    std::shared_ptr<IReadFunction> read_interface_instance = std::make_shared<ReadFunctionStub>();
    std::shared_ptr<ISeekFunction> lseek_interface_instance = std::make_shared<SeekFunctionStub>();
    std::shared_ptr<IOpenFunction> open_interface_instance = std::make_shared<OpenFunctionStub>();

    resource->SetResourcePath(test_path);
    resource->SetResourceFileType(test_ftype);
    resource->SetResourceFlags(test_flags);

    resource->SetRequestHandler(read_interface_instance);
    resource->SetRequestHandler(write_interface_instance);
    resource->SetRequestHandler(lseek_interface_instance);
    resource->SetRequestHandler(open_interface_instance);

    EXPECT_STREQ(resource->GetResourcePath().c_str(), test_path.c_str());
    EXPECT_EQ(resource->GetResourceFlags(), test_flags);
    EXPECT_EQ(resource->GetResourceFileType(), test_ftype);
    EXPECT_EQ(resource->GetResourceAttributes().handlers.lseek_handler.get(), lseek_interface_instance.get());
    EXPECT_EQ(resource->GetResourceAttributes().handlers.open_handler.get(), open_interface_instance.get());
    EXPECT_EQ(resource->GetResourceAttributes().handlers.read_handler.get(), read_interface_instance.get());
    EXPECT_EQ(resource->GetResourceAttributes().handlers.write_handler.get(), write_interface_instance.get());

    resource->AttachRegisteredHandlers();

    EXPECT_EQ(resource->GetResourceIoFunctions().lseek, io_lseek_handler);
    EXPECT_EQ(resource->GetResourceIoFunctions().read, io_read_handler);
    EXPECT_EQ(resource->GetResourceIoFunctions().write, io_write_handler);
    EXPECT_EQ(resource->GetResourceConnectFunctions().open, connect_open_handler);
}
}  // namespace test
}  // namespace os
}  // namespace score
