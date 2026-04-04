#include "editor/Raycast.h"
#include "renderer/Camera.h"
#include <gtest/gtest.h>

using namespace trackmini;
using namespace trackmini::editor;
using namespace trackmini::math;

static renderer::CameraMatrices
make_top_down_cam()
{
    renderer::Camera cam{ {
      .orbit_radius = 10.f,
      .orbit_pitch = math::to_radians(89.9f), // almost vertical
      .orbit_yaw = 0.f,
    } };
    cam.update();
    return cam.matrices();
}

TEST(RaycastTest, RayFromNdcOriginHitsPlane)
{
    auto cam = make_top_down_cam();
    Vec3f pos = { 0.f, 10.f, 0.f };

    Ray ray = ray_from_ndc(0.f, 0.f, cam, pos);
    auto hit = ray_plane_y(ray, 0.f);

    ASSERT_TRUE(hit.has_value());
    // center of the screen from above should touch origin
    EXPECT_NEAR(hit->x, 0.f, 0.5f);
    EXPECT_NEAR(hit->z, 0.f, 0.5f);
}

TEST(RaycastTest, RayParallelToPlaneReturnsNullopt)
{
    // horizon ray, never touch y=0
    Ray ray{ { 0.f, 5.f, 0.f }, { 1.f, 0.f, 0.f } };
    auto hit = ray_plane_y(ray, 0.f);
    EXPECT_FALSE(hit.has_value());
}

TEST(RaycastTest, RayBehindPlaneReturnsNullopt)
{
    // origin under the plane, ray pointing lower
    Ray ray{ { 0.f, -1.f, 0.f }, { 0.f, -1.f, 0.f } };
    auto hit = ray_plane_y(ray, 0.f);
    EXPECT_FALSE(hit.has_value());
}

TEST(RaycastTest, WorldToGridSnapsCorrectly)
{
    // kCellSize = 1.f
    EXPECT_EQ(world_to_grid({ 0.f, 0.f, 0.f }), (track::GridPos{ 0, 0, 0 }));
    EXPECT_EQ(world_to_grid({ 0.9f, 0.f, 0.f }), (track::GridPos{ 0, 0, 0 }));
    EXPECT_EQ(world_to_grid({ 1.f, 0.f, 0.f }), (track::GridPos{ 1, 0, 0 }));
    EXPECT_EQ(world_to_grid({ -0.1f, 0.f, 0.f }), (track::GridPos{ -1, 0, 0 }));
}

TEST(RaycastTest, GridToWorldCenterIsConsistent)
{
    track::GridPos pos{ 3, 0, -2 };
    Vec3f center = grid_to_world_center(pos);

    // reconversion give the same cell
    EXPECT_EQ(world_to_grid(center), pos);
}

TEST(RaycastTest, RayFromScreenCenterApproximatesNdcOrigin)
{
    auto cam = make_top_down_cam();
    Vec3f pos{ 0.f, 10.f, 0.1f };

    Ray from_ndc = ray_from_ndc(0.f, 0.f, cam, pos);
    Ray from_screen = ray_from_screen(640.f, 360.f, 1280, 720, cam, pos);

    // direction should be near each other
    EXPECT_NEAR(from_ndc.direction.x, from_screen.direction.x, 1e-3f);
    EXPECT_NEAR(from_ndc.direction.y, from_screen.direction.y, 1e-3f);
    EXPECT_NEAR(from_ndc.direction.z, from_screen.direction.z, 1e-3f);
}