#include "Player.h"
#include "AdManager.h"
#include <cstdlib> // srand, rand
#include <ctime>

Player::Player(unsigned historyCap)
    : _all(), _currentIndex(-1), _repeat(false), _rng(12345),
    _history(historyCap), _histList(), _histIndex(-1), _songsSinceAd(0)
{
    // reservar capacidad del historial lineal en caso de querer optimizar
    _histList.reserve(historyCap);
    // seed rand si aún no lo hiciste en otro sitio
    std::srand((unsigned)std::time(nullptr));
}

void Player::addSong(Song* s) { if(s) _all.push_back(s); }
unsigned Player::count() const { return (unsigned)_all.size(); }
Song* Player::current() { return (_currentIndex >= 0) ? _all.at((std::size_t)_currentIndex) : nullptr; }
void Player::setRepeat(bool r) { _repeat = r; }
bool Player::repeat() const { return _repeat; }

// helper interno: truncar la parte "adelante" del historial si estamos navegando hacia atrás
static void truncateAfter(DynArray<int>& arr, int idx) {
    if (idx < 0) { arr.clear(); return; }

    // eliminar elementos con índice > idx, usando removeAt(size-1)
    while ((int)arr.size() > idx + 1) {
        arr.removeAt(arr.size() - 1);
    }
}

Song* Player::playRandom() {
    if (count() == 0) return nullptr;
    ITER_STEP(1);

    if (_repeat && _currentIndex >= 0) {
        // repetir la actual: añadimos entrada al Ring (compatibilidad) y también al historial navegable
        _history.push(_currentIndex);
        if (_histIndex >= 0 && _histIndex + 1 < (int)_histList.size()) {
            truncateAfter(_histList, _histIndex);
        }
        _histList.push_back(_currentIndex);
        _histIndex = (int)_histList.size() - 1;

        Song* s = _all.at((std::size_t)_currentIndex);
        if (s) s->incPlays();
        return s;
    }

    unsigned idx = _rng.uniform(count());
    _currentIndex = (int)idx;

    // mantener Ring
    _history.push(_currentIndex);

    // truncar avance si estábamos navegando hacia atrás
    if (_histIndex >= 0 && _histIndex + 1 < (int)_histList.size()) {
        truncateAfter(_histList, _histIndex);
    }
    _histList.push_back(_currentIndex);
    _histIndex = (int)_histList.size() - 1;

    Song* s = _all.at((std::size_t)idx);
    if (s) s->incPlays();
    return s;
}

Song* Player::next() {
    if (count() == 0) return nullptr;
    ITER_STEP(1);

    // Si tenemos historial navegable y existe un "siguiente", avanzamos en el historial
    if (_histIndex >= 0 && _histIndex + 1 < (int)_histList.size()) {
        ++_histIndex;
        _currentIndex = _histList.at((std::size_t)_histIndex);
        _history.push(_currentIndex);
        Song* s = _all.at((std::size_t)_currentIndex);
        if (s) s->incPlays();
        return s;
    }

    // Si no hay historial navegable o no hay "siguiente", comportarse como antes:
    if (_currentIndex < 0) _currentIndex = 0;
    else if (!_repeat) _currentIndex = (_currentIndex + 1) % (int)count();

    _history.push(_currentIndex);

    // cuando generamos nuevo "siguiente" real, truncamos la parte adelante (ya no aplica) y anexamos
    if (_histIndex >= 0 && _histIndex + 1 < (int)_histList.size()) {
        truncateAfter(_histList, _histIndex);
    }
    _histList.push_back(_currentIndex);
    _histIndex = (int)_histList.size() - 1;

    Song* s = _all.at((std::size_t)_currentIndex);
    if (s) s->incPlays();
    return s;
}

Song* Player::prev() {
    // Si repeat está activo, mantener comportamiento previo: devuelve la actual
    if (_repeat) {
        if (_currentIndex < 0) return nullptr;
        _history.push(_currentIndex);
        if (_histIndex >= 0 && _histIndex + 1 < (int)_histList.size()) truncateAfter(_histList, _histIndex);
        _histList.push_back(_currentIndex);
        _histIndex = (int)_histList.size() - 1;

        Song* s = _all.at((std::size_t)_currentIndex);
        if (s) s->incPlays();
        return s;
    }

    // Si no tenemos historial navegable, devolvemos la actual (como antes)
    if (_histList.size() == 0) return current();

    // Si estamos en el inicio, devolvemos la primera (no bajamos a -1)
    if (_histIndex <= 0) {
        _histIndex = 0;
        _currentIndex = _histList.at(0);
        Song* s = _all.at((std::size_t)_currentIndex);
        if (s) s->incPlays();
        return s;
    }

    // retroceder en historial
    ITER_STEP(1);
    --_histIndex;
    _currentIndex = _histList.at((std::size_t)_histIndex);
    Song* s = _all.at((std::size_t)_currentIndex);
    if (s) s->incPlays();
    return s;
}

Song* Player::playFavoritesOrdered(FavoriteList& list, unsigned startIndex, bool* finishedOut) {
    if (finishedOut) *finishedOut = false;
    if (list.size() == 0) return nullptr;
    if (startIndex >= list.size()) { if (finishedOut) *finishedOut = true; return nullptr; }
    _currentIndex = -1; // no aplica índice global
    ITER_STEP(1);
    Song* s = list.at(startIndex);
    // mantener compatibilidad con Ring historial original
    _history.push((int)startIndex);
    if (s) s->incPlays();
    return s;
}

Song* Player::playFavoritesRandom(FavoriteList& list) {
    if (list.size() == 0) return nullptr;
    unsigned idx = _rng.uniform(list.size());
    ITER_STEP(1);
    // no añadir al historial navegable (índices de favoritos no son índices del catálogo)
    _history.push((int)idx);
    Song* s = list.at(idx);
    if (s) s->incPlays();
    return s;
}

const Advertisement* Player::maybeAd(AdManager* ads, bool isPremium) {
    if (!ads || isPremium) return nullptr;
    _songsSinceAd++;
    if (_songsSinceAd >= 2) {
        _songsSinceAd = 0;
        return ads->pickWeighted();
    }
    return nullptr;
}
