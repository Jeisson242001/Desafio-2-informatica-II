#ifndef PLAYER_H
#define PLAYER_H

#include "DynArray.h"
#include "Ring.h"
#include "Song.h"
#include "FavoriteList.h"
#include "RandomLCG.h"

class AdManager;
class Advertisement;

class Player {
    DynArray<Song*> _all;
    int  _currentIndex;
    bool _repeat;
    RandomLCG _rng;
    Ring<int> _history;      // historial (M=6 por defecto) - lo mantenemos
    DynArray<int> _histList; // historial lineal navegable (índices de _all)
    int _histIndex;          // posición actual en _histList (-1 = vacío)
    unsigned _songsSinceAd;  // anuncio cada 2 canciones

public:
    explicit Player(unsigned historyCap = 6);
    void addSong(Song* s);
    unsigned count() const;
    Song* current();

    void setRepeat(bool r);
    bool repeat() const;

    // reproducción global
    Song* playRandom();
    Song* next();
    Song* prev();

    // favoritos
    Song* playFavoritesOrdered(FavoriteList& list, unsigned startIndex, bool* finishedOut);
    Song* playFavoritesRandom(FavoriteList& list);

    // anuncios (Standard)
    const Advertisement* maybeAd(AdManager* ads, bool isPremium);
};

#endif // PLAYER_H
