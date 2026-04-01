#include "platform/Window.h"
#include <SDL3/SDL.h>
#include <gtest/gtest.h>

class WindowTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            GTEST_SKIP() << "No display available (headless env?): "
                         << SDL_GetError();
        }
    }

    void TearDown() override { SDL_Quit(); }
};

TEST_F(WindowTest, CreateWithDefaultConfig)
{
    auto result = trackmini::platform::Window::create();
    ASSERT_TRUE(result.has_value())
      << "Window creation failed: " << result.error().message;
}

TEST_F(WindowTest, CreateWithCustomConfig)
{
    auto result = trackmini::platform::Window::create({
      .title = "Test Window",
      .width = 800,
      .height = 600,
      .vsync = false,
    });

    ASSERT_TRUE(result.has_value())
      << "Window creation failed: " << result.error().message;

    EXPECT_EQ(result->width(), 800);
    EXPECT_EQ(result->height(), 600);
}

TEST_F(WindowTest, SDLHandleIsValid)
{
    auto result = trackmini::platform::Window::create();
    ASSERT_TRUE(result.has_value());
    EXPECT_NE(result->sdl_handle(), nullptr);
}

TEST_F(WindowTest, MoveConstructor)
{
    auto r1 = trackmini::platform::Window::create();
    ASSERT_TRUE(r1.has_value());

    trackmini::platform::Window w2 = std::move(*r1);
    EXPECT_NE(w2.sdl_handle(), nullptr);
}

TEST_F(WindowTest, SwapBuffersDoesNotCrash)
{
    auto result = trackmini::platform::Window::create();
    ASSERT_TRUE(result.has_value());
    EXPECT_NO_FATAL_FAILURE({
        result->swap_buffers();
        result->swap_buffers();
    });
}