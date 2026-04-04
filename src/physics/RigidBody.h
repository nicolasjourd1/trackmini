#ifndef RIGIDBODY_H_
#define RIGIDBODY_H_

#include "math/Math.h"

namespace trackmini::physics {

class RigidBody
{
  public:
    math::Vec3f position{ 0.f, 0.5f, 0.f };
    math::Quatf orientation{ math::Quatf::identity() };
    math::Vec3f velocity{ 0.f, 0.f, 0.f };    // m/s
    math::Vec3f angular_vel{ 0.f, 0.f, 0.f }; // rad/s

    float mass{ 180.f }; // kg
    float inv_mass{ 1.f / 180.f };

    // Diagonal interia tensor and its inverse
    // Ixx = m * (h² + w²)/12, Iyy = m*(l²-w²)/12, Izz = m*(l²+h²)/12
    math::Vec3f inertia{ 200.f, 400.f, 150.f };
    math::Vec3f inv_inertia{ 1.f / 200.f, 1.f / 400.f, 1.f / 150.f };

    // applied each tick
    float linear_drag{ 0.005f };
    float angular_drag{ 0.02f };

    // accumulated force
    // sum force and torque, integrate, and set back to zero
    math::Vec3f accumulated_force{ 0.f, 0.f, 0.f };
    math::Vec3f accumulated_torque{ 0.f, 0.f, 0.f };

    void apply_force(math::Vec3f const& f) noexcept
    {
        accumulated_force = accumulated_force + f;
    }

    void apply_force_at_point(math::Vec3f const& force,
                              math::Vec3f const& world_point)
    {
        accumulated_force = accumulated_force + force;
        accumulated_torque =
          accumulated_torque + (world_point - position).cross(force);
    }

    void apply_torque(math::Vec3f const& t) noexcept
    {
        accumulated_torque = accumulated_torque + t;
    }

    // integrate on dt seconds (Euler)
    void integrate(float dt) noexcept;

    void clear_forces() noexcept
    {
        accumulated_force = math::Vec3f::zero();
        accumulated_torque = math::Vec3f::zero();
    }

    // transform a local space vector to world space
    [[nodiscard]] math::Vec3f local_to_world_dir(
      math::Vec3f const& v) const noexcept
    {
        return orientation.rotate(v);
    }

    // transform a local point to a world point
    [[nodiscard]] math::Vec3f local_to_world_point(
      math::Vec3f const& p) const noexcept
    {
        return position + orientation.rotate(p);
    }

    // speed of a body point
    [[nodiscard]] math::Vec3f velocity_at_point(
      math::Vec3f const& world_point) const noexcept
    {
        return velocity + angular_vel.cross(world_point - position);
    }

    // local axis in world space
    [[nodiscard]] math::Vec3f forward() const noexcept
    {
        return orientation.rotate(math::Vec3f::forward());
    }
    [[nodiscard]] math::Vec3f right() const noexcept
    {
        return orientation.rotate(math::Vec3f::right());
    }
    [[nodiscard]] math::Vec3f up() const noexcept
    {
        return orientation.rotate(math::Vec3f::up());
    }

    // scalar speed in forward direction (signed)
    [[nodiscard]] float speed() const noexcept
    {
        return velocity.dot(forward());
    }

    // matrix to render
    [[nodiscard]] math::Mat4f model_matrix() const noexcept
    {
        return math::Mat4f::translation(position) * orientation.to_mat4();
    }
};

} // namespace trackmini::physics

#endif /* RIGIDBODY_H_ */
