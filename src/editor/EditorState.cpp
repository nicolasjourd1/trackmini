#include "editor/EditorState.h"
#include "track/TrackSerializer.h"
#include <algorithm>
#include <print>

namespace trackmini::editor {

void
EditorState::cycle_block_forward() noexcept
{
    auto it = std::ranges::find(kAvailableBlocks, m_selected_id);
    if (it == kAvailableBlocks.end() || std::next(it) == kAvailableBlocks.end())
        m_selected_id = kAvailableBlocks.front();
    else
        m_selected_id = *std::next(it);
}

void
EditorState::cycle_block_backward() noexcept
{
    auto it = std::ranges::find(kAvailableBlocks, m_selected_id);
    if (it == kAvailableBlocks.end() || it == kAvailableBlocks.begin())
        m_selected_id = kAvailableBlocks.back();
    else
        m_selected_id = *std::prev(it);
}

void
EditorState::update_cursor(float mouse_x,
                           float mouse_y,
                           int screen_w,
                           int screen_h,
                           renderer::CameraMatrices const& cam,
                           math::Vec3f cam_pos,
                           float plane_y) noexcept
{
    Ray ray =
      ray_from_screen(mouse_x, mouse_y, screen_w, screen_h, cam, cam_pos);
    auto hit = ray_plane_y(ray, plane_y);

    if (hit) {
        auto gpos = world_to_grid(*hit);
        if (m_track->grid().in_bounds(gpos))
            m_cursor_pos = gpos;
        else
            m_cursor_pos = std::nullopt;
    } else {
        m_cursor_pos = std::nullopt;
    }
}

void
EditorState::place_at_cursor() noexcept
{
    if (!m_cursor_pos)
        return;
    m_history.execute(
      std::make_unique<PlaceBlockCommand>(
        *m_cursor_pos,
        track::BlockInstance{ m_selected_id, track::Rotation::R0 }),
      *m_track);
}

void
EditorState::remove_at_cursor() noexcept
{
    if (!m_cursor_pos)
        return;
    if (m_track->grid().is_empty(*m_cursor_pos))
        return;
    m_history.execute(std::make_unique<RemoveBlockCommand>(*m_cursor_pos),
                      *m_track);
}

bool
EditorState::save(std::filesystem::path const& path) const noexcept
{
    auto r = track::TrackSerializer::save(*m_track, path);
    if (!r)
        std::println(stderr, "[Editor] Save failed: {}", r.error().message);
    return r.has_value();
}

bool
EditorState::load(std::filesystem::path const& path) noexcept
{
    auto r = track::TrackSerializer::load(m_track->catalog(), path);
    if (!r) {
        std::println(stderr, "[Editor] Load failed: {}", r.error().message);
        return false;
    }
    // replace current track by the loaded track
    m_track->grid().clear();
    r->grid().for_each([&](track::GridPos pos, track::BlockInstance blk) {
        m_track->grid().place(pos, blk);
    });
    m_track->meta() = r->meta();
    m_history.clear();
    return true;
}

} // namespace trackmini::editor