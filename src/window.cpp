#include "window.h"

size_t Window::window_count_ = 0;

Window::Window(size_t width, size_t height, const std::string &title,
               const std::function<void()> &callback)
    : width_{width}, height_{height}, title_{title}, callback_{callback} {
  /* Initialize the library */
  if (window_count_ == 0) {
    if (!glfwInit()) {
      throw std::runtime_error("Unable to initialize GLFW");
    }
  }

  /* Initialze the GLFW window */
  if (window_ =
          glfwCreateWindow(width_, height_, title_.c_str(), nullptr, nullptr);
      !window_) {
    throw std::runtime_error("Unable to create GLFW window");
  }

  /* Increment global window count */
  ++window_count_;
}

Window::~Window() { destroy(); }

Window::Window(Window &&other) noexcept
    : window_{std::exchange(other.window_, nullptr)},
      width_{std::exchange(other.width_, 0)},
      height_{std::exchange(other.height_, 0)},
      title_{std::exchange(other.title_, std::string{})},
      callback_{std::exchange(other.callback_, {})} {}

Window &Window::operator=(Window &&other) noexcept {
  if (this != &other) {
    Window temp{std::move(other)};
    swap(temp);
  }

  return *this;
}

void Window::render() {
  if (!window_) {
    throw std::runtime_error("Window is nullptr");
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window_);

  /* OpenGL rendering */
  callback_();

  /* Swap front and back buffers */
  glfwSwapBuffers(window_);
}

bool Window::shouldClose() const noexcept {
  return !window_ || glfwWindowShouldClose(window_);
}

void Window::destroy() noexcept {
  /* Do nothing if window_ is nullptr */
  if (!window_) {
    return;
  }

  glfwDestroyWindow(window_);
  window_ = nullptr;

  /* Decrement window count */
  --window_count_;

  /* Terminate GLFW if all windows have been destroyed */
  if (window_count_ == 0) {
    glfwTerminate();
  }
}

void Window::pollEvents() noexcept {
  /* Refresh GLFW events */
  glfwPollEvents();
}

size_t Window::width() const noexcept { return width_; };
size_t Window::height() const noexcept { return height_; };
std::string Window::title() const noexcept { return title_; }

void Window::swap(Window &other) {
  std::swap(window_, other.window_);
  std::swap(width_, other.width_);
  std::swap(height_, other.height_);
  std::swap(title_, other.title_);
  std::swap(callback_, other.callback_);
}
