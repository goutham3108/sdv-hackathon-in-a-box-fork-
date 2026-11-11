#include "score/mw/com/example/vehicle-dynamics-example/com-api-gen/vehicle_gen.h"

#include <gtest/gtest.h>

#include <cstddef>
#include <string>
#include <type_traits>
#include <vector>

namespace score::mw::com
{
namespace
{

struct RecordingTraits
{
    struct Base
    {
        std::vector<std::string> event_names;
    };

    template <typename Sample>
    struct Event
    {
        using SampleType = Sample;

        Event(Base& service, const char* name)
        {
            service.event_names.emplace_back(name);
        }
    };
};

TEST(BodySignalInterfaceTest, BothDirectionsDeclareAllBodySignals)
{
    const HighBeamTx<RecordingTraits> tx;
    const VehicleHighBeamRx<RecordingTraits> rx;
    EXPECT_EQ(tx.event_names.size(), 37U);
    EXPECT_EQ(rx.event_names, tx.event_names);
    EXPECT_EQ(tx.event_names.front(), "high_beam_state");
    EXPECT_EQ(tx.event_names.back(), "row2_passenger_door_open_state");

    static_assert(std::is_same_v<decltype(tx.horn_state)::SampleType, HighBeamCommand>);
    static_assert(std::is_same_v<decltype(rx.horn_state)::SampleType, HighBeamState>);
    static_assert(std::is_same_v<decltype(rx.rear_trunk_position_state)::SampleType, HighBeamCommand>);
    static_assert(std::is_same_v<decltype(tx.front_wiping_mode_state)::SampleType, BodySignalText>);
    static_assert(std::is_same_v<decltype(rx.front_wiping_mode_state)::SampleType, BodySignalText>);
}

TEST(BodySignalInterfaceTest, SamplesMatchGatewaySharedMemoryLayouts)
{
    EXPECT_EQ(sizeof(HighBeamState), 1U);
    EXPECT_EQ(offsetof(HighBeamState, value), 0U);
    EXPECT_EQ(sizeof(HighBeamCommand), 32U);
    EXPECT_EQ(alignof(HighBeamCommand), 16U);
    EXPECT_EQ(offsetof(HighBeamCommand, data), 16U);
    EXPECT_EQ(sizeof(BodySignalText), 48U);
    EXPECT_EQ(alignof(BodySignalText), 16U);
    EXPECT_EQ(offsetof(BodySignalText, data), 16U);
    const BodySignalText text{};
    EXPECT_EQ(sizeof(text.data), 32U);
}

}
}