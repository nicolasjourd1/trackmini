#include <chrono>
#include <print>

#include <SDL3/SDL.h>

#include "platform/Window.h"
#include <glad/glad.h>

int
main()
{
    auto result = trackmini::platform::Window::create(
      { .title = "Trackmini", .width = 1280, .height = 720, .vsync = true });

    if (!result) {
        std::println(stderr, "[FATAL] {}", result.error().message);
        return 1;
    }

    auto& window = *result;

    bool running = true;
    auto t0 = std::chrono::steady_clock::now();

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
            if (event.type == SDL_EVENT_KEY_DOWN &&
                event.key.scancode == SDL_SCANCODE_ESCAPE)
                running = false;
        }

        glClearColor(0.05f, 0.07f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        window.swap_buffers();
    }

    auto elapsed = std::chrono::steady_clock::now() - t0;
    std::println("[Main] Session: {:.1f}s",
                 std::chrono::duration<double>(elapsed).count());

    SDL_Quit();
    return 0;
}