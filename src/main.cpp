#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <print>

#include "engine/EventBus.h"
#include "engine/GameLoop.h"
#include "platform/Window.h"

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

    // Event bus
    trackmini::engine::EventBus bus;

    bool running = true;
    bus.subscribe<trackmini::engine::Events::QuitRequested>(
      [&running](auto const&) { running = false; });
    bus.subscribe<trackmini::engine::Events::KeyPressed>(
      [&running](trackmini::engine::Events::KeyPressed const& e) {
          if (e.scancode == SDL_SCANCODE_ESCAPE)
              running = false;
      });

    // Callbacks
    trackmini::engine::GameLoopCallbacks callbacks{
        .fixed_update =
          [](trackmini::engine::Duration /* dt*/) {
              // nothing here yet
          },

        .update =
          [](trackmini::engine::Duration /* dt */, double /* alpha */) {
              // nothing here yet
          },

        .render =
          [&window](double /* alpha */) {
              glClearColor(0.05f, 0.07f, 0.12f, 1.0f);
              glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
              window.swap_buffers();
          },

        .is_running = [&running]() { return running; },

        .poll_events =
          [&bus]() {
              SDL_Event e;
              while (SDL_PollEvent(&e)) {
                  switch (e.type) {
                      case SDL_EVENT_QUIT:
                          bus.emit(trackmini::engine::Events::QuitRequested{});
                          break;
                      case SDL_EVENT_KEY_DOWN:
                          bus.emit(trackmini::engine::Events::KeyPressed{
                            .scancode = e.key.scancode,
                            .repeat = e.key.repeat });
                          break;
                      case SDL_EVENT_KEY_UP:
                          bus.emit(trackmini::engine::Events::KeyReleased{
                            .scancode = e.key.scancode,
                          });
                          break;
                      default:
                          break;
                  }
              }
          }
    };

    trackmini::engine::GameLoop loop(std::move(callbacks));
    loop.run();

    auto s = loop.stats();
    std::println("[Main] Avg FPS: {:.1f}", s.average_fps);

    SDL_Quit();
    return 0;
}