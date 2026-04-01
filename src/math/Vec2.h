#ifndef VEC2_H_
#define VEC2_H_

#include <cmath>
#include <concepts>

namespace trackmini::math {
template<std::floating_point T>
struct Vec2
{
    T x{ 0 }, y{ 0 };

    // Constructors
    constexpr Vec2() noexcept = default;
    constexpr Vec2(T x, T y) noexcept
      : x{ x }
      , y{ y }
    {
    }
    constexpr explicit Vec2(T scalar) noexcept
      : x{ scalar }
      , y{ scalar }
    {
    }

    // Arithmetic
    [[nodiscard]] constexpr Vec2 operator+(Vec2 const& o) const noexcept
    {
        return { x + o.x, y + o.y };
    }
    [[nodiscard]] constexpr Vec2 operator-(Vec2 const& o) const noexcept
    {
        return { x - o.x, y - o.y };
    }
    [[nodiscard]] constexpr Vec2 operator*(T s) const noexcept
    {
        return { x * s, y * s };
    }
    [[nodiscard]] constexpr Vec2 operator/(T s) const noexcept
    {
        return { x / s, y / s };
    }
    [[nodiscard]] constexpr Vec2 operator-() const noexcept
    {
        return { -x, -y };
    }

    constexpr Vec2& operator+=(Vec2 const& o) noexcept
    {
        x += o.x;
        y += o.y;
        return *this;
    }
    constexpr Vec2& operator-=(Vec2 const& o) noexcept
    {
        x -= o.x;
        y -= o.y;
        return *this;
    }
    constexpr Vec2& operator*=(T s) noexcept
    {
        x *= s;
        y *= s;
        return *this;
    }
    constexpr Vec2& operator/=(T s) noexcept
    {
        x /= s;
        y /= s;
        return *this;
    }

    [[nodiscard]] constexpr bool operator==(Vec2 const&) const noexcept =
      default;

    // Product
    [[nodiscard]] constexpr T dot(Vec2 const& o) const noexcept
    {
        return x * o.x + y * o.y;
    }

    // Norm
    [[nodiscard]] constexpr T length_sq() const noexcept { return dot(*this); }
    [[nodiscard]] T length() const noexcept { return std::sqrt(length_sq()); }

    [[nodiscard]] Vec2 normalized() const noexcept
    {
        T len = length();
        return len > T{ 1e-10 } ? (*this / len) : Vec2{};
    }

    // Index
    [[nodiscard]] constexpr T& operator[](int i) noexcept { return (&x)[i]; }
    [[nodiscard]] constexpr T const& operator[](int i) const noexcept
    {
        return (&x)[i];
    }
};

// Commutative product
template<std::floating_point T>
[[nodiscard]] constexpr Vec2<T>
operator*(T s, Vec2<T> const& v) noexcept
{
    return v * s;
}

// Aliases
using Vec2f = Vec2<float>;
using Vec2d = Vec2<double>;

} // namespace trackmini::math

#endif /* VEC2_H_ */
