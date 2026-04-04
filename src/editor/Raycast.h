#ifndef RAYCAST_H_
#define RAYCAST_H_

#include "math/Math.h"
#include "renderer/Camera.h"
#include "track/BlockId.h"
#include <optional>

namespace trackmini::editor {

struct Ray
{
    math::Vec3f origin;
    math::Vec3f direction; // normalized
};

// builds a ray based on normalized display coords (ndc)
// ndc_x and ndc_y in [-1, 1]
[[nodiscard]]
inline Ray
ray_from_ndc(float ndc_x,
             float ndc_y,
             renderer::CameraMatrices const& cam,
             math::Vec3f cam_pos) noexcept
{
    math::Vec4f clip{ ndc_x, ndc_y, -1.f, 1.f };

    math::Vec4f view_dir{
        clip.x / cam.proj[0][0], clip.y / cam.proj[1][1], -1.f, 0.f
    };

    auto const& v = cam.view;
    math::Vec3f world_dir{
        v[0][0] * view_dir.x + v[0][1] * view_dir.y + v[0][2] * view_dir.z,
        v[1][0] * view_dir.x + v[1][1] * view_dir.y + v[1][2] * view_dir.z,
        v[2][0] * view_dir.x + v[2][1] * view_dir.y + v[2][2] * view_dir.z,
    };

    return { cam_pos, world_dir.normalized() };
}

// builds a ray from pixel coords
[[nodiscard]]
inline Ray
ray_from_screen(float pixel_x,
                float pixel_y,
                int screen_w,
                int screen_h,
                renderer::CameraMatrices const& cam,
                math::Vec3f cam_pos)
{
    float ndc_x = (2.f * pixel_x / static_cast<float>(screen_w)) - 1.f;
    float ndc_y = 1.f - (2.f * pixel_y / static_cast<float>(screen_h));
    return ray_from_ndc(ndc_x, ndc_y, cam, cam_pos);
}

// intersects the ray with the plane y (plane_y)
// returns the intersection point or nullopt if the ray is parallel
[[nodiscard]]
inline std::optional<math::Vec3f>
ray_plane_y(Ray const& ray, float plane_y = 0.f) noexcept
{
    // parallel => no intersection
    if (std::abs(ray.direction.y) < 1e-6f)
        return std::nullopt;

    float t = (plane_y - ray.origin.y) / ray.direction.y;
    if (t < 0.f)
        return std::nullopt; // plane behind cam

    return ray.origin + ray.direction * t;
}

inline constexpr float kCellSize = 1.f;

[[nodiscard]]
inline track::GridPos
world_to_grid(math::Vec3f const& world_pos) noexcept
{
    auto snap = [](float v) -> int16_t {
        return static_cast<int16_t>(std::floor(v / kCellSize));
    };
    return { snap(world_pos.x), snap(world_pos.y), snap(world_pos.z) };
}

[[nodiscard]]
inline math::Vec3f
grid_to_world_center(track::GridPos pos) noexcept
{
    return {
        (static_cast<float>(pos.x) + 0.5f) * kCellSize,
        (static_cast<float>(pos.y) + 0.5f) * kCellSize,
        (static_cast<float>(pos.z) + 0.5f) * kCellSize,
    };
}

} // namespace trackmini::editor

#endif /* RAYCAST_H_ */
