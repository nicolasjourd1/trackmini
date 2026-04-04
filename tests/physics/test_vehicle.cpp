#include "physics/Vehicle.h"
#include <gtest/gtest.h>

using namespace trackmini::physics;
using namespace trackmini::math;

static constexpr float kDt = 0.01f;
static constexpr float kFloorY = 0.f;

static void
simulate(Vehicle& v, VehicleInput const& input, int ticks)
{
    for (int i = 0; i < ticks; ++i)
        v.fixed_update(input, kDt, kFloorY);
}

TEST(VehicleTest, DefaultVehicleIsAtRest)
{
    Vehicle v;
    EXPECT_NEAR(v.speed_kmh(), 0.f, 0.1f);
}

TEST(VehicleTest, ThrottleAcceleratesForward)
{
    Vehicle v;
    v.reset({ 0.f, 1.f, 0.f });

    VehicleInput input{ .throttle = 1.f };
    simulate(v, input, 200);

    EXPECT_GT(v.speed_kmh(), 5.f);
}

TEST(VehicleTest, BrakeDeceleratesVehicle)
{
    Vehicle v;
    v.reset({ 0.f, 1.f, 0.f });

    simulate(v, { .throttle = 1.f }, 300);
    float speed_before = v.speed_kmh();

    simulate(v, { .brake = 1.f }, 200);
    EXPECT_LT(v.speed_kmh(), speed_before);
}

TEST(VehicleTest, SteeringChangesHeading)
{
    Vehicle v;
    v.reset({ 0.f, 1.f, 0.f });

    simulate(v, { .throttle = 1.f }, 100);
    Vec3f forward_before = v.body().forward();

    simulate(v, { .throttle = 0.8f, .steer = 1.f }, 100);
    Vec3f forward_after = v.body().forward();

    float dot = forward_before.dot(forward_after);
    EXPECT_LT(dot, 0.999f);
}

TEST(VehicleTest, VehicleStaysGroundedOnFlatFloor)
{
    Vehicle v;
    v.reset({ 0.f, 1.f, 0.f });

    simulate(v, { .throttle = 1.f }, 500);

    EXPECT_GT(v.body().position.y, 0.2f);
    EXPECT_LT(v.body().position.y, 3.f);
}

TEST(VehicleTest, GroundedWheelsNonZeroOnFlatFloor)
{
    Vehicle v;
    v.reset({ 0.f, 1.f, 0.f });

    simulate(v, {}, 50);
    EXPECT_GT(v.grounded_wheels(), 0);
}

TEST(VehicleTest, ResetRestoresState)
{
    Vehicle v;
    v.reset({ 0.f, 1.f, 0.f });
    simulate(v, { .throttle = 1.f }, 500);

    v.reset({ 0.f, 1.f, 0.f });
    EXPECT_NEAR(v.speed_kmh(), 0.f, 0.1f);
    EXPECT_NEAR(v.body().velocity.x, 0.f, 1e-5f);
    EXPECT_NEAR(v.body().velocity.z, 0.f, 1e-5f);
}

TEST(VehicleTest, SteerAngleIsBounded)
{
    Vehicle v;
    v.reset({ 0.f, 1.f, 0.f });

    simulate(v, { .throttle = 0.f, .steer = 1.f }, 1000);
    EXPECT_LE(v.steer_angle(), v.config().max_steer_angle + 1e-4f);
}

TEST(VehicleTest, VehicleDoesNotSinkBelowFloor)
{
    Vehicle v;
    v.reset({ 0.f, 10.f, 0.f });
    simulate(v, {}, 500);
    EXPECT_GE(v.body().position.y, kFloorY);
}