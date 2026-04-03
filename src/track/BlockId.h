#ifndef BLOCKID_H_
#define BLOCKID_H_

#include <cstdint>
#include <functional>
#include <string_view>

namespace trackmini::track {

enum class BlockId : uint16_t
{
    Empty = 0,
    Road = 1,
    Boost = 2,
    Jump = 3,
    Finish = 4,
};

struct GridPos
{
    int16_t x{ 0 }, y{ 0 }, z{ 0 };

    constexpr bool operator==(GridPos const&) const noexcept = default;

    [[nodiscard]] constexpr GridPos offset(int dx,
                                           int dy,
                                           int dz) const noexcept
    {
        return {
            static_cast<int16_t>(x + dx),
            static_cast<int16_t>(y + dy),
            static_cast<int16_t>(z + dz),
        };
    }
};

enum class Rotation : uint8_t
{
    R0 = 0,
    R90 = 1,
    R180 = 2,
    R270 = 3
};

[[nodiscard]] constexpr float
rotation_radians(Rotation r) noexcept
{
    return static_cast<float>(r) * (3.14159265f / 2.f);
}

struct BlockInstance
{
    BlockId id{ BlockId::Empty };
    Rotation rotation{ Rotation::R0 };

    [[nodiscard]] constexpr bool is_empty() const noexcept
    {
        return id == BlockId::Empty;
    }
};

} // namespace trackmini::track

// for std::unordered_map
template<>
struct std::hash<trackmini::track::GridPos>
{
    std::size_t operator()(trackmini::track::GridPos const& p) const noexcept
    {
        std::size_t h = 0;
        h ^= std::hash<int16_t>{}(p.x) + 0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= std::hash<int16_t>{}(p.y) + 0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= std::hash<int16_t>{}(p.z) + 0x9e3779b9 + (h << 6) + (h >> 2);
        return h;
    }
};

#endif /* BLOCKID_H_ */
