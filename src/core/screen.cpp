#include "screen.hpp"
#include <print>

namespace Lain8 {

Screen::Screen() {

  SDL_Init(SDL_INIT_VIDEO);
  this->mWindow = SDL_CreateWindow("Lain-8", 640, 320, SDL_WINDOW_RESIZABLE);

  this->mRenderer = SDL_CreateRenderer(this->mWindow, nullptr);
  SDL_SetRenderLogicalPresentation(this->mRenderer, 64, 32,
                                   SDL_LOGICAL_PRESENTATION_LETTERBOX);

  this->mPixelBuffer[2048] = {};
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
void Screen::mainLoop() {
  // intentionally slow down this loop to match the normal og frame rate of the
  // thing
  SDL_Event event;
  while (1) {
    SDL_PollEvent(&event);
    if (event.type == SDL_EVENT_QUIT)
      break;
    std::lock_guard<std::mutex> lock(mPixelBufferMutex);
    if (updatedFrame) {
      if (SDL_UpdateTexture(this->mTexture, NULL, this->mPixelBuffer, 256)) {
        updatedFrame = false;
      }
    }
    SDL_RenderClear(this->mRenderer);
    SDL_RenderTexture(this->mRenderer, this->mTexture, nullptr, nullptr);
    SDL_RenderPresent(this->mRenderer);
  }
}

Screen::~Screen() {
  SDL_DestroyWindow(this->mWindow);
  SDL_DestroyRenderer(this->mRenderer);
  SDL_DestroySurface(this->mSurface);
  SDL_DestroyTexture(this->mTexture);
  SDL_Quit();
}

} // namespace Lain8
