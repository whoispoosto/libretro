#include "window.h"
#include <stdexcept>

size_t Window::window_count_ = 0;

Window::Window(size_t width, size_t height, std::string_view title,
               const NoExceptFunctor<void()> &init_cb,
               NoExceptFunctor<void()> render_cb,
               NoExceptFunctor<void()> cleanup_cb)
    : width_{width}, height_{height}, title_{title},
      render_cb_{std::move(render_cb)}, cleanup_cb_{std::move(cleanup_cb)} {

  /* Initialize the library */
  if (window_count_ == 0) {
    if (!glfwInit()) {
      throw std::runtime_error("Unable to initialize GLFW");
    }

    // TODO: throw these values in some config file?
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  }

  /* Initialze the GLFW window */
  if (window_ =
          glfwCreateWindow(width_, height_, title_.c_str(), nullptr, nullptr);
      !window_) {
    throw std::runtime_error("Unable to create GLFW window");
  }

  /* Run initialization callback on this window context */
  if (!load_context()) {
    glfwDestroyWindow(window_);
    window_ = nullptr;
    throw std::runtime_error("Unable to load GL context");
  }

  if (init_cb) {
    init_cb();
  }

  /* Increment global window count */
  ++window_count_;
}

Window::~Window() { destroy(); }

Window::Window(Window &&other) noexcept
    : window_{std::exchange(other.window_, nullptr)},
      width_{std::exchange(other.width_, 0)},
      height_{std::exchange(other.height_, 0)}, title_{std::move(other.title_)},
      render_cb_{std::move(other.render_cb_)},
      cleanup_cb_{std::move(other.cleanup_cb_)} {}

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

  /* OpenGL rendering */
  if (!load_context()) {
    throw std::runtime_error("Unable to load GL context");
  }

  if (render_cb_) {
    render_cb_();
  }

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

  /* Call custom cleanup code */
  if (cleanup_cb_ && load_context()) {
    cleanup_cb_();
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
std::string_view Window::title() const noexcept { return title_; }

void Window::swap(Window &other) {
  std::swap(window_, other.window_);
  std::swap(width_, other.width_);
  std::swap(height_, other.height_);
  std::swap(title_, other.title_);
  std::swap(render_cb_, other.render_cb_);
  std::swap(cleanup_cb_, other.cleanup_cb_);
}

bool Window::load_context() noexcept {
  /* Make the window's context current */
  glfwMakeContextCurrent(window_);

  /* Load OpenGL functions
   * Returns 0 on failure and non-zero on success */
  return gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}
