#include "engine/GameLoop.h"
#include <print>

namespace trackmini::engine {

GameLoop::GameLoop(GameLoopCallbacks callbacks) noexcept
  : m_callbacks{ std::move(callbacks) }
{
}

void
GameLoop::run()
{
    FrameClock clock;

    auto const get_dt = m_callbacks.tick_fn
                          ? m_callbacks.tick_fn
                          : [&clock]() -> Duration { return clock.tick(); };

    Duration accumulator{ 0.0 };

    m_stats = {};
    auto const t_start = Clock::now();

    while (m_callbacks.is_running()) {
        // OS events
        m_callbacks.poll_events();

        Duration const frame_dt = get_dt();
        accumulator += frame_dt;

        // Physics ticks
        // > 100 fps, multiple ticks per frame
        // < 100 fps, 0 or 1 tick
        // Cap to 8
        constexpr int kMaxTicksPerFrame = 8;
        int ticks_this_frame = 0;

        constexpr Duration kEpsilon{ 1e-6 };
        while (accumulator + kEpsilon >= kPhysicsDt &&
               ticks_this_frame < kMaxTicksPerFrame) {
            m_callbacks.fixed_update(kPhysicsDt);
            accumulator -= kPhysicsDt;
            ++m_stats.total_ticks;
            ++ticks_this_frame;
        }

        // alpha : tick fraction between two ticks
        double const alpha = accumulator / kPhysicsDt;
        m_callbacks.update(frame_dt, alpha);

        // Rendering
        m_callbacks.render(alpha);

        ++m_stats.total_frames;
    }

    // end stats
    m_stats.total_time =
      std::chrono::duration_cast<Duration>(Clock::now() - t_start);
    m_stats.average_fps = static_cast<double>(m_stats.total_frames) /
                          std::max(m_stats.total_time.count(), 1e-9);

    std::println(
      "[GameLoop] Frames : {} | Ticks: {} | Avg FPS: {:.1f} | Time: {:.2f}s",
      m_stats.total_frames,
      m_stats.total_ticks,
      m_stats.average_fps,
      m_stats.total_time.count());
}

} // namespace trackmini::engine