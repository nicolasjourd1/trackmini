#include "math/Math.h"
#include "renderer/Camera.h"
#include <gtest/gtest.h>

using namespace trackmini::renderer;
using namespace trackmini::math;

TEST(CameraTest, DefaultConstructionSucceeds)
{
    EXPECT_NO_FATAL_FAILURE(Camera{});
}

TEST(CameraTest, UpdateProducesNonZeroMatrices)
{
    Camera cam;
    cam.update();
    auto const& m = cam.matrices();
    // Projection matrix must not be zero
    EXPECT_NE(m.proj[0][0], 0.f);
    EXPECT_NE(m.proj[1][1], 0.f);
}

TEST(CameraTest, ZoomReducesRadius)
{
    Camera cam{ { .orbit_radius = 5.f } };
    cam.zoom(1.f); // zoom in
    cam.update();
    // position must be closer now
    float dist = cam.position().length();
    EXPECT_LT(dist, 5.f);
}

TEST(CameraTest, ZoomClampsAtMinimum)
{
    Camera cam{ { .orbit_radius = 0.6f } };
    cam.zoom(100.f); // extreme zoom
    cam.update();
    // stays above 0.5
    float dist = (cam.position() - cam.target()).length();
    EXPECT_GE(dist, 0.49f);
}

TEST(CameraTest, OrbitChangesPosition)
{
    Camera cam;
    cam.update();
    Vec3f before = cam.position();

    cam.orbit(0.5f, 0.f);
    cam.update();
    Vec3f after = cam.position();

    // position must have changed
    EXPECT_GT((after - before).length(), 0.01f);
}

TEST(CameraTest, PitchClampedAt89Degrees)
{
    Camera cam;
    cam.orbit(0.f, to_radians(200.f)); // excessive pitch
    cam.update();
    // camera must not have tilted (Y position bounded)
    float y = cam.position().y;
    // at pitch = 89°, y ~= radius * sin(89°) ~= radius
    float r = (cam.position() - cam.target()).length();
    EXPECT_LT(std::abs(y), r + 0.01f);
}

TEST(CameraTest, AspectRatioAffectsProjection)
{
    Camera cam16_9{ { .aspect = 16.f / 9.f } };
    Camera cam4_3{ { .aspect = 4.f / 3.f } };
    cam16_9.update();
    cam4_3.update();
    // proj[0][0] = 1/(aspect * tan(fov/2)), differs according to aspect
    EXPECT_NE(cam16_9.matrices().proj[0][0], cam4_3.matrices().proj[0][0]);
}

TEST(CameraTest, LookAtTargetOrigin)
{
    Camera cam{ { .orbit_radius = 3.f, .orbit_pitch = 0.f, .orbit_yaw = 0.f } };
    cam.update();
    // camera points to origin, forward vector should point
    // position towards (0,0,0)
    Vec3f forward = (cam.target() - cam.position()).normalized();
    EXPECT_GT(forward.length(), 0.99f);
}