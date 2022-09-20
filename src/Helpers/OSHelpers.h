/*
 * OSHelpers.hxx
 *
 * This header contains some useful functions that are intended to be cross-platform compatible.
 */

#ifndef OS_HXX
#define OS_HXX

#include <fstream>

#include "sys/sysinfo.h"
#include "sys/types.h"

class OsHelpers {
 public:
  static bool file_exists(const std::string &name) {
    const std::ifstream f(name.c_str());
    return f.good();
  }

  static int
};

#endif
