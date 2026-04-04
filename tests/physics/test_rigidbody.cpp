#include "physics/RigidBody.h"
#include <gtest/gtest.h>

using namespace trackmini::physics;
using namespace trackmini::math;

static constexpr float kDt = 0.01f;

TEST(RigidBodyTest, DefaultStateIsAtRest)
{
    RigidBody rb;
    EXPECT_EQ(rb.velocity, Vec3f::zero());
    EXPECT_EQ(rb.angular_vel, Vec3f::zero());
}

TEST(RigidBodyTest, GravityAcceleratesDownward)
{
    RigidBody rb;
    rb.position = { 0.f, 100.f, 0.f };
    float vy_before = rb.velocity.y;
    rb.integrate(kDt);
    EXPECT_LT(rb.velocity.y, vy_before);
}

TEST(RigidBodyTest, AppliedForceChangesVelocity)
{
    RigidBody rb;
    rb.apply_force({ 100.f, 0.f, 0.f });
    float vx_before = rb.velocity.x;
    rb.integrate(kDt);
    EXPECT_GT(rb.velocity.x, vx_before);
}

TEST(RigidBodyTest, ForceAtPointGeneratesTorque)
{
    RigidBody rb;
    rb.apply_force_at_point({ 0.f, 100.f, 0.f }, { 0.f, 0.f, 2.f });
    Vec3f torque_before = rb.accumulated_torque;
    EXPECT_NE(rb.accumulated_torque.x, 0.f);
}

TEST(RigidBodyTest, ClearForcesResetsAccumulators)
{
    RigidBody rb;
    rb.apply_force({ 1.f, 2.f, 3.f });
    rb.apply_torque({ 4.f, 5.f, 6.f });
    rb.clear_forces();
    EXPECT_EQ(rb.accumulated_force, Vec3f::zero());
    EXPECT_EQ(rb.accumulated_torque, Vec3f::zero());
}

TEST(RigidBodyTest, IntegrateMovesSolidInDirectionOfVelocity)
{
    RigidBody rb;
    rb.velocity = { 0.f, 0.f, 5.f };
    Vec3f pos_before = rb.position;
    rb.integrate(kDt);
    EXPECT_GT(rb.position.z, pos_before.z);
}

TEST(RigidBodyTest, LocalToWorldDirWithIdentityIsUnchanged)
{
    RigidBody rb;
    Vec3f v{ 1.f, 0.f, 0.f };
    auto result = rb.local_to_world_dir(v);
    EXPECT_NEAR(result.x, v.x, 1e-5f);
    EXPECT_NEAR(result.y, v.y, 1e-5f);
    EXPECT_NEAR(result.z, v.z, 1e-5f);
}

TEST(RigidBodyTest, OrientationRemainsNormalizedAfterManyTicks)
{
    RigidBody rb;
    rb.apply_torque({ 0.f, 10.f, 0.f });
    for (int i = 0; i < 500; ++i) {
        rb.apply_torque({ 0.1f, 0.2f, 0.05f });
        rb.integrate(kDt);
        rb.clear_forces();
    }
    float norm = rb.orientation.norm();
    EXPECT_NEAR(norm, 1.f, 1e-4f);
}

TEST(RigidBodyTest, DragEventuallyStopsBody)
{
    RigidBody rb;
    rb.velocity = { 100.f, 0.f, 0.f };
    for (int i = 0; i < 10000; ++i) {
        rb.integrate(kDt);
        rb.clear_forces();
    }
    EXPECT_LT(std::abs(rb.velocity.x), 1.f);
}

TEST(RigidBodyTest, ModelMatrixPositionMatchesBodyPosition)
{
    RigidBody rb;
    rb.position = { 3.f, 1.f, -2.f };
    auto m = rb.model_matrix();
    EXPECT_NEAR(m[3][0], 3.f, 1e-5f);
    EXPECT_NEAR(m[3][1], 1.f, 1e-5f);
    EXPECT_NEAR(m[3][2], -2.f, 1e-5f);
}