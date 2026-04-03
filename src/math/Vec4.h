#ifndef VEC4_H_
#define VEC4_H_

#include "math/Vec3.h"

namespace trackmini::math {

template<std::floating_point T>
struct Vec4
{
    T x{ 0 }, y{ 0 }, z{ 0 }, w{ 0 };

    // Constructors
    constexpr Vec4() noexcept = default;
    constexpr Vec4(T x_, T y_, T z_, T w_) noexcept
      : x{ x_ }
      , y{ y_ }
      , z{ z_ }
      , w{ w_ }
    {
    }
    constexpr Vec4(Vec3<T> const& v, T w_) noexcept
      : x{ v.x }
      , y{ v.y }
      , z{ v.z }
      , w{ w_ }
    {
    }
    constexpr explicit Vec4(T scalar) noexcept
      : x{ scalar }
      , y{ scalar }
      , z{ scalar }
      , w{ scalar }
    {
    }

    [[nodiscard]] constexpr Vec3<T> xyz() const noexcept { return { x, y, z }; }

    [[nodiscard]] constexpr Vec4 operator+(Vec4 const& o) const noexcept
    {
        return { x + o.x, y + o.y, z + o.z, w + o.w };
    }
    [[nodiscard]] constexpr Vec4 operator-(Vec4 const& o) const noexcept
    {
        return { x - o.x, y - o.y, z - o.z, w - o.w };
    }
    [[nodiscard]] constexpr Vec4 operator*(T s) const noexcept
    {
        return { x * s, y * s, z * s, w * s };
    }
    [[nodiscard]] constexpr Vec4 operator-() const noexcept
    {
        return { -x, -y, -z, -w };
    }

    [[nodiscard]] constexpr T dot(Vec4 const& o) const noexcept
    {
        return x * o.x + y * o.y + z * o.z + w * o.w;
    }

    [[nodiscard]] constexpr bool operator==(Vec4 const&) const noexcept =
      default;

    [[nodiscard]] constexpr T& operator[](int i) noexcept { return (&x)[i]; }
    [[nodiscard]] constexpr T const& operator[](int i) const noexcept
    {
        return (&x)[i];
    }
};

template<std::floating_point T>
[[nodiscard]] constexpr Vec4<T>
operator*(T s, Vec4<T> const& v) noexcept
{
    return v * s;
}

using Vec4f = Vec4<float>;
using Vec4d = Vec4<double>;

} // namespace trackmini::math

#endif /* VEC4_H_ */
