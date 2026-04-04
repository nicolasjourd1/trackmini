// src/main.cpp
#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <print>

#include "editor/EditorState.h"
#include "editor/TrackRenderer.h"
#include "engine/EventBus.h"
#include "engine/GameLoop.h"
#include "math/Math.h"
#include "physics/Vehicle.h"
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
      "assets/shaders/mesh.vert", "assets/shaders/mesh.frag");
    if (!shader_result) {
        std::println(stderr, "[FATAL] {}", shader_result.error().message);
        return 1;
    }
    auto& shader = *shader_result;
    float white_tint[3] = { 1.f, 1.f, 1.f };
    shader.set_vec3("u_tint", white_tint);

    // Camera
    trackmini::renderer::Camera camera{ {
      .orbit_radius = 10.f,
      .orbit_pitch = trackmini::math::to_radians(30.f),
      .orbit_yaw = trackmini::math::to_radians(30.f),
    } };
    trackmini::renderer::UniformBuffer<trackmini::renderer::CameraMatrices>
      cam_ubo;

    // Track
    auto catalog = trackmini::track::BlockCatalog::make_default();
    trackmini::track::Track track{ catalog };
    trackmini::editor::EditorState editor{ track };
    trackmini::editor::TrackRenderer track_renderer{ shader };

    // Test track
    for (int i = -4; i <= 4; ++i)
        track.place_block({ static_cast<int16_t>(i), 0, 0 },
                          trackmini::track::BlockId::Road);
    track.place_block({ 5, 0, 0 }, trackmini::track::BlockId::Finish);

    // Vehicle
    trackmini::physics::Vehicle vehicle;
    vehicle.reset({ 0.f, 1.5f, 0.f });

    // Vehicle mesh
    auto car_data = trackmini::renderer::MeshData::make_cube(0.2f);
    for (auto& v : car_data.vertices) {
        v.position.x *= 1.5f;
        v.position.y *= 0.5f;
        v.position.z *= 3.0f;
        v.color = { 0.8f, 0.2f, 0.2f }; // red
    }
    trackmini::renderer::Mesh car_mesh{ car_data };

    trackmini::physics::VehicleInput input{};

    // mode (editor or drive)
    bool drive_mode = false;

    // Event bus
    trackmini::engine::EventBus bus;
    bool running = true;
    bool mouse_drag = false;
    float last_mx = 0.f, last_my = 0.f;
    float mouse_x = 0.f, mouse_y = 0.f;

    bus.subscribe<trackmini::engine::Events::QuitRequested>(
      [&](auto const&) { running = false; });
    bus.subscribe<trackmini::engine::Events::KeyPressed>(
      [&](trackmini::engine::Events::KeyPressed const& e) {
          switch (e.scancode) {
              case SDL_SCANCODE_ESCAPE:
                  running = false;
                  break;

              case SDL_SCANCODE_F1:
                  drive_mode = !drive_mode;
                  std::println("[Mode] {}", drive_mode ? "Drive" : "Editor");
                  if (drive_mode)
                      vehicle.reset({ 0.f, 1.5f, 0.f });
                  break;

              // Editor
              case SDL_SCANCODE_Z:
                  if (!drive_mode && editor.undo())
                      std::println("[Editor] Undo");
                  break;
              case SDL_SCANCODE_Y:
                  if (!drive_mode && editor.redo())
                      std::println("[Editor] Redo");
                  break;
              case SDL_SCANCODE_TAB:
                  if (!drive_mode)
                      editor.cycle_block_forward();
                  break;
              case SDL_SCANCODE_S:
                  if (!drive_mode)
                      editor.save("track.tmc");
                  break;
              case SDL_SCANCODE_L:
                  if (!drive_mode)
                      editor.load("track.tmc");
                  break;

              // Respawn
              case SDL_SCANCODE_R:
                  if (drive_mode)
                      vehicle.reset({ 0.f, 1.5f, 0.f });
                  break;
              default:
                  break;
          }
      });
    bus.subscribe<trackmini::engine::Events::WindowResized>(
      [&](trackmini::engine::Events::WindowResized const& e) {
          glViewport(0, 0, e.width, e.height);
          camera.set_aspect(static_cast<float>(e.width) /
                            static_cast<float>(e.height));
      });

    // OpenGL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glViewport(0, 0, window.width(), window.height());

    // Game loop
    trackmini::engine::GameLoop loop{ {

      .fixed_update =
        [&](trackmini::engine::Duration dt) {
            if (!drive_mode)
                return;

            auto const* keys = SDL_GetKeyboardState(nullptr);
            input.throttle = keys[SDL_SCANCODE_UP] ? 1.f : 0.f;
            input.brake = keys[SDL_SCANCODE_DOWN] ? 1.f : 0.f;
            input.steer = (keys[SDL_SCANCODE_LEFT] ? 1.f : 0.f) -
                          (keys[SDL_SCANCODE_RIGHT] ? 1.f : 0.f);
            input.handbrake = keys[SDL_SCANCODE_SPACE] != 0;

            vehicle.fixed_update(input, static_cast<float>(dt.count()));
        },

      .update =
        [&](trackmini::engine::Duration, double) {
            // in drive mode, camera follows vehicle
            if (drive_mode) {
                auto const& pos = vehicle.body().position;
                camera.set_target({ pos.x, 0.5f, pos.z });
            }

            camera.update();

            if (!drive_mode) {
                editor.update_cursor(mouse_x,
                                     mouse_y,
                                     window.width(),
                                     window.height(),
                                     camera.matrices(),
                                     camera.position());
            }
        },

      .render =
        [&](double alpha) {
            (void)alpha;
            glClearColor(0.08f, 0.09f, 0.13f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            cam_ubo.upload(camera.matrices());
            cam_ubo.bind_to_slot(0);
            shader.bind();

            // Grid and blocks
            track_renderer.render(editor, camera.matrices());

            // Vehicle
            if (drive_mode) {
                auto model = vehicle.body().model_matrix();
                shader.set_mat4("u_model", model.data());
                float red[3] = { 1.f, 0.f, 0.f };
                shader.set_vec3("u_tint", red);
                car_mesh.draw();
                shader.set_vec3("u_tint", white_tint);

                static int frame_cnt = 0;
                if (++frame_cnt % 120 == 0)
                    std::println("[Vehicle] {:.1f} km/h | wheels: {}",
                                 vehicle.speed_kmh(),
                                 vehicle.grounded_wheels());
            }

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
                    case SDL_EVENT_MOUSE_BUTTON_DOWN:
                        if (e.button.button == SDL_BUTTON_RIGHT) {
                            mouse_drag = true;
                            last_mx = e.button.x;
                            last_my = e.button.y;
                        }
                        if (!drive_mode) {
                            if (e.button.button == SDL_BUTTON_LEFT)
                                editor.place_at_cursor();
                            if (e.button.button == SDL_BUTTON_MIDDLE)
                                editor.remove_at_cursor();
                        }
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