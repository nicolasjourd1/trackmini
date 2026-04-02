#include <SDL3/SDL.h>
#include <array>
#include <glad/glad.h>
#include <print>

#include "engine/EventBus.h"
#include "engine/GameLoop.h"
#include "platform/Window.h"
#include "renderer/Buffer.h"
#include "renderer/Shader.h"
#include "renderer/VertexArray.h"

struct Vertex
{
    float px, py, pz;
    float cr, cg, cb;
};

int
main()
{
    auto win_result = trackmini::platform::Window::create({
      .title = "Trackmini",
      .width = 1280,
      .height = 720,
      .vsync = true,
    });
    if (!win_result) {
        std::println(stderr, "[FATAL] {}", win_result.error().message);
        return 1;
    }
    auto& window = *win_result;

    auto shader_result = trackmini::renderer::ShaderProgram::from_files(
      "assets/shaders/triangle.vert", "assets/shaders/triangle.frag");
    if (!shader_result) {
        std::println(stderr, "[FATAL] {}", shader_result.error().message);
        return 1;
    }
    auto& shader = *shader_result;

    static constexpr std::array<float, 18> kVertices{ {
      0.0f,
      0.5f,
      0.0f,
      1.0f,
      0.2f,
      0.2f, // up, red
      -0.5f,
      -0.5f,
      0.0f,
      0.2f,
      1.0f,
      0.2f, // down left, vert
      0.5f,
      -0.5f,
      0.0f,
      0.2f,
      0.2f,
      1.0f, // down right, bleu
    } };

    trackmini::renderer::VertexBuffer vbo;
    vbo.allocate(kVertices, trackmini::renderer::BufferUsage::StaticDraw);

    static constexpr std::array<trackmini::renderer::VertexAttrib, 2> kAttribs{
        {
          { 0, 3, GL_FLOAT, false, offsetof(Vertex, px) },
          { 1, 3, GL_FLOAT, false, offsetof(Vertex, cr) },
        }
    };

    trackmini::renderer::VertexArray vao;
    vao.add_vertex_buffer(vbo.handle(), kAttribs, sizeof(Vertex));

    // opengl state
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, window.width(), window.height());

    trackmini::engine::EventBus bus;
    bool running = true;

    bus.subscribe<trackmini::engine::Events::QuitRequested>(
      [&](auto const&) { running = false; });
    bus.subscribe<trackmini::engine::Events::KeyPressed>(
      [&](trackmini::engine::Events::KeyPressed const& e) {
          if (e.scancode == SDL_SCANCODE_ESCAPE)
              running = false;
      });
    bus.subscribe<trackmini::engine::Events::WindowResized>(
      [&](trackmini::engine::Events::WindowResized const& e) {
          glViewport(0, 0, e.width, e.height);
      });

    // Game loop
    trackmini::engine::GameLoop loop{ {
      .fixed_update = [](trackmini::engine::Duration) {},
      .update = [](trackmini::engine::Duration, double) {},

      .render =
        [&](double /*alpha*/) {
            glClearColor(0.05f, 0.07f, 0.12f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shader.bind();
            vao.bind();
            // GL_TRIANGLES : 3 vertices → 1 triangle
            glDrawArrays(GL_TRIANGLES, 0, 3);

            window.swap_buffers();
        },

      .is_running = [&]() { return running; },

      .poll_events =
        [&]() {
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                switch (e.type) {
                    case SDL_EVENT_QUIT:
                        bus.emit(trackmini::engine::Events::QuitRequested{});
                        break;
                    case SDL_EVENT_KEY_DOWN:
                        bus.emit(trackmini::engine::Events::KeyPressed{
                          .scancode = e.key.scancode,
                          .repeat = e.key.repeat,
                        });
                        break;
                    case SDL_EVENT_KEY_UP:
                        bus.emit(trackmini::engine::Events::KeyReleased{
                          .scancode = e.key.scancode,
                        });
                        break;
                    case SDL_EVENT_WINDOW_RESIZED:
                        bus.emit(trackmini::engine::Events::WindowResized{
                          .width = e.window.data1,
                          .height = e.window.data2,
                        });
                        break;
                    default:
                        break;
                }
            }
        },
    } };

    loop.run();
    SDL_Quit();
    return 0;
}