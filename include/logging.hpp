#pragma once

#include <iostream>

namespace logging {

class Logger {
public:
  template <typename... Args> static void debug(Args &&...args) {
#ifndef NO_LOGGING
    (std::cout << ... << args) << std::endl;
#endif
  }
};

} // namespace logging
