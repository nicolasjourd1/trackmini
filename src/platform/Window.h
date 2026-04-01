#ifndef WINDOW_H_
#define WINDOW_H_

#include <expected>
#include <string>
#include <string_view>

#include <SDL3/SDL.h>

namespace trackmini::platform {
struct WindowConfig
{
    std::string_view title = "TrackMini";
    int width = 1280;
    int height = 720;
    bool vsync = true;
};

struct WindowError
{
    std::string message;
};

class Window
{
  public:
    [[nodiscard]]
    static auto create(WindowConfig const& cfg = {})
      -> std::expected<Window, WindowError>;

    Window(Window const&) = delete;
    Window& operator=(Window const&) = delete;

    Window(Window&&) noexcept;
    Window& operator=(Window&&) noexcept;

    ~Window();

    void swap_buffers() const;

    [[nodiscard]] int width() const noexcept { return m_width; }
    [[nodiscard]] int height() const noexcept { return m_height; }

    [[nodiscard]] SDL_Window* sdl_handle() const noexcept { return m_window; }

  private:
    explicit Window(SDL_Window* window,
                    SDL_GLContext ctx,
                    int w,
                    int h) noexcept;

    SDL_Window* m_window = nullptr;
    SDL_GLContext m_context = nullptr;
    int m_width = 0;
    int m_height = 0;
};

} // namespace tm::platform

#endif /* WINDOW_H_ */
