#include "window.h"
#include <print>

int main(void) {
  const auto cb = []() { glClear(GL_COLOR_BUFFER_BIT); };

  const auto cb2 = []() {
    glClearColor(1, 0.5, 0, 0.5);
    glClear(GL_COLOR_BUFFER_BIT);
  };

  try {
    std::vector<Window> windows{};

    windows.emplace_back(640, 480, "Hello World", cb);
    windows.emplace_back(600, 900, "Hello World 2", cb2);

    while (!windows.empty()) {
      Window::pollEvents();

      auto it = windows.begin();
      while (it != windows.end()) {
        auto &win = *it;

        if (win.shouldClose()) {
          it = windows.erase(it);
        } else {
          win.render();
          ++it;
        }
      }
    }
  } catch (const std::exception &e) {
    std::println("Window failure: {}", e.what());
  }

  return 0;
}
