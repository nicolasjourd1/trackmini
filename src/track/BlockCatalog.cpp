#include "track/BlockCatalog.h"
#include <print>
#include <stdexcept>

namespace trackmini::track {

BlockCatalog
BlockCatalog::make_default() noexcept
{
    BlockCatalog cat;

    cat.register_block({
      .id = BlockId::Road,
      .name = "Road",
      .size_cells = { 1, 1, 1 },
      .mesh_path = "assets/blocks/road.obj",
      .is_solid = true,
    });

    cat.register_block({
      .id = BlockId::Boost,
      .name = "Boost",
      .size_cells = { 1, 1, 1 },
      .mesh_path = "assets/blocks/boost.obj",
      .is_boost = true,
      .is_solid = true,
      .boost_force = 50000.f,
    });

    cat.register_block({
      .id = BlockId::Jump,
      .name = "Jump",
      .size_cells = { 1, 1, 1 },
      .mesh_path = "assets/blocks/jump.obj",
      .is_jump = true,
      .is_solid = true,
    });

    cat.register_block({
      .id = BlockId::Finish,
      .name = "Finish",
      .size_cells = { 1, 1, 1 },
      .mesh_path = "assets/blocks/finish.obj",
      .is_finish = true,
      .is_solid = false, // traversable
    });

    return cat;
}

void
BlockCatalog::register_block(BlockDef def) noexcept
{
    auto id = def.id;
    m_defs.emplace(id, std::move(def));
}

std::optional<BlockDef const*>
BlockCatalog::find(BlockId id) const noexcept
{
    auto it = m_defs.find(id);
    if (it == m_defs.end())
        return std::nullopt;
    return &it->second;
}

BlockDef const&
BlockCatalog::get(BlockId id) const
{
    auto it = m_defs.find(id);
    if (it == m_defs.end()) {
        throw std::out_of_range{ "BlockCatalog::get — unknown BlockId" };
    }
    return it->second;
}

} // namespace trackmini::track