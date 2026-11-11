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
#ifndef SCORE_MW_LOG_DETAIL_BACKEND_H
#define SCORE_MW_LOG_DETAIL_BACKEND_H

#include "score/mw/log/detail/log_record.h"
#include "score/mw/log/detail/verbose_payload.h"
#include "score/mw/log/recorder.h"

#include "score/optional.hpp"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

/// \brief The backend represents an interface that abstracts a buffer where the final log data shall be stored in a
/// thread-safe lock-free manner. Therefore, a user can request a _Slot_ where he can write data. He shall flush the
/// slot, once he finished putting his data in.
class Backend
{
  public:
    /// \brief Before a producer can store data in our buffer, he has to reserve a slot.
    ///
    /// \return SlotHandle if a slot was able to be reserved, empty otherwise.
    ///
    /// \post This ensures that no other thread will write to the reserved slot until FlushSlot() is invoked.
    virtual score::cpp::optional<SlotHandle> ReserveSlot() = 0;

    /// \brief After a producer finished writing into a slot Flush() needs to be called.
    ///
    /// \param slot The slot that shall be flushed
    ///
    /// \pre ReserveSlot() was invoked to get a SlotHandle that shall be flushed
    /// \post This ensures that afterwards the respective slot can be either read or overwritten
    virtual void FlushSlot(const SlotHandle& slot) = 0;

    /// \brief In order to stream data into a slot, the underlying slot buffer needs to be exposed.
    ///
    /// \param slot The slot for which the associated buffer shall be returned
    /// \return The payload associated with slot. (Where a producer can add its data)
    ///
    /// \pre ReserveSlot() was invoked to receive a SlotHandle
    virtual LogRecord& GetLogRecord(const SlotHandle& slot) = 0;

    Backend() = default;
    virtual ~Backend();
    Backend(const Backend&) = delete;
    Backend(Backend&&) = delete;
    Backend& operator=(Backend&&) = delete;
    Backend& operator=(const Backend&) = delete;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_DETAIL_BACKEND_H
