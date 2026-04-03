#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <print>

#include "engine/EventBus.h"
#include "engine/GameLoop.h"
#include "math/Math.h"
#include "platform/Window.h"
#include "renderer/Camera.h"
#include "renderer/Mesh.h"
#include "renderer/Shader.h"
#include "renderer/UniformBuffer.h"

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

    // Camera
    trackmini::renderer::Camera camera({
      .orbit_radius = 4.f,
      .orbit_pitch = trackmini::math::to_radians(25.f),
      .orbit_yaw = trackmini::math::to_radians(40.f),
    });

    trackmini::renderer::UniformBuffer<trackmini::renderer::CameraMatrices>
      cam_ubo;

    // Meshes
    trackmini::renderer::Mesh cube{ trackmini::renderer::MeshData::make_cube(
      0.5f) };
    trackmini::renderer::Mesh plane{ trackmini::renderer::MeshData::make_plane(
      8.f, 8) };

    auto const model_identity = trackmini::math::Mat4f::identity();

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

    [[maybe_unused]] auto sub_quit =
      bus.subscribe<trackmini::engine::Events::QuitRequested>(
        [&](auto const&) { running = false; });
    [[maybe_unused]] auto sub_key =
      bus.subscribe<trackmini::engine::Events::KeyPressed>(
        [&](trackmini::engine::Events::KeyPressed const& e) {
            if (e.scancode == SDL_SCANCODE_ESCAPE)
                running = false;
        });
    [[maybe_unused]] auto sub_resize =
      bus.subscribe<trackmini::engine::Events::WindowResized>(
        [&](trackmini::engine::Events::WindowResized const& e) {
            glViewport(0, 0, e.width, e.height);
            camera.set_aspect(static_cast<float>(e.width) /
                              static_cast<float>(e.height));
        });

    // Game loop
    float cube_yaw = 0.f; // cube autorotates

    trackmini::engine::GameLoop loop{ {
      .fixed_update =
        [&](trackmini::engine::Duration dt) {
            cube_yaw += static_cast<float>(dt.count()) * 0.8f; // rad/s
        },

      .update = [](trackmini::engine::Duration, double) {},

      .render =
        [&](double alpha) {
            (void)alpha;
            glClearColor(0.05f, 0.07f, 0.12f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Update cam and upload UBO
            camera.update();
            cam_ubo.upload(camera.matrices());
            cam_ubo.bind_to_slot(0); // binding = 0

            shader.bind();

            // Fixed light, up right diagonal
            float light[3] = { 0.6f, 1.f, 0.4f };
            float white[3] = { 1.f, 1.f, 1.f };
            shader.set_vec3("u_light_dir", light);
            shader.set_vec3("u_light_color", white);

            // Cube
            auto model_cube = trackmini::math::Mat4f::rotation(
              trackmini::math::Vec3f::up(), cube_yaw);
            shader.set_mat4("u_model", model_cube.data());
            cube.draw();

            // Plan
            auto model_plane =
              trackmini::math::Mat4f::translation({ 0.f, -0.6f, 0.f });
            shader.set_mat4("u_model", model_plane.data());
            plane.draw();

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
                          .scancode = e.key.scancode, .repeat = e.key.repeat });
                        break;
                    case SDL_EVENT_WINDOW_RESIZED:
                        bus.emit(trackmini::engine::Events::WindowResized{
                          .width = e.window.data1, .height = e.window.data2 });
                        break;

                    // Mouse orbit control
                    case SDL_EVENT_MOUSE_BUTTON_DOWN:
                        if (e.button.button == SDL_BUTTON_LEFT) {
                            mouse_drag = true;
                            last_mx = e.button.x;
                            last_my = e.button.y;
                        }
                        break;
                    case SDL_EVENT_MOUSE_BUTTON_UP:
                        if (e.button.button == SDL_BUTTON_LEFT)
                            mouse_drag = false;
                        break;
                    case SDL_EVENT_MOUSE_MOTION:
                        if (mouse_drag) {
                            float dx = (e.motion.x - last_mx) * 0.005f;
                            float dy = (e.motion.y - last_my) * 0.005f;
                            camera.orbit(dx, dy);
                            last_mx = e.motion.x;
                            last_my = e.motion.y;
                        }
                        break;
                    case SDL_EVENT_MOUSE_WHEEL:
                        camera.zoom(e.wheel.y * 0.3f);
                        break;
                    default:
                        break;
                }
            }
        },

            .tick_fn = {},
    } };

    loop.run();
    return 0;
}