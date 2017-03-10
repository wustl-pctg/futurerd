#pragma once

#include <cassert>
#include <cstdio>

#ifndef DNDEBUG
#define DBG_TRACE(fmt, args...)                                      \
  futurerd::debug::printf(futurerd::debug::TRACE,                    \
                          "[%s]: " fmt "\n", __func__, ## args)
#else
#define DBG_TRACE
#endif

namespace futurerd {

namespace debug {

enum DebugLevel : int {
  NONE = 0,
  BASIC = 1,
  TRACE = 2,
  CALLBACK = 4,
  MEMORY = 8
};

static DebugLevel g_level = futurerd::debug::BASIC;

[[noreturn]] void die(const char *fmt, ...);
void printf(DebugLevel level, const char *fmt, ...);

} // namespace debug

} // namespace futurerd
