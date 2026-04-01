#ifndef CLOCK_H_
#define CLOCK_H_

#include <chrono>
#include <ratio>

namespace trackmini::engine {

using Clock = std::chrono::steady_clock;
using TimePoint = Clock::time_point;
using Duration = std::chrono::duration<double>;
using Seconds = std::chrono::duration<double>;
using Nanos = std::chrono::duration<double, std::nano>;

// duration of a physics tick : 1/100 s = 10ms
inline constexpr Duration kPhysicsDt{
    std::chrono::duration<long long, std::ratio<1, 100>>{ 1 }
};

// Measure time between tick() calls
class FrameClock
{
  public:
    FrameClock() noexcept
      : m_last{ Clock::now() }
    {
    }

    [[nodiscard]]
    Duration tick() noexcept
    {
        auto const now = Clock::now();
        // cap delta time
        auto const raw_dt = std::chrono::duration_cast<Duration>(now - m_last);
        auto const clamped = std::min(raw_dt, Duration{ 0.25 });
        m_last = now;
        return clamped;
    }

    [[nodiscard]]
    Duration elapsed() const noexcept
    {
        return std::chrono::duration_cast<Duration>(Clock::now() - m_start);
    }

  private:
    TimePoint m_start{ Clock::now() };
    TimePoint m_last;
};

} // namespace trackmini::engine

#endif /* CLOCK_H_ */
