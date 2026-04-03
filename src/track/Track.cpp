#include "track/Track.h"

namespace trackmini::track {

bool
Track::place_block(GridPos pos, BlockId id, Rotation rot) noexcept
{
    if (!m_catalog->find(id).has_value())
        return false;
    return m_grid.place(pos, { id, rot });
}

} // namespace trackmini::track