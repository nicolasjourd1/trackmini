#ifndef VEHICLE_H_
#define VEHICLE_H_

#include "physics/RigidBody.h"
#include "physics/VehicleConfig.h"
#include "physics/WheelRaycast.h"
#include <array>

namespace trackmini::physics {

// normalized input
struct VehicleInput
{
    float throttle{ 0.f }; // 0 = released, 1 = full
    float brake{ 0.f };    // 0 = released, 1 = full break
    float steer{ 0.f };    // -1 = left, +1 = right
    bool handbrake{ false };
};

class Vehicle
{
  public:
    explicit Vehicle(VehicleConfig cfg = {}) noexcept;

    // advance simulation of one physic tick
    void fixed_update(VehicleInput const& input,
                      float dt,
                      float floor_y = 0.f) noexcept;

    [[nodiscard]] RigidBody const& body() const noexcept { return m_body; }
    [[nodiscard]] RigidBody& body() noexcept { return m_body; }
    [[nodiscard]] VehicleConfig const& config() const noexcept { return m_cfg; }
    [[nodiscard]] float speed_kmh() const noexcept
    {
        return m_body.speed() * 3.6f;
    }
    [[nodiscard]] float steer_angle() const noexcept { return m_steer_angle; }

    [[nodiscard]]
    std::array<WheelState, 4> const& wheel_states() const noexcept
    {
        return m_wheel_states;
    }

    [[nodiscard]] int grounded_wheels() const noexcept;

    void reset(math::Vec3f pos,
               math::Quatf orient = math::Quatf::identity()) noexcept;

  private:
    VehicleConfig m_cfg;
    RigidBody m_body;
    std::array<WheelState, 4> m_wheel_states{};
    float m_steer_angle{ 0.f }; // current angle (smoothed)
};

} // namespace trackmini::physics

#endif /* VEHICLE_H_ */
