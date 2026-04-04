#ifndef COMMANDHISTORY_H_
#define COMMANDHISTORY_H_

#include "editor/Command.h"
#include <cstddef>
#include <memory>
#include <vector>

namespace trackmini::editor {

class CommandHistory
{
  public:
    explicit CommandHistory(std::size_t max_size = 256) noexcept
      : m_max_size{ max_size }
    {
    }

    void execute(std::unique_ptr<Command> cmd, track::Track& track);
    bool undo(track::Track& track);
    bool redo(track::Track& track);

    [[nodiscard]] bool can_undo() const noexcept { return m_cursor > 0; }
    [[nodiscard]] bool can_redo() const noexcept
    {
        return m_cursor < m_history.size();
    }

    [[nodiscard]] std::size_t undo_count() const noexcept { return m_cursor; }
    [[nodiscard]] std::size_t redo_count() const noexcept
    {
        return m_history.size() - m_cursor;
    }

    void clear() noexcept;

    [[nodiscard]] std::string_view next_undo_name() const noexcept;
    [[nodiscard]] std::string_view next_redo_name() const noexcept;

  private:
    std::vector<std::unique_ptr<Command>> m_history;
    std::size_t m_cursor{ 0 };
    std::size_t m_max_size;
};

} // namespace trackmini::editor

#endif /* COMMANDHISTORY_H_ */
