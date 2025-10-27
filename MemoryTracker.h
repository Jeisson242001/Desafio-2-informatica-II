#ifndef MEMORYTRACKER_H
#define MEMORYTRACKER_H
#include <cstddef>
namespace MemTrack {
void add(std::size_t bytes);
void sub(std::size_t bytes);
std::size_t totalAllocated();
std::size_t totalFreed();
std::size_t current();
}

#endif // MEMORYTRACKER_H
