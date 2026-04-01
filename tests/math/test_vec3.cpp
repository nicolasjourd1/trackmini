#include "math/Math.h"
#include <cmath>
#include <gtest/gtest.h>

using namespace trackmini::math;
using V = Vec3f;

static void
expect_near_v3(V const& a, V const& b, float eps = 1e-5f)
{
    EXPECT_NEAR(a.x, b.x, eps);
    EXPECT_NEAR(a.y, b.y, eps);
    EXPECT_NEAR(a.z, b.z, eps);
}

TEST(Vec3Test, DefaultConstructedIsZero)
{
    V v;
    EXPECT_EQ(v.x, 0.f);
    EXPECT_EQ(v.y, 0.f);
    EXPECT_EQ(v.z, 0.f);
}

TEST(Vec3Test, Addition)
{
    expect_near_v3(V{ 1, 2, 3 } + V{ 4, 5, 6 }, V{ 5, 7, 9 });
}

TEST(Vec3Test, Subtraction)
{
    expect_near_v3(V{ 5, 7, 9 } - V{ 4, 5, 6 }, V{ 1, 2, 3 });
}

TEST(Vec3Test, ScalarMultiplication)
{
    expect_near_v3(V{ 1, 2, 3 } * 2.f, V{ 2, 4, 6 });
    expect_near_v3(2.f * V{ 1, 2, 3 }, V{ 2, 4, 6 });
}

TEST(Vec3Test, Negation)
{
    expect_near_v3(-V{ 1, -2, 3 }, V{ -1, 2, -3 });
}

TEST(Vec3Test, DotProduct)
{
    EXPECT_NEAR((V{ 1, 0, 0 }.dot(V{ 0, 1, 0 })), 0.f, 1e-6f);  // perpendicular
    EXPECT_NEAR((V{ 1, 0, 0 }.dot(V{ 1, 0, 0 })), 1.f, 1e-6f);  // parallel
    EXPECT_NEAR((V{ 1, 2, 3 }.dot(V{ 4, 5, 6 })), 32.f, 1e-5f); // 4+10+18
}

TEST(Vec3Test, CrossProduct)
{
    // X x Y = Z
    expect_near_v3(V::right().cross(V{ 0, 1, 0 }), V{ 0, 0, 1 });
    // Y x X = -Z
    expect_near_v3(V{ 0, 1, 0 }.cross(V::right()), V{ 0, 0, -1 });
    // Cross vector with itself = 0
    expect_near_v3(V{ 1, 2, 3 }.cross(V{ 1, 2, 3 }), V::zero());
}

TEST(Vec3Test, Length)
{
    EXPECT_NEAR((V{ 3, 4, 0 }.length()), 5.0f, 1e-5f);
    EXPECT_NEAR((V{ 1, 0, 0 }.length()), 1.0f, 1e-6f);
    EXPECT_NEAR(V::zero().length(), 0.f, 1e-6f);
}

TEST(Vec3Test, Normalized)
{
    V n = V{ 3, 4, 0 }.normalized();
    EXPECT_NEAR(n.length(), 1.f, 1e-5f);
    // normalizing a null vector returns a null vector without crash
    expect_near_v3(V::zero().normalized(), V::zero());
}

TEST(Vec3Test, Lerp)
{
    expect_near_v3(V::lerp(V{ 0, 0, 0 }, V{ 1, 1, 1 }, 0.5f),
                   V{ 0.5f, 0.5f, 0.5f });
    expect_near_v3(V::lerp(V{ 0, 0, 0 }, V{ 1, 1, 1 }, 0.f), V::zero());
    expect_near_v3(V::lerp(V{ 0, 0, 0 }, V{ 1, 1, 1 }, 1.f), V::one());
}

TEST(Vec3Test, Reflect)
{
    // a downward vector reflected by Y+ is reflected upward
    V incident{ 0, -1, 0 };
    V normal{ 0, 1, 0 };
    expect_near_v3(incident.reflect(normal), V{ 0, 1, 0 });
}

TEST(Vec3Test, IndexedAccess)
{
    V v{ 1, 2, 3 };
    EXPECT_EQ(v[0], 1.f);
    EXPECT_EQ(v[1], 2.f);
    EXPECT_EQ(v[2], 3.f);
    v[1] = 9.f;
    EXPECT_EQ(v.y, 9.f);
}

TEST(Vec3Test, CompoundAssignment)
{
    V v{ 1, 2, 3 };
    v += V{ 1, 1, 1 };
    expect_near_v3(v, V{ 2, 3, 4 });
    v *= 2.f;
    expect_near_v3(v, V{ 4, 6, 8 });
}