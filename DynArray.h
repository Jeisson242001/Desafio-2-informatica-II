#ifndef DYNARRAY_H
#define DYNARRAY_H

#include <cstddef>  // size_t
#include <new>      // ::operator new / placement new

// Arreglo dinámico genérico SIN STL (solo std::string permitido en el proyecto).
// Métodos: reserve, push_back, removeAt, at, size, capacity, clear.
// Nota: at(idx) NO valida límites.

template <typename T>
class DynArray {
private:
    T* _data;
    std::size_t _size;
    std::size_t _capacity;

    void reallocate(std::size_t newCap) {
        T* newData = (T*)::operator new(sizeof(T) * newCap);
        for (std::size_t i = 0; i < _size; ++i) {
            new (newData + i) T(_data[i]); // copia
            _data[i].~T();
        }
        ::operator delete((void*)_data);
        _data = newData;
        _capacity = newCap;
    }

public:
    DynArray() : _data(nullptr), _size(0), _capacity(0) {}

    DynArray(const DynArray& other) : _data(nullptr), _size(0), _capacity(0) {
        if (other._size) {
            _data = (T*)::operator new(sizeof(T) * other._size);
            _capacity = _size = other._size;
            for (std::size_t i = 0; i < _size; ++i)
                new (_data + i) T(other._data[i]);
        }
    }

    DynArray& operator=(const DynArray& other) {
        if (this == &other) return *this;
        clear();
        ::operator delete((void*)_data);
        _data = nullptr; _size = 0; _capacity = 0;
        if (other._size) {
            _data = (T*)::operator new(sizeof(T) * other._size);
            _capacity = _size = other._size;
            for (std::size_t i = 0; i < _size; ++i)
                new (_data + i) T(other._data[i]);
        }
        return *this;
    }

    ~DynArray() {
        clear();
        ::operator delete((void*)_data);
    }

    void reserve(std::size_t newCap) {
        if (newCap > _capacity) reallocate(newCap);
    }

    void push_back(const T& value) {
        if (_size == _capacity)
            reallocate(_capacity ? _capacity * 2 : 4);
        new (_data + _size) T(value);
        _size++;
    }

    void removeAt(std::size_t idx) {
        if (idx >= _size) return;
        _data[idx].~T();
        for (std::size_t i = idx; i + 1 < _size; ++i) {
            new (_data + i) T(_data[i + 1]);
            _data[i + 1].~T();
        }
        _size--;
    }

    T& at(std::size_t idx) { return _data[idx]; }
    const T& at(std::size_t idx) const { return _data[idx]; }

    std::size_t size() const { return _size; }
    std::size_t capacity() const { return _capacity; }

    void clear() {
        for (std::size_t i = 0; i < _size; ++i) _data[i].~T();
        _size = 0;
    }
};

#endif // DYNARRAY_H
