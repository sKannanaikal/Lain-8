/** @file screen.hpp
 * @brief SDL display and synchronized keyboard input.
 */

#ifndef CORE_SCREEN_HPP
#define CORE_SCREEN_HPP

#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <optional>
#include <SDL3/SDL.h>

namespace Lain8 {

/// @brief Bridges SDL rendering and input with the emulator's CPU thread.
class Screen {

public:
  SDL_Window *mWindow;              ///< Window that hosts the CHIP-8 display.
  SDL_Surface *mSurface;            ///< Surface backed by the pixel buffer.
  SDL_Renderer *mRenderer;          ///< Renderer associated with the window.
  SDL_Texture *mTexture;            ///< Texture presented by the screen loop.
  std::uint32_t mPixelBuffer[2048]; ///< Host-side 64 by 32 RGBA32 framebuffer.

  /// @brief Initialize SDL video and allocate the display resources.
  Screen();

  /// @brief Release display resources and shut down SDL.
  ~Screen();

  /**
   * @brief Copy a frame into the shared buffer and mark it ready for upload.
   * @param screen_buffer Source containing 2048 readable RGBA32 pixels.
   */
  void render(std::uint32_t screen_buffer[]);

  /**
   * @brief Wait for the screen loop to sample a single CHIP-8 key.
   * @param keyCode CHIP-8 key code in the range 0x0 through 0xF.
   * @return Whether the key is held, or std::nullopt if the loop has exited.
   * @note Call from the CPU thread. Only one keyboard request may be pending.
   */
  std::optional<bool> examineKeyStates(std::uint8_t keyCode);

  /**
   * @brief Wait until the screen loop sees a mapped key held down.
   * @return The CHIP-8 code of the first held key in mapping order, or
   * std::nullopt if the loop exits before a key is available.
   * @note Call from the CPU thread. A key already held satisfies the wait.
   */
  std::optional<std::uint8_t> waitForPressedKey();

  /**
   * @brief Poll SDL input, service keyboard requests, and present frames.
   * @note Run on the SDL main thread. Exiting wakes any pending CPU wait.
   */
  void mainLoop();

private:
  /// @brief Guards the framebuffer and all keyboard request and result state.
  std::mutex mPixelBufferMutex;
  bool updatedFrame = true;
  bool keyStateRequested = false;
  /// @brief Wakes the CPU when a keyboard result arrives or the loop exits.
  std::condition_variable keyStateReady;
  bool screenLoopExited = false;
  std::uint8_t desiredKey = 0;
  bool desiredKeyPressed = false;
  bool waitingForPressedKey = false;
  std::optional<std::uint8_t> pressedKey;
};

} // namespace Lain8

#endif /* CORE_SCREEN_HPP */
