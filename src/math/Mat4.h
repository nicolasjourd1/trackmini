#ifndef MAT4_H_
#define MAT4_H_

#include "math/Vec3.h"
#include "math/Vec4.h"
#include <array>
#include <cmath>

namespace trackmini::math {

// 4x4 matrix stored in column-major (for OpenGL)
// m[col][row] instead of m[row][col]
// in memory : col0.xyzw, col1.xyzw, col2.xyzw, col3.xyzw

template<std::floating_point T>
struct Mat4
{
    Vec4<T> cols[4]{};

    constexpr Mat4() noexcept = default;

    constexpr Mat4(Vec4<T> c0, Vec4<T> c1, Vec4<T> c2, Vec4<T> c3) noexcept
      : cols{ c0, c1, c2, c3 }
    {
    }

    [[nodiscard]] static constexpr Mat4 identity() noexcept
    {
        return {
            { 1, 0, 0, 0 },
            { 0, 1, 0, 0 },
            { 0, 0, 1, 0 },
            { 0, 0, 0, 1 },
        };
    }

    [[nodiscard]] static constexpr Mat4 zero() noexcept { return {}; }

    // Access
    [[nodiscard]] constexpr Vec4<T>& operator[](int col) noexcept
    {
        return cols[col];
    }
    [[nodiscard]] constexpr Vec4<T> const& operator[](int col) const noexcept
    {
        return cols[col];
    }

    // Pointer to raw  data - for glUniformMatrix4fv for example
    [[nodiscard]] T const* data() const noexcept { return &cols[0].x; }

    [[nodiscard]] constexpr Mat4 operator*(Mat4 const& b) const noexcept
    {
        Mat4 result{};
        for (int col = 0; col < 4; ++col) {
            for (int row = 0; row < 4; ++row) {
                T sum{ 0 };
                for (int k = 0; k < 4; ++k) {
                    // cols[k][row] = élément à la ligne row, colonne k de *this
                    sum += cols[k][row] * b.cols[col][k];
                }
                result.cols[col][row] = sum;
            }
        }
        return result;
    }

    [[nodiscard]] constexpr Vec4<T> operator*(Vec4<T> const& v) const noexcept
    {
        return {
            cols[0][0] * v.x + cols[1][0] * v.y + cols[2][0] * v.z +
              cols[3][0] * v.w,
            cols[0][1] * v.x + cols[1][1] * v.y + cols[2][1] * v.z +
              cols[3][1] * v.w,
            cols[0][2] * v.x + cols[1][2] * v.y + cols[2][2] * v.z +
              cols[3][2] * v.w,
            cols[0][3] * v.x + cols[1][3] * v.y + cols[2][3] * v.z +
              cols[3][3] * v.w,
        };
    }

    [[nodiscard]] constexpr Mat4 transposed() const noexcept
    {
        Mat4 r{};
        for (int c = 0; c < 4; ++c)
            for (int row = 0; row < 4; ++row)
                r.cols[row][c] = cols[c][row];
        return r;
    }

    [[nodiscard]] static Mat4 translation(Vec3<T> const& t) noexcept
    {
        auto m = identity();
        m.cols[3] = { t.x, t.y, t.z, T{ 1 } };
        return m;
    }

    [[nodiscard]] static Mat4 scale(Vec3<T> const& s) noexcept
    {
        auto m = identity();
        m.cols[0][0] = s.x;
        m.cols[1][1] = s.y;
        m.cols[2][2] = s.z;
        return m;
    }

    // Rodrigues rotation formula
    // axis needs to be normalized
    [[nodiscard]] static Mat4 rotation(Vec3<T> const& axis,
                                       T angle_rad) noexcept
    {
        T const c = std::cos(angle_rad);
        T const s = std::sin(angle_rad);
        T const t = T{ 1 } - c;
        T const x = axis.x, y = axis.y, z = axis.z;

        return {
            { t * x * x + c, t * x * y + s * z, t * x * z - s * y, 0 },
            { t * x * y - s * z, t * y * y + c, t * y * z + s * x, 0 },
            { t * x * z + s * y, t * y * z - s * x, t * z * z + c, 0 },
            { 0, 0, 0, 1 },
        };
    }

    // fov_y : vertical angle in radians
    // aspect : height / width
    // near, far : clippping plans
    // opengl convention : Z in [-1, 1]
    [[nodiscard]] static Mat4 perspective(T fov_y,
                                          T aspect,
                                          T near,
                                          T far) noexcept
    {
        T const tan_half = std::tan(fov_y / T{ 2 });
        Mat4 m{};
        m.cols[0][0] = T{ 1 } / (aspect * tan_half);
        m.cols[1][1] = T{ 1 } / tan_half;
        m.cols[2][2] = -(far + near) / (far - near);
        m.cols[2][3] = T{ -1 };
        m.cols[3][2] = -(T{ 2 } * far * near) / (far - near);
        return m;
    }

    [[nodiscard]] static Mat4 look_at(Vec3<T> const& eye,
                                      Vec3<T> const& center,
                                      Vec3<T> const& up) noexcept
    {
        Vec3<T> const f = (center - eye).normalized(); // forward
        Vec3<T> const r = f.cross(up).normalized();    // right
        Vec3<T> const u = r.cross(f);                  // real up

        Mat4 m = identity();
        m.cols[0][0] = r.x;
        m.cols[1][0] = r.y;
        m.cols[2][0] = r.z;
        m.cols[0][1] = u.x;
        m.cols[1][1] = u.y;
        m.cols[2][1] = u.z;
        m.cols[0][2] = -f.x;
        m.cols[1][2] = -f.y;
        m.cols[2][2] = -f.z;
        m.cols[3][0] = -r.dot(eye);
        m.cols[3][1] = -u.dot(eye);
        m.cols[3][2] = f.dot(eye);
        return m;
    }

    [[nodiscard]] constexpr bool operator==(Mat4 const&) const noexcept =
      default;
};

using Mat4f = Mat4<float>;
using Mat4d = Mat4<double>;

} // namespace trackmini::math

#endif /* MAT4_H_ */
