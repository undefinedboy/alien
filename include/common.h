#ifndef ALIEN_COMMON_H
#define ALIEN_COMMON_H

#define DEBUG_GC
#define TRACE_EXECUTION

namespace alien {

enum ExitCode {
  EX_USAGE = 64,
  EX_UNAVAILABLE = 69,
};

}

#endif