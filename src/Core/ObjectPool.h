/* 
 * File:   ObjectPool.h
 * Author: Nguyen Tran
 *
 * Created on May 4, 2012, 9:45 AM
 * 
 * Things need to remember when using Object Pool:
 *      - Add initialize variable in implement file (.cpp file) like: 
 *              ObjectPool<ClassName>* ClassName::object_pool = nullptr;
 *      - Remember to call Initialize() and Release() in the main program
 *      - Be careful with inheritance, normally just apply ObjectPool to the derived class (child class)  
 * 
 * 
 * NOTICE: - Be careful with Pointer to an object need to be initialize and delete in constructor and destructor, one
 * possible work around is using default value in constructor
 * 
 */

#ifndef OBJECTPOOL_H
#define OBJECTPOOL_H

#include <vector>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <typeinfo>
#include <cassert>

#define OBJECTPOOL_IMPL(class_name)\
    ObjectPool<class_name>* class_name::object_pool = nullptr;

#define OBJECTPOOL(class_name)\
  private:\
    static ObjectPool<class_name>* object_pool;\
  public:\
    static void InitializeObjectPool(const int& size = EXPANSION_SIZE){ if (object_pool == nullptr) object_pool = new ObjectPool<class_name>(size);}\
    static void ReleaseObjectPool(){delete object_pool;object_pool=nullptr;}

enum {
  EXPANSION_SIZE = 100000
};

template<class T>
class ObjectPool {
 public:

 public:
  explicit ObjectPool(const size_t &size = EXPANSION_SIZE);

  ~ObjectPool(void);

 public:
  T *alloc();

  void free(T *some_element);

 private:
  void expand_free_list();

  size_t expansion_size_{EXPANSION_SIZE};

  /**
   * This is a vector of array of Objects T
   */
  std::vector<T *> allObjects;
  /**
   * This a Queue of Pointers T (not array)
   */
  std::vector<T *> freeList;

};

template<class T>
ObjectPool<T>::ObjectPool(const size_t &size /*= EXPANSION_SIZE*/) {
  if (size <= 0) {
    throw std::invalid_argument("expansion size must be positive");
  }
  this->expansion_size_ = size;
  freeList.reserve(expansion_size_);
  expand_free_list();
}

template<class T>
ObjectPool<T>::~ObjectPool(void) {
  // std::cout << typeid (T).name() << "\tall: " << allObjects.size() * expansionSize << "\tfree: " << freeList.size() << std::endl;

  if (allObjects.size()*expansion_size_!=freeList.size()) {
    std::cout << typeid(T).name() << "\tall: " << allObjects.size()*expansion_size_ << "\tfree: "
              << freeList.size() << std::endl;
    assert(false);
  }

  // TODO: Investigate why? Infant and NonInfantImmune delete segmentation dump

  for (int i = 0; i < (int) allObjects.size(); i++) {
    //        std::cout << typeid (T).name() << ": delete " << i << "-" << allObjects.size() << std::endl;
    delete[] allObjects[i];
  }

  //for_each(allObjects.begin(), allObjects.end(), ArrayDeleteObject);
}

template<class T>
T *ObjectPool<T>::alloc() {
  //    std::cout << "Alloc:" << typeid (T).name() << std::endl;
  if (freeList.empty()) {
    expand_free_list();
  }

  T *obj = freeList.back();
  freeList.pop_back();

  return obj;
}

template<class T>
void ObjectPool<T>::free(T *some_element) {
  if (some_element==nullptr) {
    std::cout << typeid(T).name() << " Free failed!!!" << std::endl;
    assert(false);
  }

  freeList.push_back(some_element);
}

template<class T>
void ObjectPool<T>::expand_free_list() {
  auto *new_objects = new T[expansion_size_];
  allObjects.push_back(new_objects);

  for (std::size_t i = 0; i < expansion_size_; i++) {
    freeList.push_back(&new_objects[i]);
  }
  //    std::cout << typeid (T).name() << " - Size: " << allObjects.size() * expansionSize << std::endl;
}

#endif /* //OBJECTPOOL_H */
