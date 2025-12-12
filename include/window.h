#pragma once

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <functional>
#include <string>

class Window {
public:
  Window(size_t width, size_t height, const std::string &title,
         const std::function<void()> &init_cb, std::function<void()> render_cb,
         std::function<void()> cleanup_cb);

  ~Window();

  Window(const Window &) = delete;
  Window &operator=(const Window &) = delete;

  Window(Window &&other) noexcept;
  Window &operator=(Window &&other) noexcept;

  void render();

  bool shouldClose() const noexcept;

  void destroy() noexcept;

  static void pollEvents() noexcept;

  size_t width() const noexcept;
  size_t height() const noexcept;
  std::string_view title() const noexcept;

private:
  GLFWwindow *window_{};
  size_t width_{};
  size_t height_{};
  std::string title_;
  std::function<void()> render_cb_{};
  std::function<void()> cleanup_cb_{};

  static size_t window_count_;

  void swap(Window &other);
  bool load_context() noexcept;
};
