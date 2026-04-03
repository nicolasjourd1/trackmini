#ifndef CAMERA_H_
#define CAMERA_H_

#include "math/Math.h"

namespace trackmini::renderer {

struct alignas(16) CameraMatrices
{
    math::Mat4f view;
    math::Mat4f proj;
};

// perspective cam with orbital control (arcball)
// editor cam, not car cam

class Camera
{
  public:
    struct Config
    {
        float fov_y = math::to_radians(60.f);
        float aspect = 16.f / 9.f;
        float near_plane = 0.1f;
        float far_plane = 1000.f;
        // initial pos (spheric)
        float orbit_radius = 5.f;
        float orbit_pitch = math::to_radians(20.f);
        float orbit_yaw = math::to_radians(45.f);
    };

    Camera() noexcept;
    explicit Camera(Config const& cfg) noexcept;

    // orbital control
    void orbit(float delta_yaw, float delta_pitch) noexcept;
    void zoom(float delta) noexcept;       // changes orbit_radius
    void pan(float dx, float dy) noexcept; // moves target

    // must be called when the aspect ratio changes
    // e.g when the window is resized
    void set_aspect(float aspect) noexcept { m_cfg.aspect = aspect; }

    // recompute matrices from orbit params
    void update() noexcept;

    [[nodiscard]] CameraMatrices const& matrices() const noexcept
    {
        return m_matrices;
    }
    [[nodiscard]] math::Vec3f position() const noexcept { return m_position; }
    [[nodiscard]] math::Vec3f target() const noexcept { return m_target; }

  private:
    Config m_cfg;
    math::Vec3f m_target{ 0, 0, 0 };   // point looked
    math::Vec3f m_position{ 0, 0, 0 }; // position calculated from spherics
    CameraMatrices m_matrices{};
};

} // namespace trackmini::renderer

#endif /* CAMERA_H_ */
