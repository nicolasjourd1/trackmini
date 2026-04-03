#include "track/Grid.h"
#include <format>

namespace trackmini::track {

bool
Grid::place(GridPos pos, BlockInstance block) noexcept
{
    if (!check_bounds(pos))
        return false;
    if (block.is_empty()) {
        // place empty block = remove
        m_cells.erase(pos);
        return true;
    }
    m_cells.insert_or_assign(pos, block);
    return true;
}

bool
Grid::remove(GridPos pos) noexcept
{
    return m_cells.erase(pos) > 0;
}

BlockInstance
Grid::at(GridPos pos) const noexcept
{
    auto it = m_cells.find(pos);
    return it != m_cells.end() ? it->second : BlockInstance{};
}

std::vector<GridPos>
Grid::find_all(BlockId id) const noexcept
{
    std::vector<GridPos> result;
    for (auto const& [pos, block] : m_cells) {
        if (block.id == id)
            result.push_back(pos);
    }
    return result;
}

Grid::ValidationResult
Grid::validate() const noexcept
{
    ValidationResult result;

    // Rule 1 : at least one finish
    auto finishes = find_all(BlockId::Finish);
    if (finishes.empty()) {
        result.valid = false;
        result.errors.push_back("No finish block found");
    }

    // Rule 2 : at least 3 blocks
    if (m_cells.size() < 3) {
        result.valid = false;
        result.errors.push_back(std::format(
          "Track too short ({} blocks, need at least 3)", m_cells.size()));
    }

    return result;
}

} // namespace trackmini::track