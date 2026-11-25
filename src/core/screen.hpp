#ifndef CORE_SCREEN_HPP
#define CORE_SCREEN_HPP

#include <SDL3/SDL.h>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <optional>

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

  // Returns no result if the screen loop has exited.
  std::optional<bool> examineKeyStates(std::uint8_t keyCode);
  std::optional<std::uint8_t> waitForPressedKey();

  void mainLoop(); // keep rendering the pixel_buffer for eterneity until quit
                   // signal comes through might have to do something like
                   // update buffer instead of rerendering it all
private:
  std::mutex mPixelBufferMutex;
  bool updatedFrame = true;
  bool keyStateRequested = false;
  std::condition_variable keyStateReady;
  bool screenLoopExited = false;
  std::uint8_t desiredKey = 0;
  bool desiredKeyPressed = false;
  bool waitingForPressedKey = false;
  std::optional<std::uint8_t> pressedKey;
};

} // namespace Lain8

#endif /* CORE_SCREEN_HPP */
