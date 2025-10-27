#ifndef RING_H
#define RING_H

#include <cstddef>
#include <new>

// Buffer circular de capacidad fija (sin STL). Guarda T por valor.
// Métodos: push, size, capacity, kth_last(k) -> 0 = último insertado, 1 = anterior, etc.
template <typename T>
class Ring {
private:
    T* _buf;
    std::size_t _cap;
    std::size_t _count;
    std::size_t _head; // próxima posición de escritura

public:
    explicit Ring(std::size_t cap = 8)
        : _buf(nullptr), _cap(0), _count(0), _head(0)
    {
        if (cap < 1) cap = 1;
        _buf = (T*)::operator new(sizeof(T) * cap);
        _cap = cap;
    }

    ~Ring() {
        // Destruir los _count elementos válidos
        for (std::size_t i = 0; i < _count; ++i) {
            std::size_t idx = ((_head + _cap) - i - 1) % _cap;
            _buf[idx].~T();
        }
        ::operator delete((void*)_buf);
    }

    void push(const T& v) {
        if (_count < _cap) {
            new (_buf + _head) T(v);
            _head = (_head + 1) % _cap;
            _count++;
        } else {
            // Sobrescribe el más antiguo
            _buf[_head].~T();
            new (_buf + _head) T(v);
            _head = (_head + 1) % _cap;
        }
    }

    std::size_t size() const     { return _count; }
    std::size_t capacity() const { return _cap; }

    // k=0 devuelve el último elementro insertado; k=1 el previo, etc.
    T* kth_last(std::size_t k) {
        if (k >= _count) return nullptr;
        std::size_t idx = ((_head + _cap) - k - 1) % _cap;
        return &_buf[idx];
    }
    const T* kth_last(std::size_t k) const {
        if (k >= _count) return nullptr;
        std::size_t idx = ((_head + _cap) - k - 1) % _cap;
        return &_buf[idx];
    }
};

#endif // RING_H
