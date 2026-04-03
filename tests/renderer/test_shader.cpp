#include "platform/Window.h"
#include "renderer/Buffer.h"
#include "renderer/Shader.h"
#include "renderer/VertexArray.h"
#include <SDL3/SDL.h>
#include <gtest/gtest.h>

class RendererTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            GTEST_SKIP() << "No display: " << SDL_GetError();
        }
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                            SDL_GL_CONTEXT_PROFILE_CORE);

        // hidden window
        m_window =
          SDL_CreateWindow("test", 1, 1, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
        if (!m_window) {
            SDL_Quit();
            GTEST_SKIP() << "Cannot create window: " << SDL_GetError();
        }

        m_ctx = SDL_GL_CreateContext(m_window);
        if (!m_ctx) {
            SDL_DestroyWindow(m_window);
            SDL_Quit();
            GTEST_SKIP() << "Cannot create GL context: " << SDL_GetError();
        }

        if (!gladLoadGLLoader(
              reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
            SDL_GL_DestroyContext(m_ctx);
            SDL_DestroyWindow(m_window);
            SDL_Quit();
            GTEST_SKIP() << "Cannot load GL functions";
        }
    }

    void TearDown() override
    {
        SDL_GL_DestroyContext(m_ctx);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
    }

    SDL_Window* m_window{ nullptr };
    SDL_GLContext m_ctx{ nullptr };
};

TEST_F(RendererTest, VertexShaderCompilesFromValidSource)
{
    constexpr char kSrc[] = R"(
        #version 460 core
        layout(location = 0) in vec3 a_pos;
        void main() { gl_Position = vec4(a_pos, 1.0); }
    )";
    auto result =
      trackmini::renderer::ShaderStage::from_source(GL_VERTEX_SHADER, kSrc);
    ASSERT_TRUE(result.has_value()) << result.error().message;
    EXPECT_NE(result->handle(), 0u);
}

TEST_F(RendererTest, ShaderStageFailsOnInvalidGLSL)
{
    constexpr char kBadSrc[] = "this is not valid GLSL at all $$$$";
    auto result =
      trackmini::renderer::ShaderStage::from_source(GL_VERTEX_SHADER, kBadSrc);
    EXPECT_FALSE(result.has_value());
    EXPECT_FALSE(result.error().message.empty());
}

TEST_F(RendererTest, ProgramLinksFromValidStages)
{
    constexpr char kVert[] = R"(
        #version 460 core
        layout(location = 0) in vec3 a_pos;
        void main() { gl_Position = vec4(a_pos, 1.0); }
    )";
    constexpr char kFrag[] = R"(
        #version 460 core
        out vec4 frag_color;
        void main() { frag_color = vec4(1.0); }
    )";

    auto vert =
      trackmini::renderer::ShaderStage::from_source(GL_VERTEX_SHADER, kVert);
    auto frag =
      trackmini::renderer::ShaderStage::from_source(GL_FRAGMENT_SHADER, kFrag);
    ASSERT_TRUE(vert.has_value());
    ASSERT_TRUE(frag.has_value());

    auto prog = trackmini::renderer::ShaderProgram::from_stages(*vert, *frag);
    ASSERT_TRUE(prog.has_value()) << prog.error().message;
    EXPECT_NE(prog->handle(), 0u);
}

TEST_F(RendererTest, ProgramBindUnbindDoesNotCrash)
{
    constexpr char kVert[] = R"(
        #version 460 core
        void main() { gl_Position = vec4(0.0); }
    )";
    constexpr char kFrag[] = R"(
        #version 460 core
        out vec4 frag_color;
        void main() { frag_color = vec4(1.0); }
    )";
    auto vert =
      trackmini::renderer::ShaderStage::from_source(GL_VERTEX_SHADER, kVert);
    auto frag =
      trackmini::renderer::ShaderStage::from_source(GL_FRAGMENT_SHADER, kFrag);
    auto prog = trackmini::renderer::ShaderProgram::from_stages(*vert, *frag);
    ASSERT_TRUE(prog.has_value());

    EXPECT_NO_FATAL_FAILURE({
        prog->bind();
        prog->unbind();
    });
}

TEST_F(RendererTest, VertexBufferAllocatesWithoutError)
{
    std::array<float, 9> data{ 0, 1, 0, -1, -1, 0, 1, -1, 0 };
    trackmini::renderer::VertexBuffer vbo;
    EXPECT_NO_FATAL_FAILURE(
      vbo.allocate(data, trackmini::renderer::BufferUsage::StaticDraw));
    EXPECT_NE(vbo.handle(), 0u);
    EXPECT_EQ(vbo.count(), 9u);
}

TEST_F(RendererTest, DynamicBufferCanBeUpdated)
{
    std::array<float, 6> data{ 1, 2, 3, 4, 5, 6 };
    trackmini::renderer::VertexBuffer vbo;
    vbo.allocate(data,
                 trackmini::renderer::BufferUsage::DynamicDraw,
                 GL_DYNAMIC_STORAGE_BIT);

    std::array<float, 3> patch{ 9, 9, 9 };
    EXPECT_NO_FATAL_FAILURE(vbo.upload(patch, 0));
}

TEST_F(RendererTest, VertexArrayCreatesValidHandle)
{
    trackmini::renderer::VertexArray vao;
    EXPECT_NE(vao.handle(), 0u);
}

TEST_F(RendererTest, VertexArrayBindUnbindDoesNotCrash)
{
    trackmini::renderer::VertexArray vao;
    EXPECT_NO_FATAL_FAILURE({
        vao.bind();
        vao.unbind();
    });
}

TEST_F(RendererTest, VertexArrayWithBufferDoesNotCrash)
{
    struct V
    {
        float x, y, z, r, g, b;
    };
    std::array<V, 3> verts{ { { 0, .5f, 0, 1, 0, 0 },
                              { -.5f, -.5f, 0, 0, 1, 0 },
                              { .5f, -.5f, 0, 0, 0, 1 } } };

    trackmini::renderer::GpuBuffer<V> vbo;
    vbo.allocate(verts, trackmini::renderer::BufferUsage::StaticDraw);

    static constexpr std::array<trackmini::renderer::VertexAttrib, 2> kAttribs{
        {
          { 0, 3, GL_FLOAT, false, offsetof(V, x) },
          { 1, 3, GL_FLOAT, false, offsetof(V, r) },
        }
    };

    trackmini::renderer::VertexArray vao;
    EXPECT_NO_FATAL_FAILURE(
      vao.add_vertex_buffer(vbo.handle(), kAttribs, sizeof(V)));
}