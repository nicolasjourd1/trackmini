#include "engine/EventBus.h"
#include <gtest/gtest.h>

using namespace trackmini::engine;

TEST(EventBusTest, SubscribeAndEmit)
{
    EventBus bus;
    int call_count = 0;

        [[maybe_unused]] auto sub_quit = bus.subscribe<Events::QuitRequested>(
      [&](Events::QuitRequested const&) { ++call_count; });

    bus.emit(Events::QuitRequested{});
    EXPECT_EQ(call_count, 1);

    bus.emit(Events::QuitRequested{});
    EXPECT_EQ(call_count, 2);
}

TEST(EventBusTest, EmitWithNoSubscribersDoesNotCrash)
{
    EventBus bus;
    EXPECT_NO_FATAL_FAILURE(bus.emit(Events::QuitRequested{}));
}

TEST(EventBusTest, MultipleSubsribersAllCalled)
{
    EventBus bus;
    int a = 0, b = 0, c = 0;

        [[maybe_unused]] auto sub_a =
            bus.subscribe<Events::KeyPressed>([&](auto const&) { ++a; });
        [[maybe_unused]] auto sub_b =
            bus.subscribe<Events::KeyPressed>([&](auto const&) { ++b; });
        [[maybe_unused]] auto sub_c =
            bus.subscribe<Events::KeyPressed>([&](auto const&) { ++c; });

    bus.emit(Events::KeyPressed{ .scancode = 1, .repeat = false });

    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 1);
    EXPECT_EQ(c, 1);
}

TEST(EventBusTest, EventDataIsPassedCorrectly)
{
    EventBus bus;
    int received_scancode = -1;
    bool received_repeat = false;

    [[maybe_unused]] auto sub_key =
      bus.subscribe<Events::KeyPressed>([&](Events::KeyPressed const& e) {
          received_scancode = e.scancode;
          received_repeat = e.repeat;
      });

    bus.emit(Events::KeyPressed{ .scancode = 42, .repeat = true });

    EXPECT_EQ(received_scancode, 42);
    EXPECT_TRUE(received_repeat);
}

TEST(EventBusTest, UnsubscribeStopsReceiving)
{
    EventBus bus;
    int count = 0;

    auto id =
      bus.subscribe<Events::QuitRequested>([&](auto const&) { ++count; });

    bus.emit(Events::QuitRequested{});
    EXPECT_EQ(count, 1);

    bus.unsubscribe<Events::QuitRequested>(id);
    bus.emit(Events::QuitRequested{});
    EXPECT_EQ(count, 1); // not incremented
}

TEST(EventBusTest, DifferentEventTypesDontCross)
{
    EventBus bus;
    bool got_quit = false;
    bool got_key = false;

        [[maybe_unused]] auto sub_quit =
            bus.subscribe<Events::QuitRequested>([&](auto const&) { got_quit = true; });
        [[maybe_unused]] auto sub_key =
            bus.subscribe<Events::KeyPressed>([&](auto const&) { got_key = true; });

    bus.emit(Events::QuitRequested{});

    EXPECT_TRUE(got_quit);
    EXPECT_FALSE(got_key); // not triggered
}

TEST(EventBusTest, SubscriberCountIsAccurate)
{
    EventBus bus;
    EXPECT_EQ(bus.subscriber_count<Events::QuitRequested>(), 0);

    auto id1 = bus.subscribe<Events::QuitRequested>([](auto const&) {});
    auto id2 = bus.subscribe<Events::QuitRequested>([](auto const&) {});
    EXPECT_EQ(bus.subscriber_count<Events::QuitRequested>(), 2);

    bus.unsubscribe<Events::QuitRequested>(id1);
    EXPECT_EQ(bus.subscriber_count<Events::QuitRequested>(), 1);

    bus.unsubscribe<Events::QuitRequested>(id2);
    EXPECT_EQ(bus.subscriber_count<Events::QuitRequested>(), 0);
}