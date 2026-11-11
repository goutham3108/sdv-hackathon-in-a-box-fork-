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
#include "score/os/utils/inotify/inotify_event.h"

#include <gtest/gtest.h>

namespace score
{
namespace os
{
namespace
{

class InotifyEventViewTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        auto event = reinterpret_cast<struct inotify_event*>(raw_buffer_.data());

        event->wd = watch_descriptor_;
        event->mask = mask_;
        event->cookie = cookie_;

        std::string_view name{name_};
        event->len = static_cast<std::uint32_t>(name.size() + 1);
        name.copy(event->name, name.size());
        raw_buffer_.back() = '\0';
    }

    static constexpr auto watch_descriptor_{12};
    static constexpr auto mask_{IN_CREATE};
    static constexpr auto cookie_{42U};
    static constexpr auto translated_mask_{InotifyEvent::ReadMask::kInCreate};

    static constexpr auto name_{"some_relative_path"};
    static constexpr auto null_termination_length_{1};
    static constexpr auto buffer_length_{sizeof(struct inotify_event) + std::string_view{name_}.size() +
                                         null_termination_length_};

    alignas(struct inotify_event) std::array<std::uint8_t, buffer_length_> raw_buffer_;
    struct inotify_event* inotify_event{reinterpret_cast<struct inotify_event*>(raw_buffer_.data())};
};

TEST_F(InotifyEventViewTest, CanAccessWatchDescriptor)
{
    InotifyEvent view{*inotify_event};
    EXPECT_EQ(view.GetWatchDescriptor(), InotifyWatchDescriptor{watch_descriptor_});
}

TEST_F(InotifyEventViewTest, CanAccessMask)
{
    InotifyEvent view{*inotify_event};
    EXPECT_EQ(view.GetMask(), translated_mask_);
}

TEST_F(InotifyEventViewTest, CanAccessCookie)
{
    InotifyEvent view{*inotify_event};
    EXPECT_EQ(view.GetCookie(), cookie_);
}

TEST_F(InotifyEventViewTest, CanAccessName)
{
    InotifyEvent view{*inotify_event};
    EXPECT_EQ(view.GetName(), std::string_view{name_});
}

TEST_F(InotifyEventViewTest, TranslatesInAccessCorrectly)
{
    inotify_event->mask = IN_ACCESS;
    InotifyEvent view{*inotify_event};
    EXPECT_EQ(view.GetMask(), InotifyEvent::ReadMask::kInAccess);
}

TEST_F(InotifyEventViewTest, TranslatesInMovedToCorrectly)
{
    inotify_event->mask = IN_MOVED_TO;
    InotifyEvent view{*inotify_event};
    EXPECT_EQ(view.GetMask(), InotifyEvent::ReadMask::kInMovedTo);
}

TEST_F(InotifyEventViewTest, TranslatesInCreateCorrectly)
{
    inotify_event->mask = IN_CREATE;
    InotifyEvent view{*inotify_event};
    EXPECT_EQ(view.GetMask(), InotifyEvent::ReadMask::kInCreate);
}

TEST_F(InotifyEventViewTest, TranslatesInDeleteCorrectly)
{
    inotify_event->mask = IN_DELETE;
    InotifyEvent view{*inotify_event};
    EXPECT_EQ(view.GetMask(), InotifyEvent::ReadMask::kInDelete);
}

TEST_F(InotifyEventViewTest, TranslatesInIgnoredCorrectly)
{
    inotify_event->mask = IN_IGNORED;
    InotifyEvent view{*inotify_event};
    EXPECT_EQ(view.GetMask(), InotifyEvent::ReadMask::kInIgnored);
}

TEST_F(InotifyEventViewTest, TranslatesInIsdirCorrectly)
{
    inotify_event->mask = IN_ISDIR;
    InotifyEvent view{*inotify_event};
    EXPECT_EQ(view.GetMask(), InotifyEvent::ReadMask::kInIsDir);
}

TEST_F(InotifyEventViewTest, TranslatesInQOverflowCorrectly)
{
    inotify_event->mask = IN_Q_OVERFLOW;
    InotifyEvent view{*inotify_event};
    EXPECT_EQ(view.GetMask(), InotifyEvent::ReadMask::kInQOverflow);
}

TEST_F(InotifyEventViewTest, ConstructorDoesNotInitializeNameWhenLengthIsZero)
{
    InotifyEvent view1{*inotify_event};
    EXPECT_EQ(view1.GetName(), std::string_view(name_));

    inotify_event->len = 0U;
    InotifyEvent view2{*inotify_event};
    EXPECT_EQ(view1.GetName(), std::string_view(name_));
}

TEST_F(InotifyEventViewTest, ComparisonWithDifferentWatchDescriptor)
{
    InotifyEvent lhs{*inotify_event};
    inotify_event->wd = 2;
    InotifyEvent rhs{*inotify_event};
    EXPECT_FALSE(lhs == rhs);
}

TEST_F(InotifyEventViewTest, ComparisonWithDifferentMask)
{
    InotifyEvent lhs{*inotify_event};
    inotify_event->mask = IN_ACCESS;
    InotifyEvent rhs{*inotify_event};
    EXPECT_FALSE(lhs == rhs);
}

TEST_F(InotifyEventViewTest, ComparisonWithDifferentCookie)
{
    InotifyEvent lhs{*inotify_event};
    inotify_event->cookie = 2;
    InotifyEvent rhs{*inotify_event};
    EXPECT_FALSE(lhs == rhs);
}

TEST_F(InotifyEventViewTest, ComparisonWithDifferentName)
{
    InotifyEvent lhs{*inotify_event};
    std::strcpy(inotify_event->name, "ab");
    InotifyEvent rhs{*inotify_event};
    EXPECT_FALSE(lhs == rhs);
}

TEST_F(InotifyEventViewTest, ComparisonAllDifferent)
{
    InotifyEvent lhs{*inotify_event};
    inotify_event->mask = IN_ACCESS;
    inotify_event->wd = 2;
    inotify_event->cookie = 2;
    std::strcpy(inotify_event->name, "ab");
    InotifyEvent rhs{*inotify_event};
    EXPECT_FALSE(lhs == rhs);
}

TEST_F(InotifyEventViewTest, EventEquals)
{
    InotifyEvent lhs{*inotify_event};
    InotifyEvent rhs{*inotify_event};
    EXPECT_TRUE(lhs == rhs);
}

}  // namespace
}  // namespace os
}  // namespace score
