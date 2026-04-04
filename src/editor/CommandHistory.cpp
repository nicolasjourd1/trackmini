#include "editor/CommandHistory.h"

namespace trackmini::editor {

void
CommandHistory::execute(std::unique_ptr<Command> cmd, track::Track& track)
{
    if (m_cursor < m_history.size())
        m_history.erase(m_history.begin() +
                          static_cast<std::ptrdiff_t>(m_cursor),
                        m_history.end());

    cmd->execute(track);
    m_history.push_back(std::move(cmd));
    ++m_cursor;

    if (m_history.size() > m_max_size) {
        m_history.erase(m_history.begin());
        --m_cursor;
    }
}

bool
CommandHistory::undo(track::Track& track)
{
    if (!can_undo())
        return false;
    --m_cursor;
    m_history[m_cursor]->undo(track);
    return true;
}

bool
CommandHistory::redo(track::Track& track)
{
    if (!can_redo())
        return false;
    m_history[m_cursor]->execute(track);
    ++m_cursor;
    return true;
}

void
CommandHistory::clear() noexcept
{
    m_history.clear();
    m_cursor = 0;
}

std::string_view
CommandHistory::next_undo_name() const noexcept
{
    if (!can_undo())
        return "";
    return m_history[m_cursor - 1]->name();
}

std::string_view
CommandHistory::next_redo_name() const noexcept
{
    if (!can_redo())
        return "";
    return m_history[m_cursor]->name();
}

} // namespace trackmini::editor