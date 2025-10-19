#include "Player.h"
#include "AdManager.h"

Player::Player(unsigned historyCap)
    : _all(), _currentIndex(-1), _repeat(false), _rng(12345), _history(historyCap), _songsSinceAd(0) {}

void Player::addSong(Song* s) { _all.push_back(s); }
unsigned Player::count() const { return (unsigned)_all.size(); }
Song* Player::current() { return (_currentIndex >= 0) ? _all.at((unsigned)_currentIndex) : nullptr; }
void Player::setRepeat(bool r) { _repeat = r; }
bool Player::repeat() const { return _repeat; }

Song* Player::playRandom() {
    if (count() == 0) return nullptr;
    if (_repeat && _currentIndex >= 0) {
        _history.push(_currentIndex);
        Song* s = _all.at((unsigned)_currentIndex);
        if (s) s->incPlays();
        return s;
    }
    unsigned idx = _rng.uniform(count());
    _currentIndex = (int)idx;
    _history.push(_currentIndex);
    Song* s = _all.at(idx);
    if (s) s->incPlays();
    return s;
}

Song* Player::next() {
    if (count() == 0) return nullptr;
    if (_currentIndex < 0) _currentIndex = 0;
    else if (!_repeat) _currentIndex = (_currentIndex + 1) % (int)count();
    _history.push(_currentIndex);
    Song* s = _all.at((unsigned)_currentIndex);
    if (s) s->incPlays();
    return s;
}

Song* Player::prev() {
    if (_repeat) {
        if (_currentIndex < 0) return nullptr;
        _history.push(_currentIndex);
        Song* s = _all.at((unsigned)_currentIndex);
        if (s) s->incPlays();
        return s;
    }
    int* last1 = _history.kth_last(1);
    if (!last1) return current();
    _currentIndex = *last1;
    Song* s = _all.at((unsigned)_currentIndex);
    if (s) s->incPlays();
    return s;
}

Song* Player::playFavoritesOrdered(FavoriteList& list, unsigned startIndex, bool* finishedOut) {
    if (finishedOut) *finishedOut = false;
    if (list.size() == 0) return nullptr;
    if (startIndex >= list.size()) { if (finishedOut) *finishedOut = true; return nullptr; }
    _currentIndex = -1; // no aplica índice global
    Song* s = list.at(startIndex);
    _history.push((int)startIndex);
    if (s) s->incPlays();
    return s;
}

Song* Player::playFavoritesRandom(FavoriteList& list) {
    if (list.size() == 0) return nullptr;
    unsigned idx = _rng.uniform(list.size());
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
