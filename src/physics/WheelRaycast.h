#ifndef WHEELRAYCAST_H_
#define WHEELRAYCAST_H_

#include "math/Math.h"

namespace trackmini::physics {

class RigidBody;

struct WheelConfig
{
    math::Vec3f local_anchor;          // vehicle position in local space
    float suspension_rest{ 0.5f };     // spring rest length (m)
    float suspension_travel{ 0.3f };   // maximum travel (m)
    float spring_stiffness{ 25000.f }; // N/m
    float damper_coeff{ 2000.f };      // N*s/m
    float grip_lateral{ 12.f };        // lateral grip coefficient
    float grip_longitudinal{ 10.f };   // longitudinal grip coefficient
    float radius{ 0.3f };              // wheel radius (for rendering)
};

struct WheelState
{
    bool in_contact{ false };
    float compression{ 0.f };  // in [0, suspension_travel]
    float contact_dist{ 0.f }; // actual measured ground distance
    math::Vec3f contact_point{};
    math::Vec3f contact_normal{ 0.f, 1.f, 0.f };

    // forces computed this tick
    math::Vec3f suspension_force{};
    math::Vec3f friction_force{};
};

// raycasts a wheel and computes forces
// flooy_r : ground plane height (will be block grid later)
WheelState
cast_wheel(WheelConfig const& cfg,
           RigidBody const& body,
           float drive_torque, // motor torque applied to this wheel (N*m)
           float steer_angle,  // steering angle (rad), 0 for back wheels
           float floor_y,      // ground height
           float dt) noexcept;

} // namespace trackmini::physics

#endif /* WHEELRAYCAST_H_ */
