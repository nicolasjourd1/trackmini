#include "physics/Vehicle.h"
#include <cmath>
#include <numeric>

namespace trackmini::physics {

Vehicle::Vehicle(VehicleConfig cfg) noexcept
  : m_cfg(std::move(cfg))
{
    m_body.mass = m_cfg.mass;
    m_body.inv_mass = 1.f / m_cfg.mass;
    m_body.inertia = m_cfg.inertia;
    m_body.inv_inertia = {
        1.f / m_cfg.inertia.x,
        1.f / m_cfg.inertia.y,
        1.f / m_cfg.inertia.z,
    };
}

void
Vehicle::fixed_update(VehicleInput const& input,
                      float dt,
                      float floor_y) noexcept
{
    // 1 : smooth steer
    float const target_steer = input.steer * m_cfg.max_steer_angle;
    float const steer_delta = m_cfg.steer_speed * dt;

    if (target_steer > m_steer_angle)
        m_steer_angle = std::min(m_steer_angle + steer_delta, target_steer);
    else
        m_steer_angle = std::max(m_steer_angle - steer_delta, target_steer);

    // 2 : torque for each wheel
    // front wheels receive engine torque
    float engine_torque = 0.f;
    float brake_torque = 0.f;

    float const signed_speed = m_body.speed();
    float const reverse_speed_threshold = 0.8f; // m/s

    if (input.throttle > 0.f) {
        engine_torque = input.throttle * m_cfg.max_engine_torque;
    } else if (input.brake > 0.f && signed_speed < reverse_speed_threshold) {
        // Trackmania-like control: holding brake near standstill engages
        // reverse.
        engine_torque = -input.brake * m_cfg.max_engine_torque;
    } else {
        // engine brake when releasing
        engine_torque =
          -m_body.speed() * m_cfg.engine_brake_coeff * m_cfg.max_engine_torque;
    }

    if (input.brake > 0.f && signed_speed >= reverse_speed_threshold) {
        brake_torque = input.brake * m_cfg.max_brake_torque;
        // brake opposes movement
        if (signed_speed >= 0.f)
            engine_torque -= brake_torque;
        else
            engine_torque += brake_torque;
    }

    // no torque for back wheels
    std::array<float, 4> const wheel_torques{
        engine_torque * 0.5f, // FL
        engine_torque * 0.5f, // FR
        0.f,                  // RL
        0.f,                  // RR
    };
    // only front wheels steer
    std::array<float, 4> const steer_angles{
        m_steer_angle, // FL
        m_steer_angle, // FR
        0.f,           // RL
        0.f,           // RR
    };

    // 3 : wheels raycast and force
    for (int i = 0; i < 4; ++i) {
        m_wheel_states[i] = cast_wheel(m_cfg.wheels[i],
                                       m_body,
                                       wheel_torques[i],
                                       steer_angles[i],
                                       floor_y,
                                       dt);

        if (m_wheel_states[i].in_contact) {
            m_body.apply_force_at_point(m_wheel_states[i].suspension_force +
                                          m_wheel_states[i].friction_force,
                                        m_wheel_states[i].contact_point);
        }
    }

    // 4 : integration
    m_body.integrate(dt);
    m_body.clear_forces();

    // 5 : ground constraints
    float const min_y = floor_y + 0.3f;
    if (m_body.position.y < min_y) {
        m_body.position.y = min_y;
        if (m_body.velocity.y < 0.f)
            m_body.velocity.y = 0.f;
    }
}

int
Vehicle::grounded_wheels() const noexcept
{
    int count = 0;
    for (auto const& w : m_wheel_states)
        if (w.in_contact)
            ++count;
    return count;
}

void
Vehicle::reset(math::Vec3f pos, math::Quatf orient) noexcept
{
    m_body.position = pos;
    m_body.orientation = orient;
    m_body.velocity = math::Vec3f::zero();
    m_body.angular_vel = math::Vec3f::zero();
    m_body.clear_forces();
    m_steer_angle = 0.f;
}

} // namespace trackmini::physics