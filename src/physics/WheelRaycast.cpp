#include "physics/WheelRaycast.h"
#include "physics/RigidBody.h"
#include <algorithm>
#include <cmath>

namespace trackmini::physics {

WheelState
cast_wheel(WheelConfig const& cfg,
           RigidBody const& body,
           float drive_torque,
           float steer_angle,
           float floor_y,
           float dt) noexcept
{
    WheelState state;

    // anchor's position in world space
    math::Vec3f const anchor = body.local_to_world_point(cfg.local_anchor);

    // raycast downwards
    float const max_len = cfg.suspension_rest + cfg.suspension_travel;
    float const hit_dist = anchor.y - floor_y; // distance to ground

    if (hit_dist > max_len || hit_dist < 0.f) {
        // the wheel is in the air
        return state;
    }

    state.in_contact = true;
    state.contact_dist = hit_dist;
    state.compression =
      std::clamp(cfg.suspension_rest - hit_dist, 0.f, cfg.suspension_travel);
    state.contact_point = { anchor.x, floor_y, anchor.z };
    state.contact_normal = { 0.f, 1.f, 0.f };

    // suspension force (spring + shock absorber)
    math::Vec3f const vel_at_anchor = body.velocity_at_point(anchor);
    float const compress_vel = vel_at_anchor.dot(state.contact_normal);

    float const spring_force = cfg.spring_stiffness * state.compression;
    float const damper_force = cfg.damper_coeff * (-compress_vel);

    float const susp_magnitude = std::max(0.f, spring_force + damper_force);
    state.suspension_force = state.contact_normal * susp_magnitude;

    // friction forces
    // in the wheel reference
    // - forward_wheel : direction of traval of the wheel (takes steering into
    // account)
    // - lateral_wheel : perpendicular to the forward direction in the
    // horizontal plane

    math::Vec3f const body_forward = body.forward();
    math::Vec3f const body_right = body.right();

    // apply steering
    math::Vec3f const wheel_forward =
      body_forward * std::cos(steer_angle) + body_right * std::sin(steer_angle);
    math::Vec3f const wheel_lateral =
      body_right * std::cos(steer_angle) - body_forward * std::sin(steer_angle);

    math::Vec3f const vel_contact = body.velocity_at_point(state.contact_point);

    math::Vec3f const vel_planar =
      vel_contact -
      state.contact_normal * vel_contact.dot(state.contact_normal);

    float const vel_longitudinal = vel_planar.dot(wheel_forward);
    float const vel_lateral = vel_planar.dot(wheel_lateral);

    // longitudinal force = traction and friction
    float const drive_force_mag = drive_torque / cfg.radius;
    // friction opposing longitudinal slip
    float const long_friction = -vel_longitudinal * cfg.grip_longitudinal;
    float const total_long = drive_force_mag + long_friction;

    // lateral force : opposes lateral slip
    float const lat_friction = -vel_lateral * cfg.grip_lateral;

    float const normal_force = susp_magnitude;
    float const max_friction = normal_force * 1.5f;

    float long_clamped = std::clamp(total_long, -max_friction, max_friction);
    float lat_clamped = std::clamp(lat_friction, -max_friction, max_friction);

    state.friction_force =
      wheel_forward * long_clamped + wheel_lateral * lat_clamped;

    return state;
}

} // namespace trackmini::physics