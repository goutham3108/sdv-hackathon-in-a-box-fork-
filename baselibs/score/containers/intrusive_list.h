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
#ifndef SCORE_LIB_CONTAINERS_INTRUSIVE_LIST_H
#define SCORE_LIB_CONTAINERS_INTRUSIVE_LIST_H

#include <score/assert.hpp>
#include <score/utility.hpp>

#include <algorithm>
#include <iterator>
#include <type_traits>

/// Implementing [P0406R1](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0406r1.html) with some additions:
///
/// * pop_front() was obviously an unintended omission in P0406R1
///
/// * iterator_to() method from Boost allows for unlinking of the element without keeping a (redundant) iterator to it.
///   There may also be other uses of the resulting iterator, but this use alone warrants this addition.
///
/// * ..._and_dispose...() series of methods from Boost can address the lack of destructor calls during unlinking.
///   When we remove an element from a non-intrusive container, a copy of the element is destructed, which can
///   be used to run user-defined code associated with such removal. While unlinking elements from an intrusive
///   container, a destructor is not called automatically, but a user-provided disposer callable can be used instead.
///
/// P0406R1 does not support "auto-unlinking" (i.e. removing an element from the intrusive container when the element
/// destructor is called); we also consider such situation as a user error. However, when an element belonging to a
/// list is move-constructed, we replace the old instance in the list with the new one.
///
/// No support for fancy pointers yet; by using a kind of "tag traits", it would be able to introduce fancy pointers
/// later without changing the interface.

namespace score
{
namespace containers
{

struct default_intrusive_tag;

/// This class has two functions:
/// 1. The actual list element object is derived from this class. The Tag shall match the tag of the list where this
///    element is going to be inserted. If the element is intended to belong to several lists at the same time, multiple
///    base classes with different tags shall be used.
/// 2. The same class with the same Tag is used as a root node in the list object itself, as a part of the list
///    implementation.
template <typename Tag = default_intrusive_tag>
class intrusive_list_element
{

  protected:
    /// by default, the element doesn't belong to any list
    intrusive_list_element() noexcept = default;

    /// when copy-constructed, the new element doesn't belong to any list, even if the original does,
    /// that's why we need an explicit non-defaulted copy constructor with an empty body
    // coverity[autosar_cpp14_a12_8_1_violation] see above
    intrusive_list_element(const intrusive_list_element&) noexcept {}

    /// when move-constructed, the element replaces the original in its list
    // intentional side effects to preserve list integrity
    // coverity[autosar_cpp14_a12_8_1_violation]
    intrusive_list_element(intrusive_list_element&& from) noexcept
    {
        if (from.is_linked())
        {
            next_ = from.next_;
            next_->prev_ = this;
            prev_ = from.prev_;
            prev_->next_ = this;
            from.unlink();
        }
    }

    /// when destructed, the element shall not belong to any list
    ~intrusive_list_element() noexcept
    {
        SCORE_LANGUAGE_FUTURECPP_ASSERT(!is_linked());
    }

    intrusive_list_element& operator=(const intrusive_list_element&) noexcept = delete;
    intrusive_list_element& operator=(intrusive_list_element&&) noexcept = delete;

  private:
    template <typename T, typename Tag1>
    // Suppress "AUTOSAR C++14 A11-3-1" rule finding: "Friend declarations shall not be used."
    // Apart from replacement by move-constructor, only intrusive_list is allowed to link intrusive_list_element(s)
    // coverity[autosar_cpp14_a11_3_1_violation]
    friend class intrusive_list;

    bool is_linked() const noexcept
    {
        return next_ != nullptr;
    };
    void unlink() noexcept
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(is_linked());
        next_ = nullptr;
        prev_ = nullptr;
    };

    void make_empty_root() noexcept
    {
        next_ = this;
        prev_ = this;
    };

    intrusive_list_element* next_{nullptr};
    intrusive_list_element* prev_{nullptr};
};

template <typename T, typename Tag = default_intrusive_tag>
class intrusive_list
{
    static_assert(std::is_base_of_v<intrusive_list_element<Tag>, T>,
                  "T does not inherit from intrusive_list_element of given tag");
    static_assert(!std::is_const_v<T>, "intrusive_list cannot support const element types");

    template <typename InputIterator>
    using is_valid_input_iterator =
        typename std::bool_constant<std::is_base_of_v<T, typename std::iterator_traits<InputIterator>::value_type> &&
                                    !std::is_const_v<typename std::iterator_traits<InputIterator>::value_type>>;

    template <typename InputIterator>
    // false-positive: Type trait used to validate input iterators in template functions
    // coverity[autosar_cpp14_a0_1_1_violation]
    static inline constexpr bool is_valid_input_iterator_v = is_valid_input_iterator<InputIterator>::value;

  public:
    // types:
    using value_type = T;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    template <bool IsConst>
    class iterator_impl
    {
      public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = intrusive_list::value_type;
        using difference_type = intrusive_list::difference_type;
        using pointer = typename std::conditional_t<IsConst, const value_type*, value_type*>;
        using reference = typename std::conditional_t<IsConst, const value_type&, value_type&>;

        iterator_impl() noexcept = default;

        // iterators provided by containers require implicit conversion to const-iterators
        template <bool WasConst, class = std::enable_if_t<IsConst && !WasConst>>
        // NOLINTNEXTLINE(google-explicit-constructor) see above
        iterator_impl(const iterator_impl<WasConst>& from) noexcept : node_{from.node_}
        {
        }

        iterator_impl& operator++() noexcept
        {
            SCORE_LANGUAGE_FUTURECPP_PRECONDITION(is_valid());
            node_ = node_->next_;
            return *this;
        }

        iterator_impl operator++(int) noexcept
        {
            SCORE_LANGUAGE_FUTURECPP_PRECONDITION(is_valid());
            const iterator_impl result{*this};
            node_ = node_->next_;
            return result;
        }

        iterator_impl& operator--() noexcept
        {
            SCORE_LANGUAGE_FUTURECPP_PRECONDITION(is_valid());
            node_ = node_->prev_;
            return *this;
        }

        iterator_impl operator--(int) noexcept
        {
            SCORE_LANGUAGE_FUTURECPP_PRECONDITION(is_valid());
            const iterator_impl result{*this};
            node_ = node_->prev_;
            return result;
        }

        // coverity[autosar_cpp14_a3_3_1_violation] false-positive: in header
        friend bool operator==(const iterator_impl& l, const iterator_impl& r) noexcept
        {
            return l.node_ == r.node_;
        }

        // coverity[autosar_cpp14_a3_3_1_violation] false-positive: in header
        friend bool operator!=(const iterator_impl& l, const iterator_impl& r) noexcept
        {
            return !(l == r);
        }

        reference operator*() const noexcept
        {
            return *operator->();
        }

        pointer operator->() const noexcept
        {
            SCORE_LANGUAGE_FUTURECPP_PRECONDITION(is_valid());
            return static_cast<pointer>(node_);
        }

      private:
        template <typename T1, typename Tag1>
        // Suppress "AUTOSAR C++14 A11-3-1" rule finding: "Friend declarations shall not be used."
        // Only intrusive_list is allowed to construct intrusive_list::iterator_impl from a raw pointer to the node
        // coverity[autosar_cpp14_a11_3_1_violation]
        friend class intrusive_list;

        using node_pointer = intrusive_list_element<Tag>*;
        using const_node_pointer = const intrusive_list_element<Tag>*;

        constexpr explicit iterator_impl(const node_pointer node) noexcept : node_{node} {}

        // Suppress "AUTOSAR C++14 A5-2-3" rule finding:
        // "A cast shall not remove any const or volatile qualification from the type of a pointer or reference."
        // Unfortunately, the interface requires us to support const_iterators as positions for insertion
        // fortunately, this constness removal does not leak to the user through the interface
        // NOLINTBEGIN(cppcoreguidelines-pro-type-const-cast): Effect isolated by the API
        // coverity[autosar_cpp14_a5_2_3_violation]
        constexpr explicit iterator_impl(const const_node_pointer node) noexcept : node_{const_cast<node_pointer>(node)}
        {
        }
        // NOLINTEND(cppcoreguidelines-pro-type-const-cast): Effect isolated by the API

        bool is_valid() const noexcept
        {
            return node_ != nullptr;
        };

        // only for use in non-const intrusive_list methods with const_iterator position as a parameter
        iterator_impl<false> convert_to_non_const() const
        {
            return iterator_impl<false>{node_};
        }

        node_pointer node_{nullptr};
    };

    using iterator = iterator_impl<false>;
    using const_iterator = iterator_impl<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    // construct/copy/destroy:
    intrusive_list() noexcept
    {
        root_.make_empty_root();
    }

    template <typename InputIterator, class = std::enable_if_t<is_valid_input_iterator_v<InputIterator>>>
    intrusive_list(InputIterator first, InputIterator last)
    {
        root_.make_empty_root();
        assign(first, last);
    }

    // side effects are present due to the list implementation
    // coverity[autosar_cpp14_a12_8_1_violation]
    // coverity[autosar_cpp14_a12_8_4_violation] false-positive: no copy semanitc is used
    intrusive_list(intrusive_list&& from) noexcept
    {
        root_.make_empty_root();
        *this = std::move(from);
    }

    intrusive_list(const intrusive_list&) noexcept = delete;

    ~intrusive_list() noexcept
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(empty());
        root_.unlink();
    }

    intrusive_list& operator=(intrusive_list&& from) & noexcept
    {
        if (this == &from)
        {
            // better than "valid but unspecified state", as "no auto-unlink" property is thus guaranteed
            return *this;
        }

        clear();

        if (from.empty())
        {
            // nothing to do, both lists are already empty
        }
        else
        {
            root_.prev_ = from.root_.prev_;
            root_.prev_->next_ = &root_;
            root_.next_ = from.root_.next_;
            root_.next_->prev_ = &root_;
            from.root_.make_empty_root();
        }
        return *this;
    }

    intrusive_list& operator=(const intrusive_list&) noexcept = delete;

    template <typename InputIterator, class = std::enable_if_t<is_valid_input_iterator_v<InputIterator>>>
    void assign(InputIterator first, InputIterator last)
    {
        dispose_and_assign([](auto&&) noexcept {}, first, last);
    }

    template <typename InputIterator,
              typename Disposer,
              class = std::enable_if_t<is_valid_input_iterator_v<InputIterator>>>
    void dispose_and_assign(Disposer disposer, InputIterator first, InputIterator last)
    {
        clear_and_dispose(disposer);
        // non-const argument, that's why copying to std::back_inserter won't work
        score::cpp::ignore = std::for_each(first, last, [this](T& element) {
            push_back(element);
        });
    }

    // iterators:
    iterator begin() noexcept
    {
        return iterator{root_.next_};
    }
    const_iterator begin() const noexcept
    {
        return const_iterator{root_.next_};
    }
    iterator end() noexcept
    {
        return iterator{&root_};
    }
    const_iterator end() const noexcept
    {
        return const_iterator{&root_};
    }

    reverse_iterator rbegin() noexcept
    {
        return std::make_reverse_iterator(end());
    }
    const_reverse_iterator rbegin() const noexcept
    {
        return std::make_reverse_iterator(end());
    }
    reverse_iterator rend() noexcept
    {
        return std::make_reverse_iterator(begin());
    }
    const_reverse_iterator rend() const noexcept
    {
        return std::make_reverse_iterator(begin());
    }

    const_iterator cbegin() const noexcept
    {
        return begin();
    }
    const_iterator cend() const noexcept
    {
        return end();
    }
    const_reverse_iterator crbegin() const noexcept
    {
        return rbegin();
    }
    const_reverse_iterator crend() const noexcept
    {
        return rend();
    }

    // capacity: time complexity O(1), O(N), and O(1), respectively
    bool empty() const noexcept
    {
        return root_.next_ == &root_;
    }
    size_type size() const noexcept
    {
        return static_cast<size_type>(std::distance(begin(), end()));
    }
    size_type max_size() const noexcept
    {
        return std::numeric_limits<difference_type>::max();
    }

    // element access:
    reference front() noexcept
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(!empty());
        return static_cast<reference>(*root_.next_);
    }

    const_reference front() const noexcept
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(!empty());
        return static_cast<const_reference>(*root_.next_);
    }

    reference back() noexcept
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(!empty());
        return static_cast<reference>(*root_.prev_);
    }

    const_reference back() const noexcept
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(!empty());
        return static_cast<const_reference>(*root_.prev_);
    }

    // modifiers:
    void push_front(T& x) noexcept
    {
        insert_before(x, *root_.next_);
    }

    void push_back(T& x) noexcept
    {
        insert_before(x, root_);
    }

    void pop_front() noexcept
    {
        pop_front_and_dispose([](auto&&) noexcept {});
    }

    template <typename Disposer>
    void pop_front_and_dispose(Disposer disposer) noexcept
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(!empty());
        intrusive_list_element<Tag>& node = *root_.next_;
        remove_and_dispose_node(node, disposer);
    }

    void pop_back() noexcept
    {
        pop_back_and_dispose([](auto&&) noexcept {});
    }

    template <typename Disposer>
    void pop_back_and_dispose(Disposer disposer) noexcept
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(!empty());
        intrusive_list_element<Tag>& node = *root_.prev_;
        remove_and_dispose_node(node, disposer);
    }

    iterator insert(const const_iterator position, T& x) noexcept
    {
        intrusive_list_element<Tag>& node = x;
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(!node.is_linked());
        insert_node_before(node, *position.node_);
        return iterator{&node};
    }

    template <typename InputIterator, class = std::enable_if_t<is_valid_input_iterator_v<InputIterator>>>
    iterator insert(const const_iterator position, InputIterator first, InputIterator last)
    {
        if (first == last)
        {
            return position.convert_to_non_const();
        }
        const iterator new_first = insert(position, *first);
        score::cpp::ignore = std::for_each(std::next(first), last, [this, position](T& element) {
            score::cpp::ignore = insert(position, element);
        });
        return new_first;
    }

    iterator erase(const const_iterator position) noexcept
    {
        return erase_and_dispose(position, [](auto&&) noexcept {});
    }

    template <typename Disposer>
    iterator erase_and_dispose(const const_iterator position, Disposer disposer) noexcept
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(!empty());

        const iterator new_position = std::next(position).convert_to_non_const();
        intrusive_list_element<Tag>& node = *position.node_;
        remove_and_dispose_node(node, disposer);
        return new_position;
    }

    iterator erase(const const_iterator first, const const_iterator last) noexcept
    {
        return erase_and_dispose(first, last, [](auto&&) noexcept {});
    }

    template <typename Disposer>
    iterator erase_and_dispose(const_iterator first, const const_iterator last, Disposer disposer) noexcept
    {
        while (first != last)
        {
            SCORE_LANGUAGE_FUTURECPP_PRECONDITION(!empty());

            intrusive_list_element<Tag>& node = *first.node_;
            std::advance(first, 1);
            remove_and_dispose_node(node, disposer);
        }
        return last.convert_to_non_const();
    }

    void swap(intrusive_list& x) noexcept
    {
        if (this == &x)
        {
            return;
        }
        intrusive_list tmp{std::move(x)};
        x = std::move(*this);
        *this = std::move(tmp);
    }

    void clear() noexcept
    {
        clear_and_dispose([](auto&&) noexcept {});
    }

    template <typename Disposer>
    void clear_and_dispose(Disposer disposer) noexcept
    {
        while (!empty())
        {
            pop_front_and_dispose(disposer);
        }
    }

    // Suppress "AUTOSAR C++14 A16-0-1" rule finding: "The pre-processor shall only be used..."
    // Intended to be part of the interface, but not in the scope of the initial delivery.
    // coverity[autosar_cpp14_a16_0_1_violation]
#if 0
    // list operations:
    void splice(const_iterator position, intrusive_list& x) noexcept;
    void splice(const_iterator position, intrusive_list&& x) noexcept;
    void splice(const_iterator position, intrusive_list& x, const_iterator i) noexcept;
    void splice(const_iterator position, intrusive_list&& x, const_iterator i) noexcept;
    void splice(const_iterator position, intrusive_list& x,
                const_iterator first, const_iterator last) noexcept;
    void splice(const_iterator position, intrusive_list&& x,
                const_iterator first, const_iterator last) noexcept;
    // coverity[autosar_cpp14_a16_0_1_violation]
#endif

    void remove(const T& value) noexcept
    {
        remove_and_dispose(value, [](auto&&) noexcept {});
    }

    template <typename Disposer>
    void remove_and_dispose(const T& value, Disposer disposer) noexcept
    {
        remove_and_dispose_if(
            [&value](auto&& x) {
                return value == x;
            },
            disposer);
    }

    template <class Predicate>
    void remove_if(Predicate pred) noexcept
    {
        remove_and_dispose_if(pred, [](auto&&) noexcept {});
    }

    template <typename Predicate, typename Disposer>
    void remove_and_dispose_if(Predicate pred, Disposer disposer) noexcept
    {
        iterator first = begin();
        while (first != end())
        {
            if (pred(*first))
            {
                intrusive_list_element<Tag>& node = *first.node_;
                ++first;
                remove_and_dispose_node(node, disposer);
            }
            else
            {
                ++first;
            }
        }
    }

    // Suppress "AUTOSAR C++14 A16-0-1" rule finding: "The pre-processor shall only be used..."
    // Intended to be part of the interface, but not in the scope of the initial delivery.
    // coverity[autosar_cpp14_a16_0_1_violation]
#if 0
    void unique();
    template <class BinaryPredicate>
    void unique(BinaryPredicate binary_pred);

    void merge(list& x);
    void merge(list&& x);
    template <class Compare> void merge(list& x, Compare comp);
    template <class Compare> void merge(list&& x, Compare comp);

    void sort();
    template <class Compare> void sort(Compare comp);

    void reverse() noexcept;
    // coverity[autosar_cpp14_a16_0_1_violation]
#endif

    iterator iterator_to(T& x) noexcept
    {
        intrusive_list_element<Tag>& node = x;
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(node.is_linked());
        return iterator{&node};
    }

    const_iterator iterator_to(const T& x) const noexcept
    {
        const intrusive_list_element<Tag>& node = x;
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(node.is_linked());
        return const_iterator{&node};
    }

  private:
    static void insert_before(T& x, intrusive_list_element<Tag>& target) noexcept
    {
        intrusive_list_element<Tag>& node = x;
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION(!node.is_linked());
        insert_node_before(node, target);
    }

    static void insert_node_before(intrusive_list_element<Tag>& node, intrusive_list_element<Tag>& target) noexcept
    {
        node.next_ = &target;
        node.prev_ = target.prev_;
        target.prev_->next_ = &node;
        target.prev_ = &node;
    }

    template <typename Disposer>
    static void remove_and_dispose_node(intrusive_list_element<Tag>& node, Disposer disposer) noexcept
    {
        static_assert(noexcept(std::declval<Disposer>()(&std::declval<reference>())), "Disposer must be noexcept");
        node.prev_->next_ = node.next_;
        node.next_->prev_ = node.prev_;
        node.unlink();
        disposer(&static_cast<reference>(node));
    }

    // The intrusive_list is implemented as a circular doubly linked list with a sentinel (root) node.
    // Due to the circular nature of the list, the root node appears preceding the first element and at the same time
    // following the last element of the list. When the intrusive_list is empty, both the "next" and the "prev" node
    // pointers of the root node are pointing on the root node itself.
    intrusive_list_element<Tag> root_;
};

// Suppress "AUTOSAR C++14 A16-0-1" rule finding: "The pre-processor shall only be used..."
// Intended to be part of the interface, but not in the scope of the initial delivery.
// coverity[autosar_cpp14_a16_0_1_violation]
#if 0
// Comparison operators
template <class T, class Tag>
bool operator==(const intrusive_list<T,Tag>& x,
                const intrusive_list<T,Tag>& y);
template <class T, class Tag>
bool operator< (const intrusive_list<T,Tag>& x,
                const intrusive_list<T,Tag>& y);
template <class T, class Tag>
bool operator!=(const intrusive_list<T,Tag>& x,
                const intrusive_list<T,Tag>& y);
template <class T, class Tag>
bool operator> (const intrusive_list<T,Tag>& x,
                const intrusive_list<T,Tag>& y);
template <class T, class Tag>
bool operator>=(const intrusive_list<T,Tag>& x,
                const intrusive_list<T,Tag>& y);
template <class T, class Tag>
bool operator<=(const intrusive_list<T,Tag>& x,
                const intrusive_list<T,Tag>& y);
// coverity[autosar_cpp14_a16_0_1_violation]
#endif

// Specialized algorithms:
template <class T, class Tag>
void swap(intrusive_list<T, Tag>& x, intrusive_list<T, Tag>& y)
{
    x.swap(y);
}

}  // namespace containers
}  // namespace score

#endif  // SCORE_LIB_CONTAINERS_INTRUSIVE_LIST_H
