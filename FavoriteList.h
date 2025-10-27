#ifndef FAVORITELIST_H
#define FAVORITELIST_H

#include "DynArray.h"
#include "Song.h"

class Player; // friend

class FavoriteList {
    DynArray<Song*> _items;
public:
    FavoriteList() : _items() {}
    FavoriteList(const FavoriteList& o) : _items(o._items) {}
    FavoriteList& operator=(const FavoriteList& o) { _items = o._items; return *this; }
    ~FavoriteList() {}

    void add(Song* s) {
        ITER_STEP(1);
        if (!s) return;
        if (_items.size() >= 10000) return;          // límite
        for (unsigned i=0;i<_items.size();++i)       // sin duplicados (por id)
            if (*_items.at(i) == *s) return;
        _items.push_back(s);
    }
    void removeIndex(unsigned idx)     { _items.removeAt(idx); }
    unsigned size()              const { return (unsigned)_items.size(); }
    Song* at(unsigned idx)             { return _items.at(idx); }
    const Song* at(unsigned idx) const { return _items.at(idx); }

    // Une sin duplicados
    FavoriteList operator+(const FavoriteList& b) const {
        FavoriteList r(*this);
        for (unsigned i = 0; i < b.size(); ++i) {
            Song* s = b._items.at(i);
            bool found = false;
            for (unsigned j = 0; j < r.size(); ++j) {
                ITER_STEP(1);
                if (*r._items.at(j) == *s) { found = true; break; }
            }
            if (!found) r._items.push_back(s);
        }
        return r;
    }

    friend class Player;
};

#endif // FAVORITELIST_H
