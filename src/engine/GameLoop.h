#ifndef GAMELOOP_H_
#define GAMELOOP_H_

#include "engine/Clock.h"
#include "engine/EventBus.h"
#include <functional>

namespace trackmini::engine {

struct GameLoopCallbacks
{
    // always called with dt = kPhysicsDt = 10ms
    // still an argument to avoid redondant/magic constant
    std::function<void(Duration dt)> fixed_update;

    // called whenever possible
    // interpolate between two physic ticks for smoothness
    std::function<void(Duration dt, double alpha)> update;

    // called after update, with same alpha
    std::function<void(double alpha)> render;

    std::function<bool()> is_running;
    std::function<void()> poll_events;

    std::function<Duration()> tick_fn;
};

/*
    https://gafferongames.com/post/fix_your_timestep/

    Logic:
    acc += frame_dt
    while acc >= physics_dt
        fixed_update(physics_dt)
        acc -= physics_dt
    alpha = acc / physics_dt
    render(alpha)

    Allow deterministic physics, smoother rendering even
    with slower rendering (and maybe replays ?)

*/
class GameLoop
{
  public:
    explicit GameLoop(GameLoopCallbacks callbacks) noexcept;
    void run();

    struct Stats
    {
        std::size_t total_frames;
        std::size_t total_ticks;
        double average_fps;
        Duration total_time;
    };

    [[nodiscard]] Stats stats() const noexcept { return m_stats; }

  private:
    GameLoopCallbacks m_callbacks;
    Stats m_stats{};
};

} // namespace trackmini::engine

#endif /* GAMELOOP_H_ */
