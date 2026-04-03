#include "renderer/Camera.h"
#include <algorithm>
#include <cmath>

namespace trackmini::renderer {

Camera::Camera() noexcept
  : Camera(Config{})
{
}

Camera::Camera(Config const& cfg) noexcept
  : m_cfg{ cfg }
{
    update();
}

void
Camera::orbit(float delta_yaw, float delta_pitch) noexcept
{
    m_cfg.orbit_yaw += delta_yaw;
    m_cfg.orbit_pitch += delta_pitch;

    constexpr float kMaxPitch = math::to_radians(89.f);
    m_cfg.orbit_pitch = std::clamp(m_cfg.orbit_pitch, -kMaxPitch, kMaxPitch);
}

void
Camera::zoom(float delta) noexcept
{
    m_cfg.orbit_radius -= delta;
    m_cfg.orbit_radius = std::max(m_cfg.orbit_radius, 0.5f);
}

void
Camera::pan(float dx, float dy) noexcept
{
    // moves target in the plan perpendicular to the view
    auto const forward = (m_target - m_position).normalized();
    auto const right = forward.cross(math::Vec3f::up()).normalized();
    auto const up = right.cross(forward);

    m_target = m_target + right * (-dx) + up * dy;
}

void
Camera::update() noexcept
{
    // spherical -> cartesian
    float const cos_pitch = std::cos(m_cfg.orbit_pitch);
    m_position = m_target + math::Vec3f{
        m_cfg.orbit_radius * cos_pitch * std::sin(m_cfg.orbit_yaw),
        m_cfg.orbit_radius * std::sin(m_cfg.orbit_pitch),
        m_cfg.orbit_radius * cos_pitch * std::cos(m_cfg.orbit_yaw),
    };

    m_matrices.view =
      math::Mat4f::look_at(m_position, m_target, math::Vec3f::up());
    m_matrices.proj = math::Mat4f::perspective(
      m_cfg.fov_y, m_cfg.aspect, m_cfg.near_plane, m_cfg.far_plane);
}

} // namespace trackmini::renderer