#include "screen.hpp"
#include <array>
#include <print>

namespace Lain8 {

Screen::Screen() {

  SDL_Init(SDL_INIT_VIDEO);
  this->mWindow = SDL_CreateWindow("Lain-8", 640, 320, SDL_WINDOW_RESIZABLE);

  this->mRenderer = SDL_CreateRenderer(this->mWindow, nullptr);
  SDL_SetRenderLogicalPresentation(this->mRenderer, 64, 32,
                                   SDL_LOGICAL_PRESENTATION_LETTERBOX);

  std::fill_n(this->mPixelBuffer, 2048, 0xFFFFFFFF);
  this->mSurface = SDL_CreateSurfaceFrom(64, 32, SDL_PIXELFORMAT_RGBA32,
                                         this->mPixelBuffer, 256);
  this->mTexture =
      SDL_CreateTextureFromSurface(this->mRenderer, this->mSurface);
  SDL_SetTextureScaleMode(this->mTexture, SDL_SCALEMODE_NEAREST);
}

void Screen::render(std::uint32_t screen_buffer[]) {
  std::lock_guard<std::mutex> lock(mPixelBufferMutex);
  std::copy_n(screen_buffer, 2048, this->mPixelBuffer);
  updatedFrame = true;
}

std::optional<bool> Screen::examineKeyStates(std::uint8_t keyCode) {
  std::unique_lock<std::mutex> lock(mPixelBufferMutex);
  if (screenLoopExited)
    return std::nullopt;

  desiredKey = keyCode;
  keyStateRequested = true;
  keyStateReady.wait(lock, [this] {
    return !keyStateRequested || screenLoopExited;
  });
  if (screenLoopExited)
    return std::nullopt;
  return desiredKeyPressed;
}

std::optional<std::uint8_t> Screen::waitForPressedKey() {
  std::unique_lock<std::mutex> lock(mPixelBufferMutex);
  pressedKey.reset();
  waitingForPressedKey = true;
  keyStateReady.wait(lock, [this] {
    return pressedKey.has_value() || screenLoopExited;
  });
  waitingForPressedKey = false;
  return pressedKey;
}

void Screen::mainLoop() {
  constexpr std::array<SDL_Scancode, 16> keyMap{
      SDL_SCANCODE_5, SDL_SCANCODE_6, SDL_SCANCODE_7, SDL_SCANCODE_8,
      SDL_SCANCODE_T, SDL_SCANCODE_Y, SDL_SCANCODE_U, SDL_SCANCODE_I,
      SDL_SCANCODE_G, SDL_SCANCODE_H, SDL_SCANCODE_J, SDL_SCANCODE_K,
      SDL_SCANCODE_B, SDL_SCANCODE_N, SDL_SCANCODE_M, SDL_SCANCODE_COMMA,
  };

  // intentionally slow down this loop to match the normal og frame rate of the
  // thing
  const bool *keys = SDL_GetKeyboardState(nullptr);
  SDL_Event event;
  while (true) {
    if (SDL_PollEvent(&event) && event.type == SDL_EVENT_QUIT)
      break;
    std::lock_guard<std::mutex> lock(mPixelBufferMutex);
    if (keyStateRequested) {
      desiredKeyPressed = desiredKey < keyMap.size() &&
                          keys[keyMap[desiredKey]];
      keyStateRequested = false;
      keyStateReady.notify_one();
    }
    if (waitingForPressedKey && !pressedKey.has_value()) {
      for (std::uint8_t keyCode = 0; keyCode < keyMap.size(); ++keyCode) {
        if (keys[keyMap[keyCode]]) {
          pressedKey = keyCode;
          keyStateReady.notify_one();
          break;
        }
      }
    }
    if (updatedFrame) {
      if (SDL_UpdateTexture(this->mTexture, NULL, this->mPixelBuffer, 256)) {
        updatedFrame = false;
      }
    }
    SDL_RenderClear(this->mRenderer);
    SDL_RenderTexture(this->mRenderer, this->mTexture, nullptr, nullptr);
    SDL_RenderPresent(this->mRenderer);
  }
  {
    std::lock_guard<std::mutex> lock(mPixelBufferMutex);
    screenLoopExited = true;
  }
  keyStateReady.notify_all();
}

Screen::~Screen() {
  SDL_DestroyWindow(this->mWindow);
  SDL_DestroyRenderer(this->mRenderer);
  SDL_DestroySurface(this->mSurface);
  SDL_DestroyTexture(this->mTexture);
  SDL_Quit();
}

} // namespace Lain8
