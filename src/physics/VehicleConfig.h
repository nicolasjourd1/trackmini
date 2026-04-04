#ifndef VEHICLECONFIG_H_
#define VEHICLECONFIG_H_

#include "physics/WheelRaycast.h"
#include <array>

namespace trackmini::physics {

struct VehicleConfig
{
    // Mass and inertia of the chassis
    float mass{ 180.f };
    math::Vec3f inertia{ 200.f, 400.f, 150.f };

    // Engine
    float max_engine_torque{ 800.f };  // N*m
    float max_brake_torque{ 2000.f };  // N*m (btake)
    float max_steer_angle{ 0.45f };    // rad (~26°)
    float steer_speed{ 2.5f };         // rad/s (steerign speed)
    float engine_brake_coeff{ 0.15f }; // motor friction when released

    // Wheels : FL, FR, RL, RR
    // (Front Left, Front Right, Rear Left, Rear Right)
    std::array<WheelConfig, 4> wheels{ {
      // FL
      {
        .local_anchor = { -0.75f, 0.f, 1.3f },
        .suspension_rest = 0.5f,
        .spring_stiffness = 28000.f,
        .damper_coeff = 2200.f,
        .grip_lateral = 14.f,
      },
      // FR
      {
        .local_anchor = { 0.75f, 0.f, 1.3f },
        .suspension_rest = 0.5f,
        .spring_stiffness = 28000.f,
        .damper_coeff = 2200.f,
        .grip_lateral = 14.f,
      },
      // RL
      {
        .local_anchor = { -0.75f, 0.f, -1.3f },
        .suspension_rest = 0.5f,
        .spring_stiffness = 26000.f,
        .damper_coeff = 2000.f,
        .grip_lateral = 12.f,
      },
      // RR
      {
        .local_anchor = { 0.75f, 0.f, -1.3f },
        .suspension_rest = 0.5f,
        .spring_stiffness = 26000.f,
        .damper_coeff = 2000.f,
        .grip_lateral = 12.f,
      },
    } };
};

} // namespace trackmini::physics

#endif /* VEHICLECONFIG_H_ */
