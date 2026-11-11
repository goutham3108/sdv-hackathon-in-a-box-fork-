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
#ifndef SCORE_MW_LOG_DETAIL_BACKEND_MOCK_H
#define SCORE_MW_LOG_DETAIL_BACKEND_MOCK_H

#include "score/mw/log/detail/backend.h"
#include "score/mw/log/detail/log_record.h"

#include "gmock/gmock.h"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

class BackendMock : public Backend
{
  public:
    MOCK_METHOD((score::cpp::optional<SlotHandle>), ReserveSlot, (), (override));
    MOCK_METHOD((void), FlushSlot, (const SlotHandle&), (override));
    MOCK_METHOD((LogRecord&), GetLogRecord, (const SlotHandle&), (override));
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_DETAIL_BACKEND_MOCK_H
