#include <GLFW/glfw3.h>
#include <exception>
#include <functional>
#include <print>
#include <stdexcept>
#include <string>
#include <utility>

class Window {
public:
  Window(size_t width, size_t height, const std::string &title,
         const std::function<void()> &callback)
      : width_{width}, height_{height}, title_{title}, callback_{callback} {
    /* Initialize the library */
    if (!glfwInit()) {
      throw std::runtime_error("Unable to initialize GLFW");
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

  ~Window() {
    if (window_) {
      glfwDestroyWindow(window_);
      --window_count_;
    }

    if (window_count_ == 0) {
      glfwTerminate();
    }
  }

  Window(const Window &) = delete;
  Window &operator=(const Window &) = delete;

  Window(Window &&other) noexcept
      : window_{std::exchange(other.window_, nullptr)},
        width_{std::exchange(other.width_, 0)},
        height_{std::exchange(other.height_, 0)},
        title_{std::exchange(other.title_, std::string{})} {}

  Window &operator=(Window &&other) noexcept {
    if (this != &other) {
      Window temp{std::move(other)};
      swap(temp);
    }

    return *this;
  }

  void render() {
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

  static void MainLoop(std::vector<std::unique_ptr<Window>> &&windows) {
    while (!windows.empty()) {
      glfwPollEvents();

      auto it = windows.begin();
      while (it != windows.end()) {
        const auto &win = *it;

        if (glfwWindowShouldClose(win->window_)) {
          windows.erase(it);
        } else {
          win->render();
          ++it;
        }
      }
    }
  }

  size_t width() const noexcept { return width_; };
  size_t height() const noexcept { return height_; };
  std::string title() const noexcept { return title_; }

private:
  GLFWwindow *window_{};
  size_t width_{};
  size_t height_{};
  std::string title_{};
  std::function<void()> callback_{};

  static size_t window_count_;

  void swap(Window &other) {
    std::swap(window_, other.window_);
    std::swap(width_, other.width_);
    std::swap(height_, other.height_);
    std::swap(title_, other.title_);
    std::swap(callback_, other.callback_);
  }
};

size_t Window::window_count_ = 0;

int main(void) {
  const auto cb = []() { glClear(GL_COLOR_BUFFER_BIT); };

  const auto cb2 = []() {
    glClearColor(1, 0, 0, 0.5);
    glClear(GL_COLOR_BUFFER_BIT);
  };

  try {
    /* Cannot use initializer list since that is for static storage only
    By definition, static storage cannot be changed (and thus cannot be moved
    from) */
    auto window_list = std::vector<std::unique_ptr<Window>>{};
    window_list.push_back(std::make_unique<Window>(640, 480, "Hello", cb));
    window_list.push_back(std::make_unique<Window>(600, 900, "Hello 2", cb2));

    /* Loop through all windows */
    Window::MainLoop(std::move(window_list));
  } catch (const std::exception &e) {
    std::println("Window creation failed: {}", e.what());
  }

  return 0;
}
