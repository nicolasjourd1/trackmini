#include "physics/RigidBody.h"
#include <cmath>

namespace trackmini::physics {

void
RigidBody::integrate(float dt) noexcept
{
    // === linear integration (euler) ===

    // 1 : acceleration
    math::Vec3f accel = accumulated_force * inv_mass;

    // 2 : gravity
    accel = accel + math::Vec3f{ 0.f, -9.81f, 0.f };

    // 3 : update speed
    velocity = velocity + accel * dt;

    // 4 : linear drag (reduces speed at each tick)
    velocity = velocity * (1.f - linear_drag);

    // 5 : update position
    position = position + velocity * dt;

    // === angular integration ===

    // torque in local space
    math::Vec3f local_torque{ orientation.conjugate().rotate(
      accumulated_torque) };

    // angular accel in local space
    math::Vec3f local_alpha{
        local_torque.x * inv_inertia.x,
        local_torque.y * inv_inertia.y,
        local_torque.z * inv_inertia.z,
    };

    // angular accel in world space
    math::Vec3f alpha = orientation.rotate(local_alpha);

    // update angular speed
    angular_vel = angular_vel + alpha * dt;
    angular_vel = angular_vel * (1.f - angular_drag);

    // update orientation by integrating quat
    // dq/dt = 0.5 * omega_quat * q
    // omega_quat = quat(°, omega_x, omega_y, omega_z)
    math::Quatf omega_quat{
        0.f,
        angular_vel.x * 0.5f * dt,
        angular_vel.y * 0.5f * dt,
        angular_vel.z * 0.5f * dt,
    };
    orientation = (orientation + omega_quat * orientation).normalized();
}

} // namespace trackmini::physics