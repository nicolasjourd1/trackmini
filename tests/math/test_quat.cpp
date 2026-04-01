#include "math/Math.h"
#include <gtest/gtest.h>

using namespace trackmini::math;
using Q = Quatf;
using V3 = Vec3f;

static void
expect_near_v3(V3 const& a, V3 const& b, float eps = 1e-5f)
{
    EXPECT_NEAR(a.x, b.x, eps);
    EXPECT_NEAR(a.y, b.y, eps);
    EXPECT_NEAR(a.z, b.z, eps);
}

static void
expect_near_q(Q const& a, Q const& b, float eps = 1e-5f)
{
    EXPECT_NEAR(a.w, b.w, eps);
    EXPECT_NEAR(a.x, b.x, eps);
    EXPECT_NEAR(a.y, b.y, eps);
    EXPECT_NEAR(a.z, b.z, eps);
}

TEST(QuatTest, IdentityRotatesNothing)
{
    V3 v{ 1, 2, 3 };
    expect_near_v3(Q::identity().rotate(v), v);
}

TEST(QuatTest, Rotate90DegAroundY)
{
    // X+ rotated 90° around Y is -Z
    Q q = Q::from_axis_angle({ 0, 1, 0 }, to_radians(90.f));
    expect_near_v3(q.rotate({ 1, 0, 0 }), { 0, 0, -1 });
}

TEST(QuatTest, Rotate180DegAroundZ)
{
    // X+ rotated 180° around Z is -X
    Q q = Q::from_axis_angle({ 0, 0, 1 }, to_radians(180.f));
    expect_near_v3(q.rotate({ 1, 0, 0 }), { -1, 0, 0 });
}

TEST(QuatTest, NormIsOneAfterConstruction)
{
    Q q = Q::from_axis_angle({ 0, 1, 0 }, to_radians(45.f));
    EXPECT_NEAR(q.norm(), 1.f, 1e-5f);
}

TEST(QuatTest, Multiplication_ComposesRotations)
{
    // 90° aroundY and 90° around Y = 180° around Y
    Q q90 = Q::from_axis_angle({ 0, 1, 0 }, to_radians(90.f));
    Q q180 = Q::from_axis_angle({ 0, 1, 0 }, to_radians(180.f));
    Q composed = q90 * q90;

    // both should make X+ X-
    expect_near_v3(composed.rotate({ 1, 0, 0 }), V3{ -1, 0, 0 });
    expect_near_v3(q180.rotate({ 1, 0, 0 }), V3{ -1, 0, 0 });
}

TEST(QuatTest, ConjugateIsInverse)
{
    Q q = Q::from_axis_angle({ 1, 1, 0 }, to_radians(60.f));
    Q qqi = q * q.conjugate();
    // q * q^-1 = identity
    expect_near_q(qqi.normalized(), Q::identity());
}

TEST(QuatTest, ToMat4ConsistentWithRotate)
{
    Q q = Q::from_axis_angle({ 0, 1, 0 }, to_radians(45.f));
    Vec4f v{ 1, 0, 0, 0 };

    Vec3f via_quat = q.rotate({ 1, 0, 0 });
    Vec4f via_mat = q.to_mat4() * v;

    EXPECT_NEAR(via_quat.x, via_mat.x, 1e-5f);
    EXPECT_NEAR(via_quat.y, via_mat.y, 1e-5f);
    EXPECT_NEAR(via_quat.z, via_mat.z, 1e-5f);
}

TEST(QuatTest, SlerpAtZeroIsA)
{
    Q a = Q::from_axis_angle({ 0, 1, 0 }, to_radians(0.f));
    Q b = Q::from_axis_angle({ 0, 1, 0 }, to_radians(90.f));
    expect_near_q(Q::slerp(a, b, 0.f), a);
}

TEST(QuatTest, SlerpAtOneIsB)
{
    Q a = Q::from_axis_angle({ 0, 1, 0 }, to_radians(0.f));
    Q b = Q::from_axis_angle({ 0, 1, 0 }, to_radians(90.f));
    expect_near_q(Q::slerp(a, b, 1.f), b);
}

TEST(QuatTest, SlerpAtHalfIs45Deg)
{
    Q a = Q::from_axis_angle({ 0, 1, 0 }, to_radians(0.f));
    Q b = Q::from_axis_angle({ 0, 1, 0 }, to_radians(90.f));
    Q mid = Q::slerp(a, b, 0.5f);
    // At 45°, X+ -> (~0.707, 0, ~-0.707)
    V3 result = mid.rotate({ 1, 0, 0 });
    EXPECT_NEAR(result.x, std::cos(to_radians(45.f)), 1e-4f);
    EXPECT_NEAR(result.z, -std::sin(to_radians(45.f)), 1e-4f);
}

TEST(QuatTest, FromEulerIdentity)
{
    Q q = Q::from_euler(0.f, 0.f, 0.f);
    expect_near_q(q, Q::identity());
}