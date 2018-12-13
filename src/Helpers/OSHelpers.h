#ifndef OS_H
#define OS_H

#include <fstream>

class OSHelpers {
 public:
  static bool file_exists(const std::string &name) {
    std::ifstream f(name.c_str());
    return f.good();
  }
};

#endif // OS_H
