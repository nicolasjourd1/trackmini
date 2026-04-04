#ifndef COMMAND_H_
#define COMMAND_H_

#include "track/Track.h"
#include <memory>
#include <string_view>

namespace trackmini::editor {

// abstract interface for "undo-able" commands
// every editor action implement this
class Command
{
  public:
    virtual ~Command() = default;
    virtual void execute(track::Track& track) = 0;
    virtual void undo(track::Track& track) = 0;
    // Readable name for debug and ui
    [[nodiscard]] virtual std::string_view name() const noexcept = 0;
};

class PlaceBlockCommand final : public Command
{
  public:
    PlaceBlockCommand(track::GridPos pos, track::BlockInstance new_block)
      : m_pos{ pos }
      , m_new{ new_block }
    {
    }

    void execute(track::Track& track) override
    {
        m_old = track.grid().at(m_pos);
        track.grid().place(m_pos, m_new);
    }

    void undo(track::Track& track) override
    {
        if (m_old.is_empty())
            track.grid().remove(m_pos);
        else
            track.grid().place(m_pos, m_old);
    }

    [[nodiscard]] std::string_view name() const noexcept override
    {
        return "Place block";
    }

  private:
    track::GridPos m_pos;
    track::BlockInstance m_new;
    track::BlockInstance m_old{};
};

class RemoveBlockCommand final : public Command
{

  public:
    explicit RemoveBlockCommand(track::GridPos pos) noexcept
      : m_pos{ pos }
    {
    }

    void execute(track::Track& track) override
    {
        m_old = track.grid().at(m_pos);
        track.grid().remove(m_pos);
    }

    void undo(track::Track& track) override
    {
        if (!m_old.is_empty())
            track.grid().place(m_pos, m_old);
    }

    [[nodiscard]] std::string_view name() const noexcept override
    {
        return "Remove block";
    }

  private:
    track::GridPos m_pos;
    track::BlockInstance m_old{};
};

} // namespace trackmini::editor

#endif /* COMMAND_H_ */
