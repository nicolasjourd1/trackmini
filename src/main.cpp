#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <print>

#include "editor/EditorState.h"
#include "editor/TrackRenderer.h"
#include "engine/EventBus.h"
#include "engine/GameLoop.h"
#include "math/Math.h"
#include "platform/Window.h"
#include "renderer/Camera.h"
#include "renderer/Mesh.h"
#include "renderer/Shader.h"
#include "renderer/UniformBuffer.h"
#include "track/BlockCatalog.h"
#include "track/Track.h"

int
main()
{
    struct SdlQuitGuard
    {
        ~SdlQuitGuard() { SDL_Quit(); }
    } sdl_quit_guard;

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

    // Shaders
    auto shader_result = trackmini::renderer::ShaderProgram::from_files(
      "assets/shaders/mesh.vert", "assets/shaders/mesh.frag");
    if (!shader_result) {
        std::println(stderr, "[FATAL] {}", shader_result.error().message);
        return 1;
    }
    auto& shader = *shader_result;

    // initialize u_tint to white (dont change color by default)
    float white_tint[3] = { 1.f, 1.f, 1.f };
    shader.set_vec3("u_tint", white_tint);

    // Camera
    trackmini::renderer::Camera camera({
      .orbit_radius = 4.f,
      .orbit_pitch = trackmini::math::to_radians(25.f),
      .orbit_yaw = trackmini::math::to_radians(40.f),
    });

    trackmini::renderer::UniformBuffer<trackmini::renderer::CameraMatrices>
      cam_ubo;

    // Track and editor
    auto catalog = trackmini::track::BlockCatalog::make_default();
    trackmini::track::Track track(catalog);
    trackmini::editor::EditorState editor(track);
    trackmini::editor::TrackRenderer track_renderer{ shader };

    // OpenGL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE); // backface culling
    glCullFace(GL_BACK);
    glViewport(0, 0, window.width(), window.height());

    // Event bus
    trackmini::engine::EventBus bus;
    bool running = true;
    bool mouse_drag = false;
    float last_mx = 0.f, last_my = 0.f;
    float mouse_x = 0.f, mouse_y = 0.f;
    int screen_w = window.width();
    int screen_h = window.height();

    bus.subscribe<trackmini::engine::Events::QuitRequested>(
      [&](auto const&) { running = false; });
    bus.subscribe<trackmini::engine::Events::KeyPressed>(
      [&](trackmini::engine::Events::KeyPressed const& e) {
          switch (e.scancode) {
              case SDL_SCANCODE_ESCAPE:
                  running = false;
                  break;
              case SDL_SCANCODE_Z:
                  if (editor.undo())
                      std::println("[Editor] Undo");
                  break;
              case SDL_SCANCODE_Y:
                  if (editor.redo())
                      std::println("[Editor] Redo");
                  break;
              case SDL_SCANCODE_TAB:
                  editor.cycle_block_forward();
                  std::println("[Editor] Selected: {}",
                               static_cast<int>(editor.selected_block()));
                  break;
              case SDL_SCANCODE_S:
                  if (editor.save("track.tm"))
                      std::println("[Editor] Saved");
                  break;
              case SDL_SCANCODE_L:
                  if (editor.load("track.tm"))
                      std::println("[Editor] Loaded");
                  break;
              default:
                  break;
          }
      });
    bus.subscribe<trackmini::engine::Events::WindowResized>(
      [&](trackmini::engine::Events::WindowResized const& e) {
          screen_w = e.width;
          screen_h = e.height;
          glViewport(0, 0, e.width, e.height);
          camera.set_aspect(static_cast<float>(e.width) /
                            static_cast<float>(e.height));
      });

    // Game loop

    trackmini::engine::GameLoop loop{ {
      .fixed_update = [](trackmini::engine::Duration) {},

      .update =
        [&](trackmini::engine::Duration, double) {
            camera.update();
            editor.update_cursor(mouse_x,
                                 mouse_y,
                                 screen_w,
                                 screen_h,
                                 camera.matrices(),
                                 camera.position());
        },

      .render =
        [&](double) {
            glClearColor(0.08f, 0.09f, 0.13f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            cam_ubo.upload(camera.matrices());
            cam_ubo.bind_to_slot(0);

            shader.bind();
            track_renderer.render(editor, camera.matrices());

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
                    case SDL_EVENT_WINDOW_RESIZED:
                        bus.emit(trackmini::engine::Events::WindowResized{
                          .width = e.window.data1,
                          .height = e.window.data2,
                        });
                        break;

                    // Caméra orbitale (clic droit)
                    case SDL_EVENT_MOUSE_BUTTON_DOWN:
                        if (e.button.button == SDL_BUTTON_RIGHT) {
                            mouse_drag = true;
                            last_mx = e.button.x;
                            last_my = e.button.y;
                        }
                        // Clic gauche = placer
                        if (e.button.button == SDL_BUTTON_LEFT)
                            editor.place_at_cursor();
                        // Clic milieu = supprimer
                        if (e.button.button == SDL_BUTTON_MIDDLE)
                            editor.remove_at_cursor();
                        break;
                    case SDL_EVENT_MOUSE_BUTTON_UP:
                        if (e.button.button == SDL_BUTTON_RIGHT)
                            mouse_drag = false;
                        break;
                    case SDL_EVENT_MOUSE_MOTION:
                        mouse_x = e.motion.x;
                        mouse_y = e.motion.y;
                        if (mouse_drag) {
                            float dx = (e.motion.x - last_mx) * 0.005f;
                            float dy = (e.motion.y - last_my) * 0.005f;
                            camera.orbit(dx, dy);
                            last_mx = e.motion.x;
                            last_my = e.motion.y;
                        }
                        break;
                    case SDL_EVENT_MOUSE_WHEEL:
                        camera.zoom(e.wheel.y * 0.4f);
                        break;
                    default:
                        break;
                }
            }
        },
    } };

    loop.run();
    return 0;
}