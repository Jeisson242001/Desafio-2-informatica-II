#ifndef MENU_H
#define MENU_H

#include <string>
#include "DynArray.h"
#include "Song.h"
#include "User.h"
#include "AdManager.h"
#include "Player.h"

class Menu {
    DynArray<Song>   _songs;
    DynArray<Song*>  _songPtrs;
    DynArray<User>   _users;
    AdManager        _ads;
    Player           _player;
    User*            _currentUser;
    std::string      _dataDir;   // para guardar favorites/follows

public:
    Menu();

    void loadOrSeed(const std::string& dir);
    void run();

private:
    // helpers
    void buildSongPtrs();
    User* findUser(const std::string& nick);
    Song* findSongById(const std::string& id);

    // UI
    void login();
    void listSongs();

    void mainLoopStandard();
    void mainLoopPremium();

    // Favoritos (premium)
    void favoritesMenu(User* u);
    void addFavorite(User* u);
    void removeFavorite(User* u);
    void followPremium(User* u);
    void playFavoritesOrdered(User* u);
    void playFavoritesRandom(User* u);
    void unfollow(User* u);

};

#endif // MENU_H
