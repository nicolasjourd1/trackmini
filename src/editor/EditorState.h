#ifndef EDITORSTATE_H_
#define EDITORSTATE_H_

#include "editor/CommandHistory.h"
#include "editor/Raycast.h"
#include "track/Track.h"
#include <filesystem>
#include <optional>

namespace trackmini::editor {

class EditorState
{
  public:
    explicit EditorState(track::Track& track) noexcept
      : m_track{ &track }
    {
    }

    void select_block(track::BlockId id) noexcept { m_selected_id = id; }

    [[nodiscard]] track::BlockId selected_block() const noexcept
    {
        return m_selected_id;
    }

    void cycle_block_forward() noexcept;
    void cycle_block_backward() noexcept;

    void update_cursor(float mouse_x,
                       float mouse_y,
                       int screen_w,
                       int screen_h,
                       renderer::CameraMatrices const& cam,
                       math::Vec3f cam_pos,
                       float plane_y = 0.f) noexcept;

    [[nodiscard]] std::optional<track::GridPos> cursor_pos() const noexcept
    {
        return m_cursor_pos;
    }

    void place_at_cursor() noexcept;
    void remove_at_cursor() noexcept;

    bool undo() { return m_history.undo(*m_track); }
    bool redo() { return m_history.redo(*m_track); }

    [[nodiscard]] bool can_undo() const noexcept
    {
        return m_history.can_undo();
    }
    [[nodiscard]] bool can_redo() const noexcept
    {
        return m_history.can_redo();
    }
    [[nodiscard]] std::string_view next_undo_name() const noexcept
    {
        return m_history.next_undo_name();
    }

    bool save(std::filesystem::path const& path) const noexcept;
    bool load(std::filesystem::path const& path) noexcept;

    [[nodiscard]] track::Track const& track() const noexcept
    {
        return *m_track;
    }

  private:
    track::Track* m_track;
    CommandHistory m_history;
    track::BlockId m_selected_id{ track::BlockId::Road };
    std::optional<track::GridPos> m_cursor_pos;

    static constexpr std::array kAvailableBlocks{
        track::BlockId::Road,
        track::BlockId::Boost,
        track::BlockId::Jump,
        track::BlockId::Finish,
    };
};

} // namespace trackmini::editor

#endif /* EDITORSTATE_H_ */
