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
#include "score/os/utils/qnx/resource_manager/include/resource_manager.h"

#include "score/os/mocklib/qnx/mock_dispatch.h"
#include "score/os/mocklib/qnx/mock_iofunc.h"

#include "gtest/gtest.h"
#include <gmock/gmock.h>

#include <string>
#include <vector>

using ::testing::_;
using ::testing::InSequence;
using testing::InvokeWithoutArgs;
using ::testing::Return;

namespace score
{
namespace os
{
namespace test
{
static const std::int32_t kDummyAddress = 0xABCD;
dispatch_t* const kDispatchPointer = reinterpret_cast<dispatch_t*>(kDummyAddress);

class ResourceManagerTestFixture : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        // Prepare resource manager configuration
        resource_manager_configuration.flags = test_resmgr_flags;
        resource_manager_configuration.msg_max_size = 2048;
        resource_manager_configuration.nparts_max = 1;

        resources.push_back(resource_instance);

        resource_instance->SetResourcePath(test_path);
        resource_instance->SetResourceFileType(test_ftype);
        resource_instance->SetResourceFlags(test_flags);

        second_resource_instance->SetResourcePath(second_test_path);
        second_resource_instance->SetResourceFileType(second_test_ftype);
        second_resource_instance->SetResourceFlags(second_test_flags);
    }

    void expect_resource_manager_initialize_sequence()
    {
        EXPECT_CALL(*dynamic_cast<MockDispatch*>(dispatch_ptr.get()), dispatch_create())
            .WillOnce(Return(kDispatchPointer));

        EXPECT_CALL(
            *dynamic_cast<MockIoFunc*>(io_func_ptr.get()),
            iofunc_func_init(_RESMGR_CONNECT_NFUNCS,
                             const_cast<resmgr_connect_funcs_t*>(&(resource_instance->GetResourceConnectFunctions())),
                             _RESMGR_IO_NFUNCS,
                             const_cast<resmgr_io_funcs_t*>(&(resource_instance->GetResourceIoFunctions()))))
            .Times(1);

        EXPECT_CALL(*dynamic_cast<MockIoFunc*>(io_func_ptr.get()),
                    iofunc_attr_init(const_cast<iofunc_attr_t*>(&(device_attributes.attr)),
                                     resource_instance->GetResourceFlags(),
                                     nullptr,
                                     nullptr))
            .Times(1);

        EXPECT_CALL(
            *dynamic_cast<MockDispatch*>(dispatch_ptr.get()),
            resmgr_attach(_,
                          _,
                          testing::StrEq(test_path.c_str()),
                          test_ftype,
                          test_resmgr_flags,
                          const_cast<resmgr_connect_funcs_t*>(&(resource_instance->GetResourceConnectFunctions())),
                          const_cast<resmgr_io_funcs_t*>(&(resource_instance->GetResourceIoFunctions())),
                          const_cast<extended_dev_attr_t*>(&(resource_instance->GetResourceAttributes()))))
            .WillOnce(Return((std::int32_t)kDummyAddress));

        EXPECT_CALL(*dynamic_cast<MockDispatch*>(dispatch_ptr.get()), dispatch_context_alloc(_))
            .WillOnce(Return((dispatch_context_t*)kDummyAddress));
    }

  public:
    score::cpp::stop_source stop_source{};
    score::cpp::stop_token stop_token = stop_source.get_token();

    score::cpp::expected_blank<score::os::Error> score_future_cpp_ret = {};

    ResourceManagerConfig resource_manager_configuration;
    // prepare Resource
    std::shared_ptr<Resource> resource_instance = std::make_shared<Resource>();
    std::vector<std::shared_ptr<Resource>> resources;
    const extended_dev_attr_t& device_attributes = resource_instance->GetResourceAttributes();

    std::shared_ptr<Resource> second_resource_instance = std::make_shared<Resource>();
    const extended_dev_attr_t& second_device_attributes = second_resource_instance->GetResourceAttributes();

    const std::string test_path = "/dev/dev/dev/dev/1";
    const _file_type test_ftype = _FTYPE_FILE;
    const uint32_t test_flags = 0444;
    const uint32_t test_resmgr_flags = _RESMGR_FLAG_BEFORE;

    const std::string second_test_path = "/dev/dev/dev/dev/2";
    const _file_type second_test_ftype = _FTYPE_ANY;
    const uint32_t second_test_flags = 0666;
    // prepare OSAL system calls
    DispatchPtr dispatch_ptr = std::make_shared<MockDispatch>();
    IoFuncPtr io_func_ptr = std::make_shared<MockIoFunc>();
};

TEST_F(ResourceManagerTestFixture, should_initialize_successfully)
{
    InSequence s;
    expect_resource_manager_initialize_sequence();
    ResourceManager res_man(resource_manager_configuration, resources, dispatch_ptr, io_func_ptr);
    EXPECT_EQ(res_man.Initialize(), 0);
}

TEST_F(ResourceManagerTestFixture, should_initialize_more_resources_successfully)
{
    InSequence s;
    resources.push_back(second_resource_instance);

    EXPECT_CALL(
        *dynamic_cast<MockIoFunc*>(io_func_ptr.get()),
        iofunc_func_init(_RESMGR_CONNECT_NFUNCS,
                         const_cast<resmgr_connect_funcs_t*>(&(resource_instance->GetResourceConnectFunctions())),
                         _RESMGR_IO_NFUNCS,
                         const_cast<resmgr_io_funcs_t*>(&(resource_instance->GetResourceIoFunctions()))))
        .Times(1);

    EXPECT_CALL(*dynamic_cast<MockIoFunc*>(io_func_ptr.get()),
                iofunc_func_init(
                    _RESMGR_CONNECT_NFUNCS,
                    const_cast<resmgr_connect_funcs_t*>(&(second_resource_instance->GetResourceConnectFunctions())),
                    _RESMGR_IO_NFUNCS,
                    const_cast<resmgr_io_funcs_t*>(&(second_resource_instance->GetResourceIoFunctions()))))
        .Times(1);

    EXPECT_CALL(*dynamic_cast<MockIoFunc*>(io_func_ptr.get()),
                iofunc_attr_init(const_cast<iofunc_attr_t*>(&(device_attributes.attr)),
                                 resource_instance->GetResourceFlags(),
                                 nullptr,
                                 nullptr))
        .Times(1);

    EXPECT_CALL(*dynamic_cast<MockIoFunc*>(io_func_ptr.get()),
                iofunc_attr_init(const_cast<iofunc_attr_t*>(&(second_device_attributes.attr)),
                                 second_resource_instance->GetResourceFlags(),
                                 nullptr,
                                 nullptr))
        .Times(1);

    EXPECT_CALL(*dynamic_cast<MockDispatch*>(dispatch_ptr.get()),
                resmgr_attach(_,
                              _,
                              testing::StrEq(test_path.c_str()),
                              test_ftype,
                              test_resmgr_flags,
                              const_cast<resmgr_connect_funcs_t*>(&(resource_instance->GetResourceConnectFunctions())),
                              const_cast<resmgr_io_funcs_t*>(&(resource_instance->GetResourceIoFunctions())),
                              const_cast<extended_dev_attr_t*>(&(resource_instance->GetResourceAttributes()))))
        .WillOnce(Return((std::int32_t)kDummyAddress));

    EXPECT_CALL(
        *dynamic_cast<MockDispatch*>(dispatch_ptr.get()),
        resmgr_attach(_,
                      _,
                      testing::StrEq(second_test_path.c_str()),
                      second_test_ftype,
                      test_resmgr_flags,
                      const_cast<resmgr_connect_funcs_t*>(&(second_resource_instance->GetResourceConnectFunctions())),
                      const_cast<resmgr_io_funcs_t*>(&(second_resource_instance->GetResourceIoFunctions())),
                      const_cast<extended_dev_attr_t*>(&(second_resource_instance->GetResourceAttributes()))))
        .WillOnce(Return((std::int32_t)kDummyAddress));

    EXPECT_CALL(*dynamic_cast<MockDispatch*>(dispatch_ptr.get()), dispatch_context_alloc(_))
        .WillOnce(Return((dispatch_context_t*)kDummyAddress));

    ResourceManager res_man(resource_manager_configuration, resources, dispatch_ptr, io_func_ptr);
    EXPECT_EQ(res_man.Initialize(), 0);
}

TEST_F(ResourceManagerTestFixture, should_handle_dispatch_error)
{
    EXPECT_CALL(*dynamic_cast<MockDispatch*>(dispatch_ptr.get()), dispatch_create())
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno())));

    ResourceManager res_man(resource_manager_configuration, resources, dispatch_ptr, io_func_ptr);
    EXPECT_EQ(res_man.Initialize(), -1);
}

TEST_F(ResourceManagerTestFixture, should_handle_attach_error)
{
    EXPECT_CALL(*dynamic_cast<MockDispatch*>(dispatch_ptr.get()), dispatch_create()).WillOnce(Return(kDispatchPointer));

    EXPECT_CALL(
        *dynamic_cast<MockIoFunc*>(io_func_ptr.get()),
        iofunc_func_init(_RESMGR_CONNECT_NFUNCS,
                         const_cast<resmgr_connect_funcs_t*>(&(resource_instance->GetResourceConnectFunctions())),
                         _RESMGR_IO_NFUNCS,
                         const_cast<resmgr_io_funcs_t*>(&(resource_instance->GetResourceIoFunctions()))))
        .Times(1);

    EXPECT_CALL(*dynamic_cast<MockIoFunc*>(io_func_ptr.get()),
                iofunc_attr_init(const_cast<iofunc_attr_t*>(&(device_attributes.attr)),
                                 resource_instance->GetResourceFlags(),
                                 nullptr,
                                 nullptr))
        .Times(1);

    EXPECT_CALL(*dynamic_cast<MockDispatch*>(dispatch_ptr.get()),
                resmgr_attach(_,
                              _,
                              testing::StrEq(test_path.c_str()),
                              test_ftype,
                              test_resmgr_flags,
                              const_cast<resmgr_connect_funcs_t*>(&(resource_instance->GetResourceConnectFunctions())),
                              const_cast<resmgr_io_funcs_t*>(&(resource_instance->GetResourceIoFunctions())),
                              const_cast<extended_dev_attr_t*>(&(resource_instance->GetResourceAttributes()))))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno())));

    ResourceManager res_man(resource_manager_configuration, resources, dispatch_ptr, io_func_ptr);
    EXPECT_EQ(res_man.Initialize(), -1);
}

TEST_F(ResourceManagerTestFixture, should_handle_ctp_error)
{
    EXPECT_CALL(*dynamic_cast<MockDispatch*>(dispatch_ptr.get()), dispatch_create()).WillOnce(Return(kDispatchPointer));

    EXPECT_CALL(
        *dynamic_cast<MockIoFunc*>(io_func_ptr.get()),
        iofunc_func_init(_RESMGR_CONNECT_NFUNCS,
                         const_cast<resmgr_connect_funcs_t*>(&(resource_instance->GetResourceConnectFunctions())),
                         _RESMGR_IO_NFUNCS,
                         const_cast<resmgr_io_funcs_t*>(&(resource_instance->GetResourceIoFunctions()))))
        .Times(1);

    EXPECT_CALL(*dynamic_cast<MockIoFunc*>(io_func_ptr.get()),
                iofunc_attr_init(const_cast<iofunc_attr_t*>(&(device_attributes.attr)),
                                 resource_instance->GetResourceFlags(),
                                 nullptr,
                                 nullptr))
        .Times(1);

    EXPECT_CALL(*dynamic_cast<MockDispatch*>(dispatch_ptr.get()),
                resmgr_attach(_,
                              _,
                              testing::StrEq(test_path.c_str()),
                              test_ftype,
                              test_resmgr_flags,
                              const_cast<resmgr_connect_funcs_t*>(&(resource_instance->GetResourceConnectFunctions())),
                              const_cast<resmgr_io_funcs_t*>(&(resource_instance->GetResourceIoFunctions())),
                              const_cast<extended_dev_attr_t*>(&(resource_instance->GetResourceAttributes()))))
        .WillOnce(Return((std::int32_t)kDummyAddress));

    EXPECT_CALL(*dynamic_cast<MockDispatch*>(dispatch_ptr.get()), dispatch_context_alloc(_))
        .WillOnce(Return(score::cpp::make_unexpected(score::os::Error::createFromErrno())));

    ResourceManager res_man(resource_manager_configuration, resources, dispatch_ptr, io_func_ptr);
    EXPECT_EQ(res_man.Initialize(), -1);
}

TEST_F(ResourceManagerTestFixture, should_run_successfully)
{
    InSequence s;

    expect_resource_manager_initialize_sequence();
    ON_CALL(*dynamic_cast<MockDispatch*>(dispatch_ptr.get()), dispatch_block(_))
        .WillByDefault(::testing::InvokeWithoutArgs([&] {
            stop_source.request_stop();
            return score_future_cpp_ret;
        }));

    EXPECT_CALL(*dynamic_cast<MockDispatch*>(dispatch_ptr.get()), dispatch_handler(_))
        .WillOnce(::testing::Return(score::cpp::expected_blank<std::int32_t>{}));

    ResourceManager res_man(resource_manager_configuration, resources, dispatch_ptr, io_func_ptr);
    EXPECT_EQ(res_man.Initialize(), 0);
    // Run()
    EXPECT_EQ(res_man.Run(stop_token), 0);
    ASSERT_TRUE(stop_token.stop_requested());
}

TEST_F(ResourceManagerTestFixture, should_handle_dispatch_block_error)
{
    InSequence s;

    expect_resource_manager_initialize_sequence();

    EXPECT_CALL(*dynamic_cast<MockDispatch*>(dispatch_ptr.get()), dispatch_block(_))
        .WillOnce(::testing::Return(score::cpp::make_unexpected(score::os::Error::createFromErrno())));

    ResourceManager res_man(resource_manager_configuration, resources, dispatch_ptr, io_func_ptr);
    EXPECT_EQ(res_man.Initialize(), 0);
    // Run()
    EXPECT_EQ(res_man.Run(stop_token), -1);
}

TEST_F(ResourceManagerTestFixture, should_handle_dispatch_handle_error)
{
    InSequence s;

    expect_resource_manager_initialize_sequence();

    EXPECT_CALL(*dynamic_cast<MockDispatch*>(dispatch_ptr.get()), dispatch_block(_))
        .WillOnce(::testing::Return(score_future_cpp_ret));
    EXPECT_CALL(*dynamic_cast<MockDispatch*>(dispatch_ptr.get()), dispatch_handler(_))
        .WillOnce(::testing::Return(score::cpp::make_unexpected(-1)));

    ResourceManager res_man(resource_manager_configuration, resources, dispatch_ptr, io_func_ptr);
    EXPECT_EQ(res_man.Initialize(), 0);
    // Run()
    EXPECT_EQ(res_man.Run(stop_token), -1);
}
}  // namespace test
}  // namespace os
}  // namespace score
