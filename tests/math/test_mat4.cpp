#include "math/Math.h"
#include <gtest/gtest.h>

using namespace trackmini::math;
using M = Mat4f;
using V3 = Vec3f;
using V4 = Vec4f;

static void
expect_near_v4(V4 const& a, V4 const& b, float eps = 1e-4f)
{
    EXPECT_NEAR(a.x, b.x, eps);
    EXPECT_NEAR(a.y, b.y, eps);
    EXPECT_NEAR(a.z, b.z, eps);
    EXPECT_NEAR(a.w, b.w, eps);
}

static void
expect_near_mat(M const& a, M const& b, float eps = 1e-4f)
{
    for (int c = 0; c < 4; ++c)
        expect_near_v4(a[c], b[c], eps);
}

TEST(Mat4Test, IdentityTimesVecIsVec)
{
    V4 v{ 1, 2, 3, 1 };
    expect_near_v4(M::identity() * v, v);
}

TEST(Mat4Test, IdentityTimesMatIsIdent)
{
    expect_near_mat(M::identity() * M::identity(), M::identity());
}

TEST(Mat4Test, Translation)
{
    auto m = M::translation({ 1, 2, 3 });
    V4 point{ 0, 0, 0, 1 }; // w=1 : point
    expect_near_v4(m * point, { 1, 2, 3, 1 });

    V4 dir{ 1, 0, 0, 0 }; // w=0 : direction, no effect ontranslation
    expect_near_v4(m * dir, { 1, 0, 0, 0 });
}

TEST(Mat4Test, Scale)
{
    auto m = M::scale({ 2, 3, 4 });
    expect_near_v4(m * V4{ 1, 1, 1, 1 }, { 2, 3, 4, 1 });
}

TEST(Mat4Test, RotationAroundY90Deg)
{
    // Rotating X of 90° around Y should be -Z
    auto m = M::rotation(V3{ 0, 1, 0 }, to_radians(90.f));
    V4 x_axis{ 1, 0, 0, 0 };
    auto result = m * x_axis;
    EXPECT_NEAR(result.x, 0.f, 1e-5f);
    EXPECT_NEAR(result.y, 0.f, 1e-5f);
    EXPECT_NEAR(result.z, -1.f, 1e-5f);
}

TEST(Mat4Test, Transposed)
{
    M m{
        { 1, 2, 3, 4 },
        { 5, 6, 7, 8 },
        { 9, 10, 11, 12 },
        { 13, 14, 15, 16 },
    };
    auto t = m.transposed();
    // line 0 of m becomes column 0 of t
    EXPECT_NEAR(t[0][0], m[0][0], 1e-6f);
    EXPECT_NEAR(t[1][0], m[0][1], 1e-6f);
    EXPECT_NEAR(t[0][1], m[1][0], 1e-6f);
}

TEST(Mat4Test, TransposeOfTransposeIsOriginal)
{
    M m{
        { 1, 2, 3, 4 }, { 5, 6, 7, 8 }, { 9, 10, 11, 12 }, { 13, 14, 15, 16 }
    };
    expect_near_mat(m.transposed().transposed(), m);
}

TEST(Mat4Test, TRSComposition)
{
    // T * R * S on origin is a translation
    auto t = M::translation({ 5, 0, 0 });
    auto r = M::rotation(V3{ 0, 1, 0 }, 0.f);
    auto s = M::scale({ 1, 1, 1 });
    auto trs = t * r * s;
    expect_near_v4(trs * V4{ 0, 0, 0, 1 }, { 5, 0, 0, 1 });
}

TEST(Mat4Test, PerspectiveMatrixNonZero)
{
    // checks perspective matrix isnt 0
    auto p = M::perspective(to_radians(60.f), 16.f / 9.f, 0.1f, 1000.f);
    EXPECT_NE(p[0][0], 0.f);
    EXPECT_NE(p[1][1], 0.f);
}

TEST(Mat4Test, LookAtTransformsPointsCorrectly)
{
    auto v = M::look_at({ 0, 0, 0 }, { 0, 0, -1 }, { 0, 1, 0 });

    // a point just in front of the cam stay on the Z- axis in cam space
    V4 front{ 0, 0, -1, 1 };
    auto result = v * front;
    EXPECT_NEAR(result.x, 0.f, 0.0001f);
    EXPECT_NEAR(result.y, 0.f, 0.0001f);
    EXPECT_NEAR(result.z, -1.f, 0.0001f);

    // a point on the right (X+) stays on right
    V4 right{ 1, 0, 0, 1 };
    auto r2 = v * right;
    EXPECT_NEAR(r2.x, 1.f, 0.0001f);
    EXPECT_NEAR(r2.y, 0.f, 0.0001f);
    EXPECT_NEAR(r2.z, 0.f, 0.0001f);

    // world origin is still camera origin
    V4 origin{ 0, 0, 0, 1 };
    auto r3 = v * origin;
    EXPECT_NEAR(r3.x, 0.f, 0.0001f);
    EXPECT_NEAR(r3.y, 0.f, 0.0001f);
    EXPECT_NEAR(r3.z, 0.f, 0.0001f);
}