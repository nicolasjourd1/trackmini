#include "platform/Window.h"

#include <print>
#include <utility>

#include <SDL3/SDL.h>
#include <glad/glad.h>

namespace trackmini::platform {

auto
Window::create(WindowConfig const& cfg) -> std::expected<Window, WindowError>
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return std::unexpected(
          WindowError{ std::string{ "SDL_Init failed: " } + SDL_GetError() });
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
                        SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_Window* raw_window =
      SDL_CreateWindow(cfg.title.data(),
                       cfg.width,
                       cfg.height,
                       SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (!raw_window) {
        return std::unexpected(WindowError{
          std::string{ "SDL_CreateWindow failed: " } + SDL_GetError() });
    }

    SDL_GLContext ctx = SDL_GL_CreateContext(raw_window);
    if (!ctx) {
        SDL_DestroyWindow(raw_window);
        return std::unexpected(WindowError{
          std::string{ "SDL_GL_CreateContext failed: " } + SDL_GetError() });
    }

    if (!gladLoadGLLoader(
          reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
        SDL_GL_DestroyContext(ctx);
        SDL_DestroyWindow(raw_window);
        return std::unexpected(WindowError{ "gladLoadGLLoader failed" });
    }

    SDL_GL_SetSwapInterval(cfg.vsync ? 1 : 0);

    std::println("[Window] OpenGL {},{} | Renderer: {}",
                 4,
                 6,
                 reinterpret_cast<char const*>(glGetString(GL_RENDERER)));

    return Window{ raw_window, ctx, cfg.width, cfg.height };
}

Window::Window(SDL_Window* window, SDL_GLContext ctx, int w, int h) noexcept
  : m_window{ window }
  , m_context{ ctx }
  , m_width{ w }
  , m_height{ h }
{
}

Window::Window(Window&& other) noexcept
  : m_window{ std::exchange(other.m_window, nullptr) }
  , m_context{ std::exchange(other.m_context, nullptr) }
  , m_width{ std::exchange(other.m_width, 0) }
  , m_height{ std::exchange(other.m_height, 0) }
{
}

Window&
Window::operator=(Window&& other) noexcept
{
    if (this != &other) {
        this->~Window();
        new (this) Window{ std::move(other) };
    }
    return *this;
}

Window::~Window()
{
    if (m_context)
        SDL_GL_DestroyContext(m_context);
    if (m_window)
        SDL_DestroyWindow(m_window);
}

void
Window::swap_buffers() const
{
    SDL_GL_SwapWindow(m_window);
}

} // namespace tm::platform