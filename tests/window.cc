#include <gtest/gtest.h>

#include "window.h"

#include <stdexcept>

// ---------------------------------------------------------------------------
// Helper: create a window with no-op callbacks
// ---------------------------------------------------------------------------
static Window makeWindow(size_t w = 800, size_t h = 600,
                         std::string_view title = "Test") {
  return Window{
      w, h, title, []() noexcept {}, []() noexcept {}, []() noexcept {}};
}

// ---------------------------------------------------------------------------
// Accessors
// ---------------------------------------------------------------------------
TEST(WindowAccessors, Width) {
  auto w = makeWindow(1280, 720);
  EXPECT_EQ(w.width(), 1280u);
}

TEST(WindowAccessors, Height) {
  auto w = makeWindow(1280, 720);
  EXPECT_EQ(w.height(), 720u);
}

TEST(WindowAccessors, Title) {
  auto w = makeWindow(800, 600, "MyTitle");
  EXPECT_EQ(w.title(), "MyTitle");
}

// ---------------------------------------------------------------------------
// Callbacks
// ---------------------------------------------------------------------------
TEST(WindowCallbacks, InitCallbackIsCalled) {
  bool called = false;
  Window w{800,
           600,
           "Test",
           [&called]() noexcept { called = true; },
           []() noexcept {},
           []() noexcept {}};
  EXPECT_TRUE(called);
}

TEST(WindowCallbacks, RenderCallbackIsCalled) {
  bool called = false;
  Window w{800,
           600,
           "Test",
           []() noexcept {},
           [&called]() noexcept { called = true; },
           []() noexcept {}};
  w.render();
  EXPECT_TRUE(called);
}

TEST(WindowCallbacks, CleanupCallbackCalledOnScopeExit) {
  bool called = false;
  {
    Window w{800,
             600,
             "Test",
             []() noexcept {},
             []() noexcept {},
             [&called]() noexcept { called = true; }};
  }
  EXPECT_TRUE(called);
}

TEST(WindowCallbacks, CleanupCallbackCalledOnExplicitDestroy) {
  bool called = false;
  Window w{800,
           600,
           "Test",
           []() noexcept {},
           []() noexcept {},
           [&called]() noexcept { called = true; }};
  w.destroy();
  EXPECT_TRUE(called);
}

TEST(WindowCallbacks, RenderCallbackNotCalledBeforeRender) {
  bool called = false;
  Window w{800,
           600,
           "Test",
           []() noexcept {},
           [&called]() noexcept { called = true; },
           []() noexcept {}};
  EXPECT_FALSE(called);
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------
TEST(WindowLifecycle, ShouldCloseReturnsFalseAfterConstruction) {
  auto w = makeWindow();
  EXPECT_FALSE(w.shouldClose());
}

TEST(WindowLifecycle, ShouldCloseReturnsTrueAfterDestroy) {
  auto w = makeWindow();
  w.destroy();
  EXPECT_TRUE(w.shouldClose());
}

TEST(WindowLifecycle, DestroyIsIdempotent) {
  auto w = makeWindow();
  w.destroy();
  EXPECT_NO_THROW(w.destroy());
  EXPECT_NO_THROW(w.destroy());
}

TEST(WindowLifecycle, RenderOnDestroyedWindowThrows) {
  auto w = makeWindow();
  w.destroy();
  EXPECT_THROW(w.render(), std::runtime_error);
}

TEST(WindowLifecycle, RenderCanBeCalledMultipleTimes) {
  int count = 0;
  Window w{800,
           600,
           "Test",
           []() noexcept {},
           [&count]() noexcept { ++count; },
           []() noexcept {}};
  w.render();
  w.render();
  w.render();
  EXPECT_EQ(count, 3);
}

// ---------------------------------------------------------------------------
// Move semantics
// ---------------------------------------------------------------------------
TEST(WindowMove, MoveConstructorTransfersState) {
  auto w1 = makeWindow(800, 600, "Original");
  Window w2{std::move(w1)};

  EXPECT_TRUE(w1.shouldClose());
  EXPECT_FALSE(w2.shouldClose());
  EXPECT_EQ(w2.width(), 800u);
  EXPECT_EQ(w2.height(), 600u);
  EXPECT_EQ(w2.title(), "Original");
}

TEST(WindowMove, MoveAssignmentTransfersState) {
  auto w1 = makeWindow(1920, 1080, "Large");
  auto w2 = makeWindow(640, 480, "Small");
  w2 = std::move(w1);

  EXPECT_TRUE(w1.shouldClose());
  EXPECT_FALSE(w2.shouldClose());
  EXPECT_EQ(w2.width(), 1920u);
  EXPECT_EQ(w2.height(), 1080u);
  EXPECT_EQ(w2.title(), "Large");
}

TEST(WindowMove, MoveConstructorTransfersRenderCallback) {
  bool called = false;
  Window w1{800,
            600,
            "Test",
            []() noexcept {},
            [&called]() noexcept { called = true; },
            []() noexcept {}};
  Window w2{std::move(w1)};
  w2.render();
  EXPECT_TRUE(called);
}

TEST(WindowMove, MoveConstructorTransfersCleanupCallback) {
  bool called = false;
  {
    Window w1{800,
              600,
              "Test",
              []() noexcept {},
              []() noexcept {},
              [&called]() noexcept { called = true; }};
    Window w2{std::move(w1)};
  }
  EXPECT_TRUE(called);
}

TEST(WindowMove, SelfMoveAssignmentIsHarmless) {
  auto w = makeWindow(800, 600, "Self");
  auto &wref = w;
  EXPECT_NO_THROW(w = std::move(wref));
  EXPECT_NO_THROW(w.shouldClose());
}

// ---------------------------------------------------------------------------
// Multiple windows
// ---------------------------------------------------------------------------
TEST(WindowMultiple, TwoWindowsCanCoexist) {
  auto w1 = makeWindow(800, 600, "W1");
  auto w2 = makeWindow(400, 300, "W2");

  EXPECT_FALSE(w1.shouldClose());
  EXPECT_FALSE(w2.shouldClose());
  EXPECT_EQ(w1.title(), "W1");
  EXPECT_EQ(w2.title(), "W2");
}

TEST(WindowMultiple, WindowsCleanUpIndependently) {
  bool cleanup1 = false, cleanup2 = false;
  {
    Window w1{800,
              600,
              "W1",
              []() noexcept {},
              []() noexcept {},
              [&cleanup1]() noexcept { cleanup1 = true; }};
    {
      Window w2{400,
                300,
                "W2",
                []() noexcept {},
                []() noexcept {},
                [&cleanup2]() noexcept { cleanup2 = true; }};
    }
    EXPECT_TRUE(cleanup2);
    EXPECT_FALSE(cleanup1);
  }
  EXPECT_TRUE(cleanup1);
}

TEST(WindowMultiple, EachWindowHasIndependentDimensions) {
  auto w1 = makeWindow(1920, 1080, "W1");
  auto w2 = makeWindow(800, 600, "W2");

  EXPECT_EQ(w1.width(), 1920u);
  EXPECT_EQ(w1.height(), 1080u);
  EXPECT_EQ(w2.width(), 800u);
  EXPECT_EQ(w2.height(), 600u);
}
