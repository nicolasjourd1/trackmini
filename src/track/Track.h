#ifndef TRACK_H_
#define TRACK_H_

#include "track/BlockCatalog.h"
#include "track/Grid.h"
#include <chrono>
#include <optional>
#include <string>

namespace trackmini::track {

struct TrackMeta
{
    std::string name{ "Unnamed Track" };
    std::string author{ "Unknown" };
    // nullopt = never completed
    std::optional<uint32_t> best_time_ms;
};

class Track
{
  public:
    explicit Track(BlockCatalog const& catalog) noexcept
      : m_catalog(&catalog)
    {
    }

    [[nodiscard]] Grid& grid() noexcept { return m_grid; }
    [[nodiscard]] Grid const& grid() const noexcept { return m_grid; }
    [[nodiscard]] TrackMeta& meta() noexcept { return m_meta; }
    [[nodiscard]] TrackMeta const& meta() const noexcept { return m_meta; }
    [[nodiscard]] BlockCatalog const& catalog() const noexcept
    {
        return *m_catalog;
    }

    bool place_block(GridPos pos,
                     BlockId id,
                     Rotation rot = Rotation::R0) noexcept;

    [[nodiscard]] Grid::ValidationResult validate() const noexcept
    {
        return m_grid.validate();
    }

    [[nodiscard]] std::size_t block_count() const noexcept
    {
        return m_grid.block_count();
    }

  private:
    Grid m_grid;
    TrackMeta m_meta;
    BlockCatalog const* m_catalog;
};

} // namespace trackmini::track

#endif /* TRACK_H_ */
