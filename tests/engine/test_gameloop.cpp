#include "engine/GameLoop.h"
#include <gtest/gtest.h>

using namespace trackmini::engine;

auto
make_fixed_tick(Duration dt)
{
    return [dt]() -> Duration { return dt; };
}

TEST(GameLoopTest, RunsFixedTicksAtCorrectRate)
{
    int fixed_ticks = 0;
    int frames = 0;
    double total_sim = 0.0;

    // simulate 10 frames
    constexpr int kTargetFrames = 10;
    constexpr int kExpectedTicksTotal = 30;

    GameLoopCallbacks cb{
        .fixed_update =
          [&](Duration dt) {
              ++fixed_ticks;
              total_sim += dt.count();
          },
        .update = [&](Duration, double) { ++frames; },
        .render = [](double) {},
        .is_running = [&]() { return frames < kTargetFrames; },
        .poll_events = []() {},
        .tick_fn = make_fixed_tick(Duration{ 0.03 }), // 30ms simulation
    };

    GameLoop loop{ cb };
    loop.run();

    EXPECT_EQ(loop.stats().total_frames,
              static_cast<std::size_t>(kTargetFrames));
    EXPECT_EQ(fixed_ticks, kExpectedTicksTotal);

    // each tick received kPhysicsDt
    EXPECT_NEAR(total_sim / fixed_ticks, kPhysicsDt.count(), 1e-9);
}

TEST(GameLoopTest, StopsWhenIsRunningReturnsFalse)
{
    int frames = 0;

    GameLoopCallbacks cb{ .fixed_update = [](Duration) {},
                          .update = [&](Duration, double) { ++frames; },
                          .render = [](double) {},
                          .is_running = [&]() { return frames < 5; },
                          .poll_events = []() {},
                          .tick_fn = nullptr };

    GameLoop loop{ cb };
    loop.run();

    EXPECT_EQ(frames, 5);
}

TEST(GameLoopTest, AlphaIsInUnitInterval)
{
    // alpha in [0, 1)
    bool alpha_valid = true;
    int frames = 0;

    GameLoopCallbacks cb{ .fixed_update = [](Duration) {},
                          .update =
                            [&](Duration, double alpha) {
                                if (alpha < 0.0 || alpha >= 1.0)
                                    alpha_valid = false;
                                ++frames;
                            },
                          .render = [](double) {},
                          .is_running = [&]() { return frames < 20; },
                          .poll_events = []() {},
                          .tick_fn = nullptr };

    GameLoop loop{ cb };
    loop.run();

    EXPECT_TRUE(alpha_valid);
}

TEST(GameLoopTest, StatsArePopulatedAfterRun)
{
    int frames = 0;

    GameLoopCallbacks cb{ .fixed_update = [](Duration) {},
                          .update = [&](Duration, double) { ++frames; },
                          .render = [](double) {},
                          .is_running = [&]() { return frames < 10; },
                          .poll_events = []() {},
                          .tick_fn = nullptr };

    GameLoop loop{ cb };
    loop.run();

    auto s = loop.stats();
    EXPECT_EQ(s.total_frames, 10u);
    EXPECT_GT(s.total_time.count(), 0.0);
    EXPECT_GT(s.average_fps, 0.0);
}