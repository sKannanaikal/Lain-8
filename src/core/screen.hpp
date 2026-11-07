#ifndef CORE_SCREEN_HPP
#define CORE_SCREEN_HPP

#include <SDL3/SDL.h>
#include <cstdint>
#include <mutex>

namespace Lain8 {

class Screen {

public:
  SDL_Window *mWindow;
  SDL_Surface *mSurface;
  SDL_Renderer *mRenderer;
  SDL_Texture *mTexture;
  std::uint32_t mPixelBuffer[2048]; // update these to use std::array

  Screen();
  ~Screen();

  void render(
      std::uint32_t screen_buffer[]); // the emulator passes its screen buffer
                                      // here to update the buffer here

  void mainLoop(); // keep rendering the pixel_buffer for eterneity until quit
                   // signal comes through might have to do something like
                   // update buffer instead of rerendering it all
private:
  std::mutex mPixelBufferMutex;
  bool updatedFrame = true;
};

} // namespace Lain8

#endif /* CORE_SCREEN_HPP */
