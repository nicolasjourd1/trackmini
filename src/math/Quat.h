#ifndef QUAT_H_
#define QUAT_H_

#include "math/Mat4.h"
#include "math/Vec3.h"
#include <cmath>

namespace trackmini::math {

// unit quaternion representing a rotation
// convention: q = w + xi + yj + zk
// w = scalar part (cos(theta/2))
// xyz = vector part (sin(theta/2) *  axis)

template<std::floating_point T>
struct Quat
{
    T w{ 1 }, x{ 0 }, y{ 0 }, z{ 0 };

    constexpr Quat() noexcept = default;
    constexpr Quat(T w, T x, T y, T z) noexcept
      : w{ w }
      , x{ x }
      , y{ y }
      , z{ z }
    {
    }

    [[nodiscard]] static constexpr Quat identity() noexcept
    {
        return { 1, 0, 0, 0 };
    }

    // axis needs to be normalized
    [[nodiscard]] static Quat from_axis_angle(Vec3<T> const& axis,
                                              T angle_rad) noexcept
    {
        T const half = angle_rad / T{ 2 };
        T const s = std::sin(half);
        return { std::cos(half), axis.x * s, axis.y * s, axis.z * s };
    }

    // X -> Y -> Z
    [[nodiscard]] static Quat from_euler(T pitch, T yaw, T roll) noexcept
    {
        T const cp = std::cos(pitch / 2), sp = std::sin(pitch / 2);
        T const cy = std::cos(yaw / 2), sy = std::sin(yaw / 2);
        T const cr = std::cos(roll / 2), sr = std::sin(roll / 2);
        return {
            cr * cp * cy + sr * sp * sy,
            sr * cp * cy - cr * sp * sy,
            cr * sp * cy + sr * cp * sy,
            cr * cp * sy - sr * sp * cy,
        };
    }

    // Operators
    [[nodiscard]] constexpr Quat operator*(Quat const& o) const noexcept
    {
        return {
            w * o.w - x * o.x - y * o.y - z * o.z,
            w * o.x + x * o.w + y * o.z - z * o.y,
            w * o.y - x * o.z + y * o.w + z * o.x,
            w * o.z + x * o.y - y * o.x + z * o.w,
        };
    }

    [[nodiscard]] constexpr Quat operator+(Quat const& o) const noexcept
    {
        return { w + o.w, x + o.x, y + o.y, z + o.z };
    }

    [[nodiscard]] constexpr Quat operator-(Quat const& o) const noexcept
    {
        return { w - o.w, x - o.x, y - o.y, z - o.z };
    }

    [[nodiscard]] constexpr Quat operator*(T s) const noexcept
    {
        return { w * s, x * s, y * s, z * s };
    }

    // Norm
    [[nodiscard]] constexpr T norm_sq() const noexcept
    {
        return w * w + x * x + y * y + z * z;
    }
    [[nodiscard]] T norm() const noexcept { return std::sqrt(norm_sq()); }

    [[nodiscard]] Quat normalized() const noexcept
    {
        T n = norm();
        return n > T{ 1e-10 } ? Quat{ w / n, x / n, y / n, z / n } : identity();
    }

    // Conjugate
    [[nodiscard]] constexpr Quat conjugate() const noexcept
    {
        return { w, -x, -y, -z };
    }

    // Rotates v by the quaternion
    // Formula : q * (0,v) * q^-1
    [[nodiscard]] constexpr Vec3<T> rotate(Vec3<T> const& v) const noexcept
    {
        // Optimized version (Fabrice Bellard) - avoids two quat multiplications
        Vec3<T> const qv{ x, y, z };
        Vec3<T> const t = T{ 2 } * qv.cross(v);
        return v + w * t + qv.cross(t);
    }

    [[nodiscard]] constexpr Mat4<T> to_mat4() const noexcept
    {
        T const xx = x * x, yy = y * y, zz = z * z;
        T const xy = x * y, xz = x * z, yz = y * z;
        T const wx = w * x, wy = w * y, wz = w * z;

        return {
            { T{ 1 } - T{ 2 } * (yy + zz),
              T{ 2 } * (xy + wz),
              T{ 2 } * (xz - wy),
              0 },
            { T{ 2 } * (xy - wz),
              T{ 1 } - T{ 2 } * (xx + zz),
              T{ 2 } * (yz + wx),
              0 },
            { T{ 2 } * (xz + wy),
              T{ 2 } * (yz - wx),
              T{ 1 } - T{ 2 } * (xx + yy),
              0 },
            { 0, 0, 0, 1 },
        };
    }

    // Spheric interpolation
    // t in [0,1] (see alpha in the game loop)
    [[nodiscard]] static Quat slerp(Quat a, Quat const& b, T t) noexcept
    {
        T dot = a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;

        // if dot < 0, the quaternions are in opposites hemispheres
        // we reverse b to take the shortest path
        Quat b2 = b;
        if (dot < T{ 0 }) {
            b2 = b * T{ -1 };
            dot = -dot;
        }

        // if very close, linear lerp to avoid division by 0
        if (dot > T{ 0.9995 }) {
            return (a + (b2 - a) * t).normalized();
        }

        T const theta_0 = std::acos(dot);
        T const theta = theta_0 * t;
        T const sin_t0 = std::sin(theta_0);
        T const sin_t = std::sin(theta);

        T const s1 = std::cos(theta) - dot * sin_t / sin_t0;
        T const s2 = sin_t / sin_t0;

        return (a * s1 + b2 * s2).normalized();
    }

    [[nodiscard]] constexpr bool operator==(Quat const&) const noexcept =
      default;
};

template<std::floating_point T>
[[nodiscard]] constexpr Quat<T>
operator*(T s, Quat<T> const& q) noexcept
{
    return q * s;
}

using Quatf = Quat<float>;
using Quatd = Quat<double>;

} // namespace trackmini::math

#endif /* QUAT_H_ */
