#ifndef CORE_SESSION_HPP
#define CORE_SESSION_HPP

#include <cstdint>
#include <span>

namespace Lain8 {

/// Runs one ROM and its display on the calling main thread.
class Session {
public:
  void run(std::span<const std::uint8_t> program);
};

} // namespace Lain8
#endif
