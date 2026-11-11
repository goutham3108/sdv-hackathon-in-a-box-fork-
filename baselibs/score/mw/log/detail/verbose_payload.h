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
#ifndef SCORE_MW_LOG_DETAIL_VERBOSE_PAYLOAD_H
#define SCORE_MW_LOG_DETAIL_VERBOSE_PAYLOAD_H

#include <score/callback.hpp>
#include <score/span.hpp>

#include <cstddef>
#include <cstring>
#include <type_traits>
#include <vector>

namespace score
{
namespace mw
{
namespace log
{
namespace detail
{

using Byte = char;
using ByteVector = std::vector<Byte>;

using ReserveCallback = score::cpp::callback<std::size_t(score::cpp::span<Byte>)>;

/// \brief Abstracts the usage of our underlying buffer for memory safety reasons.
///
/// \details We want to be sure that our buffer only allocates memory on construction.
/// By providing our wrapper around it we can ensure this. Another nice point is that we can encapsulate memory
/// handling. Meaning that an overflow of our buffer can be mitigated at one central place.
class VerbosePayload final
{
  public:
    /// \brief Constructs payload, by resizing buffer to max_size
    ///
    /// \param max_size The maximum size of memory that will be allocated
    /// \param buffer The buffer that shall be manipulated
    explicit VerbosePayload(const std::size_t max_size, ByteVector& buffer) noexcept;

    /// \brief Stores data in the underlying buffer, while taking care of memory safety
    ///
    /// \param data The pointer to data that shall be stored in the buffer
    /// \param length The length of data that shall be stored in the buffer
    ///
    /// \details If the buffer is full, then this function will not continue to fill it. Meaning, any data will be cut
    /// of at the maximum edge of the buffer. Otherwise the data will be appended.
    void Put(const Byte* const data, const std::size_t length) noexcept;

    /// \brief Stores data in the underlying buffer, while taking care of some memory safety
    ///
    /// \param callback function with signature std::size(*f)(score::cpp::callback<Byte>) used to fill provided buffer
    /// \param reserve_size reserve size for incoming data. When size zero is specified maximum allowed space is
    /// reserved. This is default behaviour
    ///
    /// \details User code must be careful not to exceed provided buffer capacity.
    /// Before return buffer is resized to fit the actual data use
    /// \return Number of bytes written
    std::size_t Put(const ReserveCallback callback, const std::size_t reserve_size = 0UL) noexcept;

    score::cpp::span<const std::uint8_t> GetSpan() const noexcept;

    /// \brief Clear buffer for next cycle operation
    ///
    void Reset() const noexcept;

    /// \brief Check if provided length fits into payload
    ///
    /// \param length The length which shall be checked
    /// \return true if length fits not in payload, false otherwise
    bool WillOverflow(const std::size_t length) const noexcept;

    /// \brief Return the number of remaining bytes that shall fit in the buffer.
    std::size_t RemainingCapacity() const noexcept;

    void SetBuffer(ByteVector&) noexcept;

  private:
    /// Use reference_wrapper to enable buffer rebinding in SetBuffer().
    std::reference_wrapper<ByteVector> buffer_;
};

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace score

#endif  // SCORE_MW_LOG_DETAIL_VERBOSE_PAYLOAD_H
