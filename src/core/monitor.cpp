#include <core/monitor.hpp>

#include <algorithm>
#include <array>

namespace Lain8 {
namespace {

constexpr std::array<SDL_Scancode, 16> key_map{
    SDL_SCANCODE_5, SDL_SCANCODE_6, SDL_SCANCODE_7, SDL_SCANCODE_8,
    SDL_SCANCODE_T, SDL_SCANCODE_Y, SDL_SCANCODE_U, SDL_SCANCODE_I,
    SDL_SCANCODE_G, SDL_SCANCODE_H, SDL_SCANCODE_J, SDL_SCANCODE_K,
    SDL_SCANCODE_B, SDL_SCANCODE_N, SDL_SCANCODE_M, SDL_SCANCODE_COMMA,
};

} // namespace

Monitor::~Monitor() {
  if (texture_)
    SDL_DestroyTexture(texture_);
  if (renderer_)
    SDL_DestroyRenderer(renderer_);
  if (window_)
    SDL_DestroyWindow(window_);
  if (initialized_)
    SDL_Quit();
}

bool Monitor::open() {
  initialized_ = SDL_Init(SDL_INIT_VIDEO);
  if (!initialized_)
    return false;

  constexpr int width = Emulator::display_width;
  constexpr int height = Emulator::display_height;
  window_ = SDL_CreateWindow("Lain-8", width * 10, height * 10,
                             SDL_WINDOW_RESIZABLE);
  if (!window_)
    return false;

  renderer_ = SDL_CreateRenderer(window_, nullptr);
  if (!renderer_ ||
      !SDL_SetRenderLogicalPresentation(renderer_, width, height,
                                        SDL_LOGICAL_PRESENTATION_LETTERBOX))
    return false;

  texture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGBA32,
                               SDL_TEXTUREACCESS_STREAMING, width, height);
  return texture_ && SDL_SetTextureScaleMode(texture_, SDL_SCALEMODE_NEAREST);
}

Monitor::Events Monitor::poll_events() {
  Events events;
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_EVENT_QUIT:
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
      events.close_requested = true;
      return events;
    case SDL_EVENT_WINDOW_EXPOSED:
    case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
      events.redraw_requested = true;
      break;
    case SDL_EVENT_KEY_DOWN:
      if (!event.key.repeat && !events.pressed_key) {
        const auto key = std::ranges::find(key_map, event.key.scancode);
        if (key != key_map.end())
          events.pressed_key = static_cast<std::uint8_t>(key - key_map.begin());
      }
      break;
    }
  }

  if (SDL_GetKeyboardFocus() == window_) {
    const bool *keys = SDL_GetKeyboardState(nullptr);
    for (std::size_t i = 0; i < key_map.size(); ++i)
      events.keys[i] = keys[key_map[i]];
  }
  return events;
}

bool Monitor::present(const Emulator::Framebuffer &framebuffer) {
  std::array<std::uint32_t, Emulator::Framebuffer{}.size()> pixels;
  std::ranges::transform(framebuffer, pixels.begin(), [](bool lit) {
    return lit ? std::uint32_t{0xFFFFFFFF} : std::uint32_t{0};
  });
  constexpr int pitch = Emulator::display_width * sizeof(pixels.front());
  return SDL_UpdateTexture(texture_, nullptr, pixels.data(), pitch) &&
         SDL_RenderClear(renderer_) &&
         SDL_RenderTexture(renderer_, texture_, nullptr, nullptr) &&
         SDL_RenderPresent(renderer_);
}

} // namespace Lain8
