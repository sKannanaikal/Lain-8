#include <core/session.hpp>

#include <core/emulator.hpp>
#include <core/monitor.hpp>

#include <chrono>
#include <print>
#include <thread>

namespace Lain8 {

void Session::run(std::span<const std::uint8_t> program) {
  Emulator emulator;
  if (!emulator.load_program(program)) {
    std::println("Invalid ROM size");
    return;
  }

  Monitor monitor;
  if (!monitor.open()) {
    std::println("Could not open emulator window ({})", SDL_GetError());
    return;
  }

  using Clock = std::chrono::steady_clock;
  using TimerPeriod = std::chrono::duration<std::int64_t, std::ratio<1, 60>>;
  const auto start = Clock::now();
  TimerPeriod previous_tick{0};

  for (;;) {
    const auto events = monitor.poll_events();
    if (events.close_requested)
      return;
    emulator.set_keys(events.keys, events.pressed_key);

    const auto current_tick =
        std::chrono::duration_cast<TimerPeriod>(Clock::now() - start);
    emulator.tick_timers((current_tick - previous_tick).count());
    previous_tick = current_tick;

    emulator.step();

    if (emulator.display_dirty() || events.redraw_requested) {
      if (!monitor.present(emulator.framebuffer())) {
        std::println("Could not display emulator frame ({})", SDL_GetError());
        return;
      }
      emulator.acknowledge_frame();
    }

    const auto next_tick =
        std::chrono::ceil<Clock::duration>(current_tick + TimerPeriod{1});
    std::this_thread::sleep_until(start + next_tick);
  }
}

} // namespace Lain8
