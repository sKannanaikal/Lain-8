#ifndef CORE_MONITOR_HPP
#define CORE_MONITOR_HPP

#include <core/emulator.hpp>

#include <SDL3/SDL.h>

namespace Lain8 {

/// Owns the SDL window and translates between SDL and CHIP-8 input and pixels.
class Monitor {
public:
  struct Events {
    Emulator::Keypad keys{};
    std::optional<std::uint8_t> pressed_key;
    bool close_requested = false;
    bool redraw_requested = false;
  };

  Monitor() = default;
  ~Monitor();
  Monitor(const Monitor &) = delete;
  Monitor &operator=(const Monitor &) = delete;

  bool open();
  Events poll_events();
  bool present(const Emulator::Framebuffer &framebuffer);

private:
  bool initialized_ = false;
  SDL_Window *window_ = nullptr;
  SDL_Renderer *renderer_ = nullptr;
  SDL_Texture *texture_ = nullptr;
};

} // namespace Lain8
#endif
