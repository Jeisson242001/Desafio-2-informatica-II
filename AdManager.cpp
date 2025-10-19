#include "AdManager.h"

const Advertisement* AdManager::pickWeighted() {
    if (_ads.size() == 0) return nullptr;

    unsigned totalW = 0;
    for (unsigned i = 0; i < _ads.size(); ++i) totalW += _ads.at(i).weight();
    if (totalW == 0) return nullptr;

    // Dos intentos: el 1º evitando repetir, el 2º relajando la restricción
    for (int attempt = 0; attempt < 2; ++attempt) {
        unsigned r = _rng.uniform(totalW), acc = 0;
        for (unsigned i = 0; i < _ads.size(); ++i) {
            acc += _ads.at(i).weight();
            if (r < acc) {
                const Advertisement* pick = &_ads.at(i);
                if (attempt == 0 && pick->id() == _lastId) break; // reintentar
                _lastId = pick->id();
                return pick;
            }
        }
    }
    _lastId = _ads.at(0).id();
    return &_ads.at(0);
}
