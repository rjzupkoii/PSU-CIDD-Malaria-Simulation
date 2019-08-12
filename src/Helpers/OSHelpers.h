#ifndef OS_H
#define OS_H

#include <fstream>

class OsHelpers {
 public:
  static bool file_exists(const std::string &name) {
    const std::ifstream f(name.c_str());
    return f.good();
  }
};

#endif // OS_H
