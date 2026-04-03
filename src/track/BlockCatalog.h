#ifndef BLOCKCATALOG_H_
#define BLOCKCATALOG_H_

#include "math/Vec3.h"
#include "track/BlockId.h"

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace trackmini::track {

struct BlockDef
{
    BlockId id;
    std::string name;

    // size in celle numbers
    // most blocks are 1 x 1 x 1
    math::Vec3f size_cells{ 1.f, 1.f, 1.f };

    // path to mesh (relative to assets/)
    // nullptr means invisible block
    std::string mesh_path;

    // gameplay properties
    bool is_boost{ false };
    bool is_jump{ false };
    bool is_finish{ false };
    bool is_solid{ true };

    float boost_force{ 0.f };
};

class BlockCatalog
{

  public:
    static BlockCatalog make_default() noexcept;

    void register_block(BlockDef def) noexcept;

    [[nodiscard]]
    std::optional<BlockDef const*> find(BlockId id) const noexcept;

    [[nodiscard]]
    BlockDef const& get(BlockId id) const; // throws if missing

    [[nodiscard]]
    std::size_t size() const noexcept
    {
        return m_defs.size();
    }

    // for editor UI : iter on every block
    template<typename Fn>
    void for_each(Fn&& fn) const
    {
        for (auto const& [id, def] : m_defs)
            fn(def);
    }

  private:
    std::unordered_map<BlockId,
                       BlockDef,
                       // Hash forBlockId (enum class on uint16_t)
                       decltype([](BlockId b) {
                           return std::hash<uint16_t>{}(
                             static_cast<uint16_t>(b));
                       })>
      m_defs;
};

} // namespace trackmini::track

#endif /* BLOCKCATALOG_H_ */
