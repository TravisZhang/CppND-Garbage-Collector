#include "gc_details.h"
#include "gc_iterator.h"
#include <cstdlib>
#include <iostream>
#include <list>
#include <typeinfo>
/*
    Pointer implements a pointer type that uses
    garbage collection to release unused memory.
    A Pointer must only be used to point to memory
    that was dynamically allocated using new.
    When used to refer to an allocated array,
    specify the array size.
*/
template <class T, int size = 0> class Pointer {
private:
  // refContainer maintains the garbage collection list.
  static std::list<PtrDetails<T>> refContainer;
  // addr points to the allocated memory to which
  // this Pointer pointer currently points.
  T *addr;
  /*  isArray is true if this Pointer points
      to an allocated array. It is false
      otherwise.
  */
  bool isArray;
  // true if pointing to array
  // If this Pointer is pointing to an allocated
  // array, then arraySize contains its size.
  unsigned arraySize; // size of the array
  static bool first;  // true when first Pointer is created
  // Return an iterator to pointer details in refContainer.
  typename std::list<PtrDetails<T>>::iterator findPtrInfo(T *ptr);

public:
  // Define an iterator type for Pointer<T>.
  typedef Iter<T> GCiterator;
  // Empty constructor
  // NOTE: templates aren't able to have prototypes with default arguments
  // this is why constructor is designed like this:
  Pointer() { Pointer(NULL); }
  Pointer(T *);
  // Copy constructor.
  Pointer(const Pointer &);
  // Destructor for Pointer.
  ~Pointer();
  // Collect garbage. Returns true if at least
  // one object was freed.
  static bool collect();
  // Overload assignment of pointer to Pointer.
  T *operator=(T *t);
  // Overload assignment of Pointer to Pointer.
  Pointer &operator=(Pointer &rv);
  // Return a reference to the object pointed
  // to by this Pointer.
  T &operator*() { return *addr; }
  // Return the address being pointed to.
  T *operator->() { return addr; }
  // Return a reference to the object at the
  // index specified by i.
  T &operator[](int i) { return addr[i]; }
  // Conversion function to T *.
  operator T *() { return addr; }
  // Return an Iter to the start of the allocated memory.
  Iter<T> begin() {
    int _size;
    if (isArray)
      _size = arraySize;
    else
      _size = 1;
    return Iter<T>(addr, addr, addr + _size);
  }
  // Return an Iter to one past the end of an allocated array.
  Iter<T> end() {
    int _size;
    if (isArray)
      _size = arraySize;
    else
      _size = 1;
    return Iter<T>(addr + _size, addr, addr + _size);
  }
  // Return the size of refContainer for this type of Pointer.
  static int refContainerSize() { return refContainer.size(); }
  // A utility function that displays refContainer.
  static void showlist();
  // Clear refContainer when program exits.
  static void shutdown();
};

// STATIC INITIALIZATION
// Creates storage for the static variables
template <class T, int size>
std::list<PtrDetails<T>> Pointer<T, size>::refContainer;
template <class T, int size> bool Pointer<T, size>::first = true;

// Constructor for both initialized and uninitialized objects. -> see class
// interface
template <class T, int size> Pointer<T, size>::Pointer(T *t) {
  // Register shutdown() as an exit function.
  if (first)
    atexit(shutdown);
  first = false;

  // TODO: Implement Pointer constructor
  // Lab: Smart Pointer Project Lab
  typename std::list<PtrDetails<T>>::iterator p = findPtrInfo(t);
  if (p == refContainer.end())
    refContainer.push_back(PtrDetails<T>(t, size));
  else
    p->refcount++;
  addr = t;
  isArray = (size > 0);
  arraySize = size;
}
// Copy constructor.
// p will not be nullptr since ob already exists
template <class T, int size> Pointer<T, size>::Pointer(const Pointer &ob) {

  // TODO: Implement Pointer constructor
  // Lab: Smart Pointer Project Lab

  typename std::list<PtrDetails<T>>::iterator p = findPtrInfo(ob.addr);
  p->refcount++;
  addr = ob.addr;
  arraySize = ob.arraySize;
  isArray = ob.isArray;
}

// Destructor for Pointer.
template <class T, int size> Pointer<T, size>::~Pointer() {

  // TODO: Implement Pointer destructor
  // Lab: New and Delete Project Lab
  typename std::list<PtrDetails<T>>::iterator p = findPtrInfo(addr);
  if (p->refcount > 0)
    p->refcount--;
  collect();
}

// Collect garbage. Returns true if at least
// one object was freed.
// template <class T, int size> bool Pointer<T, size>::collect() {

//   // TODO: Implement collect function
//   // LAB: New and Delete Project Lab
//   // Note: collect() will be called in the destructor
//   bool memfreed = false;
//   typename std::list<PtrDetails<T>>::iterator p;
//   // Scan refContainer looking for unreferenced pointers.
//   for (p = refContainer.begin(); p != refContainer.end();) {
//     // TODO: Implement collect()
//     // If in-use, skip.
//     std::cout << "-------------- for loop begin --------------" << std::endl;
//     std::cout << "refContainer.size: " << refContainerSize() << std::endl;
//     // showlist();
//     if (p->refcount > 0) {
//       std::cout << "p in use, skipping" << std::endl;
//       p++;
//       continue;
//     }
//     std::cout << "p not in use, if p->memPtr == nullptr: "
//               << (p->memPtr == nullptr) << std::endl;
//     // Free memory unless the Pointer is null.
//     if (p->memPtr) {
//       std::cout << "p->memPtr != nullptr "
//                 << "p->isArray: " << (p->isArray) << std::endl;
//       std:: cout << "p node value: " << p->memPtr->value << std::endl;
//       if (p->isArray) {
//         std::cout << "deleting array" << std::endl;
//         delete[] p->memPtr;
//       } else {
//         std::cout << "deleting number" << std::endl;
//         delete p->memPtr;
//         std::cout << "just get out of delete" << std::endl;
//       }
//       std::cout << "p->memPtr deleted " << std::endl;
//     }
//     // Remove unused entry from refContainer.
//     std::cout << "erasing p "
//               << "if p->memPtr == nullptr: " << (p->memPtr == nullptr)
//               << std::endl;
//     std::cout << "refContainer.size: " << refContainerSize() << std::endl;
//     p = refContainer.erase(p);
//     // refContainer.erase(p++);
//     std::cout << "erasing done" << std::endl;
//     std::cout << "refContainer.size: " << refContainerSize() << std::endl;
//     std::cout << "if p == end: " << (p == refContainer.end()) << std::endl;
//     std::cout << "if p == begin: " << (p == refContainer.begin()) <<
//     std::endl;
//     // showlist();
//     std::cout << "-------------- for loop end --------------" << std::endl;
//   }
//   std::cout << "collecting done" << std::endl;
//   memfreed = true;
//   return memfreed;
// }

template <class T, int size> bool Pointer<T, size>::collect() {

  bool memfreed = false;
  typename std::list<PtrDetails<T>>::iterator p;
  do {
    // Scan refContainer looking for unreferenced pointers.

    for (p = refContainer.begin(); p != refContainer.end(); p++) {
      // If in-use, skip.
      // std::cout << "-------------- for loop begin --------------" << std::endl;
      if (p->refcount > 0) {
        // std::cout << "p in use, skipping" << std::endl;
        continue;
      }
      // std::cout << "p not in use, if p->memPtr == nullptr: "
                // << (p->memPtr == nullptr) << std::endl;
      // std::cout << "refContainer.size: " << refContainer.size() << std::endl;
      memfreed = true;
      // Remove unused entry from refContainer.
      refContainer.remove(*p);
      // std::cout << "erasing done" << std::endl;
      // std::cout << "refContainer.size: " << refContainer.size() << std::endl;
      // std::cout << "if p == end: " << (p == refContainer.end()) << std::endl;
      // std::cout << "if p == begin: " << (p == refContainer.begin())
                // << std::endl;

      // Free memory if not null.
      if (p->memPtr) {
        // std::cout << "p->memPtr != nullptr "
                  // << "p->isArray: " << (p->isArray) << std::endl;
        std:: cout << "p node value: " << p->memPtr->value << std::endl;
        if (p->isArray) {
          // std::cout << "deleting array" << std::endl;
          delete[] p->memPtr; // delete array
        } else {
          // std::cout << "deleting number" << std::endl;
          delete p->memPtr; // delete single element
        }
        // std::cout << "p->memPtr deleted " << std::endl;
      }
      // std::cout << "-------------- for loop end --------------" << std::endl;
      // Restart the search.
      break;
    }
  } while (p != refContainer.end());
  // std::cout << "collecting done" << std::endl;
  return memfreed;
}

// Overload assignment of pointer to Pointer.
template <class T, int size> T *Pointer<T, size>::operator=(T *t) {

  // TODO: Implement operator==
  // LAB: Smart Pointer Project Lab
  typename std::list<PtrDetails<T>>::iterator p = findPtrInfo(addr);
  p->refcount--; // since previous exists refcount will always > 0
  p = findPtrInfo(t);
  if (p == refContainer.end())
    refContainer.push_back(PtrDetails<T>(t, size));
  else
    p->refcount++;
  addr = t;
  isArray = p->isArray;
  arraySize = p->arraySize;
  return t;
}
// Overload assignment of Pointer to Pointer.
template <class T, int size>
Pointer<T, size> &Pointer<T, size>::operator=(Pointer &rv) {

  // TODO: Implement operator==
  // LAB: Smart Pointer Project Lab
  typename std::list<PtrDetails<T>>::iterator p = findPtrInfo(addr);
  p->refcount--; // since previous exists refcount will always > 0
  p = findPtrInfo(rv.addr);
  if (p == refContainer.end())
    refContainer.push_back(PtrDetails<T>(rv.addr, rv.arraySize));
  else
    p->refcount++;
  addr = rv.addr;
  isArray = p->isArray;
  arraySize = p->arraySize;
  return rv;
}

// A utility function that displays refContainer.
template <class T, int size> void Pointer<T, size>::showlist() {
  typename std::list<PtrDetails<T>>::iterator p;
  std::cout << "refContainer<" << typeid(T).name() << ", " << size << ">:\n";
  std::cout << "memPtr refcount value\n ";
  if (refContainer.begin() == refContainer.end()) {
    std::cout << " Container is empty!\n\n ";
  }
  for (p = refContainer.begin(); p != refContainer.end(); p++) {
    std::cout << "[" << (void *)p->memPtr << "]"
              << " " << p->refcount << " ";
    if (p->memPtr)
      std::cout << " " << *p->memPtr;
    else
      std::cout << "---";
    std::cout << std::endl;
  }
  std::cout << std::endl;
}
// Find a pointer in refContainer.
template <class T, int size>
typename std::list<PtrDetails<T>>::iterator
Pointer<T, size>::findPtrInfo(T *ptr) {
  typename std::list<PtrDetails<T>>::iterator p;
  // Find ptr in refContainer.
  for (p = refContainer.begin(); p != refContainer.end(); p++)
    if (p->memPtr == ptr)
      return p;
  return p;
}
// Clear refContainer when program exits.
template <class T, int size> void Pointer<T, size>::shutdown() {
  if (refContainerSize() == 0)
    return; // list is empty
  typename std::list<PtrDetails<T>>::iterator p;
  for (p = refContainer.begin(); p != refContainer.end(); p++) {
    // Set all reference counts to zero
    p->refcount = 0;
  }
  collect();
}