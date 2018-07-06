#ifndef OBJECTHELPERS_H
#define OBJECTHELPERS_H

class ObjectHelpers {
public:

  template <typename T>
  static void delete_pointer(T*& p) {
    if (p != nullptr) {
      delete p;
      p = nullptr;
    }
  }
};


#endif // OBJECTHELPERS_H
