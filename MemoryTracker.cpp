#include "MemoryTracker.h"
#include <new>
#include <cstdlib>

namespace { static volatile std::size_t g_alloc = 0, g_free = 0; }
void MemTrack::add(std::size_t b){ g_alloc += b; }
void MemTrack::sub(std::size_t b){ g_free  += b; }
std::size_t MemTrack::totalAllocated(){ return g_alloc; }
std::size_t MemTrack::totalFreed()    { return g_free; }
std::size_t MemTrack::current()       { return (g_alloc>=g_free)?(g_alloc-g_free):0; }

// Sobrecargas globales (deben existir UNA sola vez en el proyecto)
void* operator new(std::size_t size) {
    std::size_t total = size + sizeof(std::size_t);
    void* raw = std::malloc(total);
    if (!raw) throw std::bad_alloc();
    *(std::size_t*)raw = size;
    MemTrack::add(size);
    return (char*)raw + sizeof(std::size_t);
}
void operator delete(void* ptr) noexcept {
    if (!ptr) return;
    void* raw = (char*)ptr - sizeof(std::size_t);
    std::size_t sz = *(std::size_t*)raw;
    MemTrack::sub(sz);
    std::free(raw);
}
void* operator new[](std::size_t size) {
    std::size_t total = size + sizeof(std::size_t);
    void* raw = std::malloc(total);
    if (!raw) throw std::bad_alloc();
    *(std::size_t*)raw = size;
    MemTrack::add(size);
    return (char*)raw + sizeof(std::size_t);
}
void operator delete[](void* ptr) noexcept {
    if (!ptr) return;
    void* raw = (char*)ptr - sizeof(std::size_t);
    std::size_t sz = *(std::size_t*)raw;
    MemTrack::sub(sz);
    std::free(raw);
}
