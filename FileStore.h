#ifndef FILESTORE_H
#define FILESTORE_H

#include <string>
#include "DynArray.h"
#include "Song.h"
#include "User.h"
#include "AdManager.h"

class FileStore {
public:
    // Carga
    static bool loadSongs(const std::string& path, DynArray<Song>& outSongs);
    static bool loadAds(const std::string& path, AdManager& outAds);
    static bool loadUsers(const std::string& path, DynArray<User>& outUsers);
    static bool loadFavorites(const std::string& path, DynArray<User>& users, DynArray<Song>& songs);
    static bool loadFollows(const std::string& path, DynArray<User>& users);

    // Utilidades
    static void buildSongPtrs(DynArray<Song>& songs, DynArray<Song*>& outPtrs);

    // Guardado (novedad)
    static bool saveFavorites(const std::string& path, const DynArray<User>& users);
    static bool saveFollows(const std::string& path, const DynArray<User>& users);
};

#endif // FILESTORE_H
