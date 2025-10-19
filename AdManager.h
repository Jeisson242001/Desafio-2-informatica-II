#ifndef ADMANAGER_H
#define ADMANAGER_H

#include "DynArray.h"
#include "Advertisement.h"
#include "RandomLCG.h"
#include <string>

class AdManager {
    DynArray<Advertisement> _ads;
    RandomLCG _rng;
    std::string _lastId;   // para no repetir anuncio consecutivo
public:
    AdManager(): _ads(), _rng(987654321ULL), _lastId("") {}

    void add(const Advertisement& a) { _ads.push_back(a); }
    unsigned count() const { return (unsigned)_ads.size(); }

    // Selección ponderada por categoría (C=1, B=2, A=3), evitando repetir 2 veces seguidas
    const Advertisement* pickWeighted();
};

#endif // ADMANAGER_H
