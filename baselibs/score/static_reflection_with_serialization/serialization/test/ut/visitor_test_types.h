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
#ifndef COMMON_SERIALIZATION_TEST_UT_VISITOR_TEST_TYPES_H
#define COMMON_SERIALIZATION_TEST_UT_VISITOR_TEST_TYPES_H

#include <vector>

namespace test
{

template <class T, class A = ::std::allocator<T>>
class serializable_container_base
{
  protected:
    using container_type = std::vector<T, A>;
    container_type container_;

  public:
    using iterator = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;
    using value_type = typename container_type::value_type;

    serializable_container_base() {}

    // initializer list constructor for convenience in test code
    serializable_container_base(const std::initializer_list<T> initializer_list)
        : container_(initializer_list.begin(), initializer_list.end())
    {
    }

    iterator begin()
    {
        return std::begin(container_);
    }
    const_iterator cbegin() const
    {
        return std::cbegin(container_);
    }
    size_t size() const
    {
        return container_.size();
    }
    size_t max_size() const
    {
        return container_.max_size();
    }
    void push_back(const value_type& v)
    {
        container_.push_back(v);
    }
};

template <class T, class A = std::allocator<T>>
class resizeable_container : public serializable_container_base<T, A>
{
  public:
    resizeable_container() : max_size_{serializable_container_base<T, A>::container_.max_size()}, overflow_{false} {}

    explicit resizeable_container(size_t max_size) : max_size_{max_size}, overflow_{false} {}

    // initializer list constructor for convenience in test code
    resizeable_container(const std::initializer_list<T> initializer_list)
        : serializable_container_base<T, A>(initializer_list)
    {
    }

    size_t max_size() const
    {
        return max_size_;
    }

    void resize(size_t size)
    {
        overflow_ = size > max_size_;
        serializable_container_base<T, A>::container_.resize(size);
    }

    bool get_overflow() const
    {
        return overflow_;
    }

  private:
    size_t max_size_;
    bool overflow_;
};

template <class T, class A = std::allocator<T>>
class clearable_container : public serializable_container_base<T, A>
{
  public:
    clearable_container() {}

    // initializer list constructor for convenience in test code
    clearable_container(const std::initializer_list<T> initializer_list)
        : serializable_container_base<T, A>(initializer_list)
    {
    }

    void clear()
    {
        serializable_container_base<T, A>::container_.clear();
    }
};

template <class T, class A = std::allocator<T>>
class assignable_container : public serializable_container_base<T, A>
{
  public:
    assignable_container() {}

    // initializer list constructor for convenience in test code
    assignable_container(const std::initializer_list<T> initializer_list)
        : serializable_container_base<T, A>(initializer_list)
    {
    }

    assignable_container& operator=(const assignable_container&) = default;
    assignable_container& operator=(assignable_container&&) = default;
};

template <class T, class A = std::allocator<T>>
class unserializable_container : public serializable_container_base<T, A>
{
    unserializable_container() {}
    unserializable_container& operator=(const unserializable_container&) = delete;
    unserializable_container& operator=(unserializable_container&&) = delete;
};

}  // namespace test

#endif
