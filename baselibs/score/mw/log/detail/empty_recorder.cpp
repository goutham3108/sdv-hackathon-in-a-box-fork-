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
#include "score/mw/log/detail/empty_recorder.h"

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

score::cpp::optional<SlotHandle> EmptyRecorder::StartRecord(const std::string_view, const LogLevel) noexcept
{
    return {};
}

void EmptyRecorder::StopRecord(const SlotHandle&) noexcept {}

void EmptyRecorder::Log(const SlotHandle&, const bool) noexcept {}
void EmptyRecorder::Log(const SlotHandle&, const std::uint8_t) noexcept {}
void EmptyRecorder::Log(const SlotHandle&, const std::int8_t) noexcept {}
void EmptyRecorder::Log(const SlotHandle&, const std::uint16_t) noexcept {}
void EmptyRecorder::Log(const SlotHandle&, const std::int16_t) noexcept {}
void EmptyRecorder::Log(const SlotHandle&, const std::uint32_t) noexcept {}
void EmptyRecorder::Log(const SlotHandle&, const std::int32_t) noexcept {}
void EmptyRecorder::Log(const SlotHandle&, const std::uint64_t) noexcept {}
void EmptyRecorder::Log(const SlotHandle&, const std::int64_t) noexcept {}
void EmptyRecorder::Log(const SlotHandle&, const float) noexcept {}
void EmptyRecorder::Log(const SlotHandle&, const double) noexcept {}
void EmptyRecorder::Log(const SlotHandle&, const std::string_view) noexcept {}

void EmptyRecorder::Log(const SlotHandle&, const LogHex8) noexcept {}
void EmptyRecorder::Log(const SlotHandle&, const LogHex16) noexcept {}
void EmptyRecorder::Log(const SlotHandle&, const LogHex32) noexcept {}
void EmptyRecorder::Log(const SlotHandle&, const LogHex64) noexcept {}

void EmptyRecorder::Log(const SlotHandle&, const LogBin8) noexcept {}
void EmptyRecorder::Log(const SlotHandle&, const LogBin16) noexcept {}
void EmptyRecorder::Log(const SlotHandle&, const LogBin32) noexcept {}
void EmptyRecorder::Log(const SlotHandle&, const LogBin64) noexcept {}

void EmptyRecorder::Log(const SlotHandle&, const LogRawBuffer) noexcept {}

void EmptyRecorder::Log(const SlotHandle&, const LogSlog2Message) noexcept {}

bool EmptyRecorder::IsLogEnabled(const LogLevel&, const std::string_view) const noexcept
{
    return false;
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score
