#include "engine/Clock.h"
#include <gtest/gtest.h>
#include <thread>

using namespace trackmini::engine;

TEST(FrameClockTest, FirstTickIsNearZero)
{
    FrameClock clk;
    // delta should be < 10ms
    Duration dt = clk.tick();
    EXPECT_LT(dt.count(), 0.01);
}

TEST(FrameClockTest, MeasuresElapsedTime)
{
    FrameClock clk;
    std::this_thread::sleep_for(std::chrono::milliseconds{ 50 });
    Duration dt = clk.tick();
    // 50ms with a small tolerance
    EXPECT_GT(dt.count(), 0.04);
    EXPECT_LT(dt.count(), 0.10);
}

TEST(FrameClockTest, ClampsLargeDelta)
{
    FrameClock clk;
    // simulate a freeze
    std::this_thread::sleep_for(std::chrono::milliseconds{ 300 });
    Duration dt = clk.tick();
    // Check 250ms cap
    EXPECT_LE(dt.count(), 0.251);
}

TEST(FrameClockTest, ElapsedGrowsMonotonically)
{
    FrameClock clk;
    auto e1 = clk.elapsed();
    std::this_thread::sleep_for(std::chrono::milliseconds{ 10 });
    auto e2 = clk.elapsed();
    EXPECT_GT(e2.count(), e1.count());
}

TEST(ClockConstantsTest, PhysicsDtIs10ms)
{
    // kPhysicsDt must be 0.01s
    EXPECT_DOUBLE_EQ(kPhysicsDt.count(), 0.01);
}