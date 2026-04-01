#ifndef MATH_H_
#define MATH_H_

#include <algorithm>
#include <cmath>
#include <concepts>
#include <numbers>

namespace trackmini::math {

template<std::floating_point T>
inline constexpr T kPi = std::numbers::pi_v<T>;

inline constexpr float kPif = kPi<float>;
inline constexpr double kPid = kPi<double>;
inline constexpr float kEps = 1e-6f;

template<std::floating_point T>
[[nodiscard]] constexpr T
to_radians(T degrees) noexcept
{
    return degrees * kPi<T> / T{ 180 };
}

template<std::floating_point T>
[[nodiscard]] constexpr T
to_degrees(T radians) noexcept
{
    return radians * T{ 180 } / kPi<T>;
}

template<std::floating_point T>
[[nodiscard]] constexpr T
lerp(T a, T b, T t) noexcept
{
    return a + t * (b - a);
}

template<std::floating_point T>
[[nodiscard]] constexpr bool
nearly_equal(T a, T b, T eps = T{ 1e-6 }) noexcept
{
    return std::abs(a - b) <= eps;
}

template<std::floating_point T>
[[nodiscard]] constexpr T
clamp(T v, T lo, T hi) noexcept
{
    return std::clamp(v, lo, hi);
}

} // namespace trackmini::math

#include "math/Mat4.h"
#include "math/Quat.h"
#include "math/Vec2.h"
#include "math/Vec3.h"
#include "math/Vec4.h"

#endif /* MATH_H_ */
