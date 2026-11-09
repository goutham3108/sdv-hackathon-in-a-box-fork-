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
#ifndef SCORE_TEST_OS_QNX_SIMPLE_READ_FUNCTION_IMPL_H
#define SCORE_TEST_OS_QNX_SIMPLE_READ_FUNCTION_IMPL_H

#include "score/os/qnx/iofunc.h"
#include "score/os/qnx/resmgr_impl.h"
#include "score/os/utils/qnx/resource_manager/include/simple_read_function.h"

#include "score/result/result.h"

class SimpleReadFunctionStub final : public score::os::SimpleReadFunction
{
  public:
    SimpleReadFunctionStub(score::os::ResMgr& resmgr, score::os::IoFunc& iofunc, std::string& data);

  private:
    score::Result<size_t> read(const off_t offset, const size_t nbytes, uint64_t& result) override;

    std::string& data_;
};

#endif  // SCORE_TEST_OS_QNX_SIMPLE_READ_FUNCTION_IMPL_H
