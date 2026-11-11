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

#include "score/mw/log/detail/log_record.h"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

namespace
{

void SetupBuffer(LogRecord& dst, const std::size_t capacity) noexcept
{
    // Beware that std::vector move assignment only preserves content, not capacity.
    dst.GetLogEntry().payload.shrink_to_fit();
    dst.GetLogEntry().payload.reserve(capacity);

    // Finally update the reference inside verbosePayload to point to the updated payload buffer.
    dst.GetVerbosePayload().SetBuffer(dst.GetLogEntry().payload);
}

LogRecord& MoveConstruct(LogRecord& dst, LogRecord&& src) noexcept
{
    // Get the capacity before we move from src.
    // Deviation from Rule A18-9-2:
    // - Forwarding values to other functions shall be done via: (1) std::move if the value is an rvalue reference, (2)
    // std::forward if the value is forwarding reference.
    // Justification:
    // - It's false positive, std::move of the expression of the trivially-copyable type 'std::vector<char>::size_type'
    // (aka 'unsigned long') has no effect.
    // coverity[autosar_cpp14_a18_9_2_violation : FALSE]
    const auto capacity = src.GetLogEntry().payload.capacity();

    // Beware of the lifetimes and **assignment order**: VerbosePayload contains a reference to
    // logEntry.verbosePayload_ that would be dangling when logEntry_ is assigned. Thus we update verbosePayload_ first,
    // and then logEntry_ to rule out any undefined behavior.
    // Justification:
    // - It's false positive, std::move of the expression of the trivially-copyable type 'detail::VerbosePayload' has no
    // effect.
    // coverity[autosar_cpp14_a18_9_2_violation : FALSE]
    dst.GetVerbosePayload() = src.GetVerbosePayload();

    // Justification:
    // - It's false positive, std::move used correctly.
    // coverity[autosar_cpp14_a18_9_2_violation : FALSE]
    dst.GetLogEntry() = std::move(src.GetLogEntry());

    SetupBuffer(dst, capacity);

    return dst;
}

LogRecord& CopyConstruct(LogRecord& dst, const LogRecord& src) noexcept
{
    // Beware of the lifetimes and **assignment order**: VerbosePayload contains a reference to
    // logEntry.verbosePayload_ that would be dangling when logEntry_ is assigned. Thus we update verbosePayload_ first,
    // and then logEntry_ to rule out any undefined behavior.
    dst.GetVerbosePayload() = src.GetVerbosePayload();
    dst.GetLogEntry() = src.GetLogEntry();

    SetupBuffer(dst, src.GetLogEntry().payload.capacity());

    return dst;
}

}  // namespace

// This is false positive. Constructor is declared only once.
// coverity[autosar_cpp14_a3_1_1_violation]
LogRecord::LogRecord(const std::size_t max_payload_size_bytes) noexcept
    : log_entry_{}, verbose_payload_(max_payload_size_bytes, log_entry_.payload)
{
}

LogEntry& LogRecord::GetLogEntry() noexcept
{
    // Returning address of non-static class member is justified by design
    // coverity[autosar_cpp14_a9_3_1_violation]
    return log_entry_;
}

detail::VerbosePayload& LogRecord::GetVerbosePayload() noexcept
{
    // Returning address of non-static class member is justified by design
    // coverity[autosar_cpp14_a9_3_1_violation]
    return verbose_payload_;
}

const LogEntry& LogRecord::GetLogEntry() const noexcept
{
    return log_entry_;
}

const detail::VerbosePayload& LogRecord::GetVerbosePayload() const noexcept
{
    return verbose_payload_;
}

LogRecord::LogRecord(const LogRecord& other) noexcept
    : log_entry_{other.log_entry_}, verbose_payload_{other.verbose_payload_}
{
    std::ignore = CopyConstruct(*this, other);
}

// a12_8_4 - Conflict with clang_tidy warning:
// std::move of the expression of the trivially-copyable type 'detail::VerbosePayload' has no effect.
// a3_1_1 - This is false positive. Constructor is declared only once.
// coverity[autosar_cpp14_a12_8_4_violation]
// coverity[autosar_cpp14_a3_1_1_violation]
LogRecord::LogRecord(LogRecord&& other) noexcept : log_entry_{}, verbose_payload_{other.verbose_payload_}
{
    std::ignore = MoveConstruct(*this, std::move(other));
}

LogRecord& LogRecord::operator=(const LogRecord& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }
    std::ignore = CopyConstruct(*this, other);
    return *this;
}

LogRecord& LogRecord::operator=(LogRecord&& other) noexcept
{
    std::ignore = MoveConstruct(*this, std::move(other));
    return *this;
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
