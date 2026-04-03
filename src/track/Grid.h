#ifndef GRID_H_
#define GRID_H_

#include "track/BlockId.h"
#include <functional>
#include <optional>
#include <unordered_map>
#include <vector>

namespace trackmini::track {

inline constexpr int16_t kGridMin = -128;
inline constexpr int16_t kGridMax = 127;

class Grid
{
  public:
    bool place(GridPos pos, BlockInstance block) noexcept;
    bool remove(GridPos pos) noexcept;
    void clear() noexcept { m_cells.clear(); }

    [[nodiscard]]
    BlockInstance at(GridPos pos) const noexcept;

    [[nodiscard]]
    bool is_empty(GridPos pos) const noexcept
    {
        return at(pos).is_empty();
    }

    [[nodiscard]]
    bool in_bounds(GridPos pos) const noexcept
    {
        return pos.x >= kGridMin && pos.x <= kGridMax && pos.y >= kGridMin &&
               pos.y <= kGridMax && pos.z >= kGridMin && pos.z <= kGridMax;
    }

    [[nodiscard]] std::size_t block_count() const noexcept
    {
        return m_cells.size();
    }

    // iters on non empty blocks
    template<typename Fn>
    void for_each(Fn&& fn) const
    {
        for (auto const& [pos, block] : m_cells) {
            fn(pos, block);
        }
    }

    [[nodiscard]]
    std::vector<GridPos> find_all(BlockId id) const noexcept;

    struct ValidationResult
    {
        bool valid{ true };
        std::vector<std::string> errors;
    };

    [[nodiscard]]
    ValidationResult validate() const noexcept;

  private:
    [[nodiscard]]
    static bool check_bounds(GridPos pos) noexcept
    {
        return pos.x >= kGridMin && pos.x <= kGridMax && pos.y >= kGridMin &&
               pos.y <= kGridMax && pos.z >= kGridMin && pos.z <= kGridMax;
    }

    std::unordered_map<GridPos, BlockInstance> m_cells;
};

} // namespace trackmini::track

#endif /* GRID_H_ */
