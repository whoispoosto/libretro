#pragma once

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <functional>
#include <string>
#include <type_traits>

class Window {
  // TODO: have callbacks be std::move_only_function once clang supports it
  // https://github.com/llvm/llvm-project/pull/94670
  template <typename> struct NoExceptFunctor;

  template <typename R, typename... Args> struct NoExceptFunctor<R(Args...)> {
  public:
    NoExceptFunctor() = default;

    /* The underlying functor must be noexcept.
     * R must be default constructible (or void) so operator() can return
     * a default value when called on an empty functor. */
    template <typename F>
      requires std::is_nothrow_invocable_r_v<R, F, Args...> &&
               (std::is_default_constructible_v<R> || std::is_void_v<R>)
    NoExceptFunctor(F &&f) : functor_{std::forward<F>(f)} {}

    explicit operator bool() const noexcept {
      return static_cast<bool>(functor_);
    }

    R operator()(Args... args) const noexcept {
      if constexpr (std::is_void_v<R>) {
        if (functor_) {
          functor_(std::forward<Args>(args)...);
        }
      } else {
        return functor_ ? functor_(std::forward<Args>(args)...) : R{};
      }
    }

  private:
    std::function<R(Args...)> functor_{};
  };

public:
  Window(size_t width, size_t height, const std::string &title,
         const NoExceptFunctor<void()> &init_cb,
         NoExceptFunctor<void()> render_cb, NoExceptFunctor<void()> cleanup_cb);

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
  NoExceptFunctor<void()> render_cb_{};
  NoExceptFunctor<void()> cleanup_cb_{};

  static size_t window_count_;

  void swap(Window &other);
  bool load_context() noexcept;
};
