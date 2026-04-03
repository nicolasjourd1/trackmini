#ifndef VEC3_H_
#define VEC3_H_

#include <cmath>
#include <concepts>

namespace trackmini::math {
template<std::floating_point T>
struct Vec3
{
    T x{ 0 }, y{ 0 }, z{ 0 };

    // Constructors
    constexpr Vec3() noexcept = default;
        constexpr Vec3(T x_, T y_, T z_) noexcept
            : x{ x_ }
            , y{ y_ }
            , z{ z_ }
    {
    }
    constexpr explicit Vec3(T scalar) noexcept
      : x{ scalar }
      , y{ scalar }
      , z{ scalar }
    {
    }

    // Constants
    [[nodiscard]] static constexpr Vec3 zero() noexcept { return { 0, 0, 0 }; }
    [[nodiscard]] static constexpr Vec3 one() noexcept { return { 1, 1, 1 }; }
    [[nodiscard]] static constexpr Vec3 up() noexcept { return { 0, 1, 0 }; }
    [[nodiscard]] static constexpr Vec3 right() noexcept { return { 1, 0, 0 }; }
    [[nodiscard]] static constexpr Vec3 forward() noexcept
    {
        return { 0, 0, -1 };
    }

    // Arithmetic
    [[nodiscard]] constexpr Vec3 operator+(Vec3 const& o) const noexcept
    {
        return { x + o.x, y + o.y, z + o.z };
    }
    [[nodiscard]] constexpr Vec3 operator-(Vec3 const& o) const noexcept
    {
        return { x - o.x, y - o.y, z - o.z };
    }
    [[nodiscard]] constexpr Vec3 operator*(T s) const noexcept
    {
        return { x * s, y * s, z * s };
    }
    [[nodiscard]] constexpr Vec3 operator/(T s) const noexcept
    {
        return { x / s, y / s, z / s };
    }
    [[nodiscard]] constexpr Vec3 operator-() const noexcept
    {
        return { -x, -y, -z };
    }

    // Product "coords by coords"
    [[nodiscard]] constexpr Vec3 operator*(Vec3 const& o) const noexcept
    {
        return { x * o.x, y * o.y, z * o.z };
    }

    constexpr Vec3& operator+=(Vec3 const& o) noexcept
    {
        x += o.x;
        y += o.y;
        z += o.z;
        return *this;
    }
    constexpr Vec3& operator-=(Vec3 const& o) noexcept
    {
        x -= o.x;
        y -= o.y;
        z -= o.z;
        return *this;
    }
    constexpr Vec3& operator*=(T s) noexcept
    {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }
    constexpr Vec3& operator/=(T s) noexcept
    {
        x /= s;
        y /= s;
        z /= s;
        return *this;
    }

    [[nodiscard]] constexpr bool operator==(Vec3 const&) const noexcept =
      default;

    // Scalar product
    [[nodiscard]] constexpr T dot(Vec3 const& o) const noexcept
    {
        return x * o.x + y * o.y + z * o.z;
    }

    // Vector product
    [[nodiscard]] constexpr Vec3 cross(Vec3 const& o) const noexcept
    {
        return {
            y * o.z - z * o.y,
            z * o.x - x * o.z,
            x * o.y - y * o.x,
        };
    }

    // Norm
    [[nodiscard]] constexpr T length_sq() const noexcept { return dot(*this); }
    [[nodiscard]] T length() const noexcept { return std::sqrt(length_sq()); }

    [[nodiscard]] Vec3 normalized() const noexcept
    {
        T len = length();
        return len > static_cast<T>(1e-10) ? (*this / len) : Vec3{};
    }

    [[nodiscard]] static Vec3 lerp(Vec3 const& a, Vec3 const& b, T t) noexcept
    {
        return a + (b - a) * t;
    }

    [[nodiscard]] constexpr Vec3 reflect(Vec3 const& n) const noexcept
    {
        return *this - n * (T{ 2 } * dot(n));
    }

    [[nodiscard]] constexpr T& operator[](int i) noexcept { return (&x)[i]; }
    [[nodiscard]] constexpr T const& operator[](int i) const noexcept
    {
        return (&x)[i];
    }
};

template<std::floating_point T>
[[nodiscard]] constexpr Vec3<T>
operator*(T s, Vec3<T> const& v) noexcept
{
    return v * s;
}

using Vec3f = Vec3<float>;
using Vec3d = Vec3<double>;

} // namespace trackmini::math

#endif /* VEC3_H_ */
