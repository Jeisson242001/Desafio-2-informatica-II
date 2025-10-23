#include "Menu.h"
#include "FileStore.h"
#include "Advertisement.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>

#include "MemoryTracker.h"
#include "Iteration.h"

static void sleep3s(){ std::this_thread::sleep_for(std::chrono::seconds(3)); }

Menu::Menu() : _songs(), _songPtrs(), _users(), _ads(), _player(6), _currentUser(nullptr), _dataDir("") {}

void Menu::loadOrSeed(const std::string& dir){
    _dataDir = dir;

    bool okSongs = FileStore::loadSongs(dir + "/songs.txt", _songs);
    bool okAds   = FileStore::loadAds(dir + "/ads.txt", _ads);
    bool okUsers = FileStore::loadUsers(dir + "/users.txt", _users);
    if(okUsers) {
        FileStore::loadFavorites(dir + "/favorites.txt", _users, _songs);
        FileStore::loadFollows(dir + "/follows.txt", _users);
    }

    if(!(okSongs && okAds && okUsers)){
        // Datos demo
        _songs.push_back( Song("100010101","A tu lado",210,"/users/storage/claudia_lorelle/audio/a_tu_lado",
                              "Claudia Lorelle","Lugar Secreto","/users/storage/claudia_lorelle/image/lugar_secreto.png") );
        _songs.push_back( Song("100040201","La Tierra del Olvido",250,"/users/storage/carlos_vives/audio/la_tierra_del_olvido",
                              "Carlos Vives","La Tierra del Olvido","/users/storage/carlos_vives/image/tierra_del_olvido.png") );
        _songs.push_back( Song("100030101","La Rebelion",240,"/users/storage/joe_arroyo/audio/la_rebelion",
                              "Joe Arroyo","Grandes Exitos","/users/storage/joe_arroyo/image/grandes_exitos.png") );
        _ads.add( Advertisement("ad01",'C',"Prueba Gratis 7 dias") );
        _ads.add( Advertisement("ad02",'B',"2 meses al 50%") );
        _ads.add( Advertisement("ad03",'A',"AAA—Auris con cancelacion") );
        _users.push_back( User("julian_std","Medellin","CO", UserType::Standard, "2025-09-01") );
        _users.push_back( User("julian_pro","Medellin","CO", UserType::Premium,  "2025-09-01") );
        if(_users.at(1).favorites()){
            _users.at(1).favorites()->add(&_songs.at(0));
            _users.at(1).favorites()->add(&_songs.at(2));
        }
        // guardamos demo para que quede persistente si no existían archivos
        FileStore::saveFavorites(_dataDir + "/favorites.txt", _users);
        FileStore::saveFollows(_dataDir + "/follows.txt", _users);

    }

    FileStore::buildSongPtrs(_songs,_songPtrs);
    for(unsigned i=0;i<_songPtrs.size();++i) _player.addSong(_songPtrs.at(i));
}

User* Menu::findUser(const std::string& nick){
    for(unsigned i=0;i<_users.size();++i) if(_users.at(i).nick()==nick) return &_users.at(i);
    return nullptr;
}
Song* Menu::findSongById(const std::string& id){
    for(unsigned i=0;i<_songPtrs.size();++i) if(_songPtrs.at(i)->id()==id) return _songPtrs.at(i);
    return nullptr;
}
void Menu::buildSongPtrs(){
    _songPtrs.clear(); _songPtrs.reserve(_songs.size());
    for(unsigned i=0;i<_songs.size();++i) _songPtrs.push_back(&_songs.at(i));
}

void Menu::login(){
    std::cout << "==== Login ====\nNick: ";
    std::string nick; std::getline(std::cin, nick);
    _currentUser = findUser(nick);
    if(!_currentUser) std::cout << "Usuario no encontrado.\n";
    else std::cout << "Bienvenido, " << _currentUser->nick()
                  << " (" << (_currentUser->isPremium()?"Premium":"Standard")
                  << ", " << _currentUser->regDate() << ")\n";
}

void Menu::listSongs(){
    std::cout << "== Canciones ==\n";
    for(unsigned i=0;i<_songs.size();++i){
        const Song& s = _songs.at(i);
        std::cout << " - " << s.id() << " | " << s.name() << " | " << s.duration()
                  << "s | " << s.artist() << " | " << s.album() << "\n";
    }
}

void Menu::mainLoopStandard(){
    auto printSong = [&](Song* s){
        if(!s) return;
        std::cout << "\nCantante: " << s->artist() << "\n"
                  << "Album: " << s->album() << "\n"
                  << "Ruta a la portada: " << s->cover() << "\n"
                  << "Titulo: " << s->name() << "\n"
                  << "Ruta al audio: " << s->path128() << "\n"
                  << "Duracion: " << s->duration() << " s\n";
    };

    while(true){
        std::cout << "\n[Standard] Opciones:\n 1) Aleatorio (K=5, 3s)\n 2) Listar\n 0) Salir\n> ";
        std::string op; std::getline(std::cin, op);
        if(op=="0") break;
        if(op=="2"){ listSongs(); continue; }
        if(op=="1"){
            ITER_RESET();
            const unsigned K=5;
            for(unsigned i=0;i<K;++i){
                Song* s = _player.playRandom();
                if(!s){ std::cout<<"No hay canciones.\n"; break; }
                printSong(s);
                if(const Advertisement* ad = _player.maybeAd(&_ads, /*isPremium=*/false))
                    std::cout << "\n...ANUNCIO EN MARCHA..." << '"' << ad->text() << "\" (Categoria " << ad->category() << ")\n";
                sleep3s();
            }
            std::cout << "[Recursos] iteraciones=" << ITER_VALUE()
                      << "  mem=" << MemTrack::current() << " bytes\n";
        }
    }
}

void Menu::favoritesMenu(User* u){
    if(!u->isPremium()){ std::cout<<"Solo Premium.\n"; return; }

    while(true){
        const User* f = u->follows();
        std::cout << "\nFavoritos (sigues a: " << (f ? f->nick() : "(nadie)") << ")\n"
                  << " 1) Agregar por id\n"
                  << " 2) Eliminar por indice\n"
                  << " 3) Seguir a Premium\n"
                  << " 4) Listar\n"
                  << " 5) Dejar de seguir\n"
                  << " 0) Volver\n> ";

        std::string op; std::getline(std::cin, op);
        if(op=="0") break;
        else if(op=="1") addFavorite(u);
        else if(op=="2") removeFavorite(u);
        else if(op=="3") followPremium(u);
        else if(op=="4"){
            const FavoriteList* fl = u->favorites();
            if(!fl || fl->size()==0){ std::cout<<"(vacio)\n"; continue; }
            for(unsigned i=0;i<fl->size();++i){
                const Song* s = fl->at(i);
                if(s) std::cout<<i<<") "<<s->id()<<" - "<<s->name()<<"\n";
            }
        }
        else if(op=="5") unfollow(u);       // <-- NUEVO
    }
}


void Menu::addFavorite(User* u){
    std::cout<<"Id canción: "; std::string id; std::getline(std::cin, id);
    Song* s = findSongById(id); if(!s){ std::cout<<"No existe.\n"; return; }
    if(FavoriteList* fl=u->favorites()){
        fl->add(s); std::cout<<"Agregada.\n";
        // persistir
        FileStore::saveFavorites(_dataDir + "/favorites.txt", _users);
    }
}

void Menu::removeFavorite(User* u){
    if(!u->isPremium()||!u->favorites()){ std::cout<<"No premium.\n"; return; }
    std::cout<<"Índice a eliminar: "; std::string idxs; std::getline(std::cin, idxs);
    unsigned idx=(unsigned)std::atoi(idxs.c_str());
    u->favorites()->removeIndex(idx);
    std::cout<<"Eliminado.\n";
    // persistir
    FileStore::saveFavorites(_dataDir + "/favorites.txt", _users);
}

void Menu::followPremium(User* u){
    std::cout<<"Nick de Premium a seguir: "; std::string nick; std::getline(std::cin, nick);

    // No permitir self-follow
    if (nick == u->nick()) { std::cout << "No puedes seguirte a ti mismo.\n"; return; }

    // Buscar y validar Premium destino
    User* p = findUser(nick);
    if(!p || !p->isPremium()){ std::cout<<"No es Premium o no existe.\n"; return; }

    // Si ya lo seguías, no cambies follows.txt (pero igual podemos importar sus favoritos)
    bool changedFollow = false;
    if (u->follows() == p) {
        std::cout << "Ya sigues a " << p->nick() << ".\n";
    } else {
        u->follow(p); // reemplaza al anterior si había
        std::cout<<"Ahora sigues a "<<p->nick()<<".\n";
        changedFollow = true;
    }

    // --- IMPORTAR FAVORITOS DEL SEGUIDO A LOS MÍOS (UNIÓN, SIN DUPLICADOS) ---
    if (u->favorites() && p->favorites()) {
        FavoriteList* dst = u->favorites();
        const FavoriteList* src = p->favorites();
        unsigned before = dst->size();
        for (unsigned i = 0; i < src->size(); ++i) {
            const Song* sconst = src->at(i);
            dst->add(const_cast<Song*>(sconst));   // ← quitamos el const solo para guardar el puntero
            if (dst->size() >= 10000) break;
        }

        unsigned added = dst->size() - before;
        std::cout << "Se agregaron " << added << " canciones a tus favoritos (union sin duplicados).\n";
        if (added > 0) {
            FileStore::saveFavorites(_dataDir + "/favorites.txt", _users); // PERSISTE favoritos
        }
    }

    // Persistir follows SOLO si cambió a quién sigues
    if (changedFollow) {
        FileStore::saveFollows(_dataDir + "/follows.txt", _users);          // PERSISTE follows
    }
}


void Menu::unfollow(User* u){
    if(!u->isPremium()){ std::cout<<"Solo Premium.\n"; return; }
    if(u->follows()==nullptr){
        std::cout<<"No sigues a nadie.\n";
        return;
    }
    u->follow(nullptr); // limpia el seguido
    FileStore::saveFollows(_dataDir + "/follows.txt", _users); // persiste
    std::cout<<"Ahora no sigues a nadie.\n";
}




void Menu::playFavoritesOrdered(User* u){
    if(!u->isPremium()||!u->favorites()){ std::cout<<"No premium.\n"; return; }
    auto printSong = [&](Song* s){
        if(!s) return;
        std::cout << "\nCantante: " << s->artist() << "\n"
                  << "Album: " << s->album() << "\n"
                  << "Ruta a la portada: " << s->cover() << "\n"
                  << "Titulo: " << s->name() << "\n"
                  << "Ruta al audio: " << s->path320() << "\n"
                  << "Duracion: " << s->duration() << " s\n";
    };

    FavoriteList merged = *u->favorites();
    if(User* f=u->follows()) if(f->favorites()) merged = merged + *(f->favorites());
    bool finished=false;
    ITER_RESET();
    for(unsigned i=0;i<merged.size();++i){
        Song* s = _player.playFavoritesOrdered(merged,i,&finished);
        if(!s) break;
        printSong(s);
        sleep3s();
    }
    if(finished) std::cout<<"(Fin de lista)\n";
    std::cout << "[Recursos] iteraciones=" << ITER_VALUE()
              << "  mem=" << MemTrack::current() << " bytes\n";
}

void Menu::playFavoritesRandom(User* u){
    if(!u->isPremium()||!u->favorites()){ std::cout<<"No premium.\n"; return; }
    auto printSong = [&](Song* s){
        if(!s) return;
        std::cout << "\nCantante: " << s->artist() << "\n"
                  << "Album: " << s->album() << "\n"
                  << "Ruta a la portada: " << s->cover() << "\n"
                  << "Titulo: " << s->name() << "\n"
                  << "Ruta al audio: " << s->path320() << "\n"
                  << "Duracion: " << s->duration() << " s\n";
    };

    FavoriteList merged = *u->favorites();
    if(User* f=u->follows()) if(f->favorites()) merged = merged + *(f->favorites());
    ITER_RESET();
    const unsigned K=5;
    for(unsigned i=0;i<K;++i){
        Song* s = _player.playFavoritesRandom(merged);
        if(!s){ std::cout<<"No hay en favoritos.\n"; break; }
        printSong(s);
        sleep3s();
    }
    std::cout << "[Recursos] iteraciones=" << ITER_VALUE()
              << "  mem=" << MemTrack::current() << " bytes\n";
}

void Menu::mainLoopPremium(){
    auto printSong = [&](Song* s){
        if(!s) return;
        std::cout << "\nCantante: " << s->artist() << "\n"
                  << "Album: " << s->album() << "\n"
                  << "Ruta a la portada: " << s->cover() << "\n"
                  << "Titulo: " << s->name() << "\n"
                  << "Ruta al audio: " << s->path320() << "\n"
                  << "Duracion: " << s->duration() << " s\n";
    };

    while(true){
        std::cout << "\n[Premium] Opciones:\n"
                  << " 1) Aleatorio (K=5, 3s)\n"
                  << " 2) Next\n 3) Prev\n 4) Toggle Repeat\n"
                  << " 5) Favoritos (gestionar)\n"
                  << " 6) Favoritos en orden\n"
                  << " 7) Favoritos aleatorio\n"
                  << " 8) Listar\n 0) Salir\n> ";
        std::string op; std::getline(std::cin, op);
        if(op=="0") break;
        if(op=="8"){ listSongs(); continue; }
        if(op=="2"){ if(Song* s=_player.next()) printSong(s); continue; }
        if(op=="3"){ if(Song* s=_player.prev()) printSong(s); continue; }
        if(op=="4"){ _player.setRepeat(!_player.repeat()); std::cout<<"Repeat: "<<(_player.repeat()?"ON":"OFF")<<"\n"; continue; }
        if(op=="5"){ favoritesMenu(_currentUser); continue; }
        if(op=="6"){ playFavoritesOrdered(_currentUser); continue; }
        if(op=="7"){ playFavoritesRandom(_currentUser); continue; }
        if(op=="1"){
            ITER_RESET();
            const unsigned K=5;
            for(unsigned i=0;i<K;++i){
                Song* s = _player.playRandom();
                if(!s){ std::cout<<"No hay canciones.\n"; break; }
                printSong(s);
                sleep3s();
            }
            std::cout << "[Recursos] iteraciones=" << ITER_VALUE()
                      << "  mem=" << MemTrack::current() << " bytes\n";
        }
    }
}

void Menu::run(){
    login();
    if(!_currentUser) return;
    if(_currentUser->isPremium()) mainLoopPremium(); else mainLoopStandard();
}
