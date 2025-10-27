#include "FileStore.h"
#include "StringUtil.h"
#include <fstream>
#include <cstdlib>

// helpers de búsqueda
static User* findUser(DynArray<User>& users, const std::string& nick){
    for(unsigned i=0;i<users.size();++i) if(users.at(i).nick()==nick) return &users.at(i);
    return nullptr;
}
static Song* findSong(DynArray<Song>& songs, const std::string& id){
    for(unsigned i=0;i<songs.size();++i) if(songs.at(i).id()==id) return &songs.at(i);
    return nullptr;
}

bool FileStore::loadSongs(const std::string& path, DynArray<Song>& outSongs){
    std::ifstream in(path.c_str());
    if(!in.is_open()) return false;
    std::string line;
    DynArray<std::string> parts;
    while(std::getline(in,line)){
        ITER_STEP(1);
        line = StringUtil::trim(line);
        if(line.empty()||line[0]=='#') continue;
        StringUtil::split(line,'|',parts);
        if(parts.size()<4) continue;
        const std::string id = StringUtil::trim(parts.at(0));
        const std::string name = StringUtil::trim(parts.at(1));
        unsigned dur = (unsigned)std::atoi(StringUtil::trim(parts.at(2)).c_str());
        const std::string base = StringUtil::trim(parts.at(3));
        const std::string artist = (parts.size()>=5)? StringUtil::trim(parts.at(4)) : "Unknown Artist";
        const std::string album  = (parts.size()>=6)? StringUtil::trim(parts.at(5)) : "Unknown Album";
        const std::string cover  = (parts.size()>=7)? StringUtil::trim(parts.at(6)) : (base + ".png");
        outSongs.push_back(Song(id,name,dur,base,artist,album,cover));
    }
    return true;
}

bool FileStore::loadAds(const std::string& path, AdManager& outAds){
    std::ifstream in(path.c_str());
    if(!in.is_open()) return false;
    std::string line; DynArray<std::string> parts;
    while(std::getline(in,line)){
        ITER_STEP(1);
        line = StringUtil::trim(line);
        if(line.empty()||line[0]=='#') continue;
        StringUtil::split(line,'|',parts);
        if(parts.size()<3) continue;
        std::string id = StringUtil::trim(parts.at(0));
        std::string catS = StringUtil::trim(parts.at(1));
        char cat = catS.empty()? 'C' : catS[0];
        std::string text = StringUtil::trim(parts.at(2));
        outAds.add( Advertisement(id,cat,text) );
    }
    return true;
}

bool FileStore::loadUsers(const std::string& path, DynArray<User>& outUsers){
    std::ifstream in(path.c_str());
    if(!in.is_open()) return false;
    std::string line; DynArray<std::string> parts;
    while(std::getline(in,line)){
        ITER_STEP(1);
        line = StringUtil::trim(line);
        if(line.empty()||line[0]=='#') continue;
        StringUtil::split(line,'|',parts);
        if(parts.size()<4) continue;
        std::string nick = StringUtil::trim(parts.at(0));
        unsigned type = (unsigned)std::atoi(StringUtil::trim(parts.at(1)).c_str()); // 0 std, 1 prem
        std::string city = StringUtil::trim(parts.at(2));
        std::string country = StringUtil::trim(parts.at(3));
        std::string reg = (parts.size()>=5)? StringUtil::trim(parts.at(4)) : "1970-01-01";
        outUsers.push_back( User(nick,city,country, type==1 ? UserType::Premium : UserType::Standard, reg) );
    }
    return true;
}

bool FileStore::loadFavorites(const std::string& path, DynArray<User>& users, DynArray<Song>& songs){
    std::ifstream in(path.c_str());
    if(!in.is_open()) return false;
    std::string line; DynArray<std::string> parts; DynArray<std::string> ids;
    while(std::getline(in,line)){
        ITER_STEP(1);
        line = StringUtil::trim(line);
        if(line.empty()||line[0]=='#') continue;
        StringUtil::split(line,'|',parts);
        if(parts.size()<2) continue;
        std::string nick = StringUtil::trim(parts.at(0));
        User* u = findUser(users,nick);
        if(!u || !u->isPremium()) continue;
        ids.clear();
        StringUtil::split(StringUtil::trim(parts.at(1)), ',', ids);
        for(unsigned i=0;i<ids.size();++i){
            std::string id = StringUtil::trim(ids.at(i));
            if(Song* s = findSong(songs,id)) u->favorites()->add(s);
        }
    }
    return true;
}

bool FileStore::loadFollows(const std::string& path, DynArray<User>& users){
    std::ifstream in(path.c_str());
    if(!in.is_open()) return false;
    std::string line; DynArray<std::string> parts;
    while(std::getline(in,line)){
        ITER_STEP(1);
        line = StringUtil::trim(line);
        if(line.empty()||line[0]=='#') continue;
        StringUtil::split(line,'|',parts);
        if(parts.size()<2) continue;
        std::string follower = StringUtil::trim(parts.at(0));
        std::string followed = StringUtil::trim(parts.at(1));
        if (follower == followed) continue;            // <<< ignora self-follow del archivo
        User* uf = findUser(users,follower);
        User* up = findUser(users,followed);
        if(uf && up && up->isPremium() && uf != up)    // <<< no self y solo premium
            uf->follow(up);
    }
    return true;
}


void FileStore::buildSongPtrs(DynArray<Song>& songs, DynArray<Song*>& outPtrs){
    outPtrs.clear();
    outPtrs.reserve(songs.size());
    for(unsigned i=0;i<songs.size();++i) outPtrs.push_back(&songs.at(i));
}

// ====== Guardado ======
static void writeLine(std::ofstream& out, const std::string& s){
    out.write(s.c_str(), (std::streamsize)s.size());
    out.put('\n');
}

bool FileStore::saveFavorites(const std::string& path, const DynArray<User>& users){
    std::ofstream out(path.c_str(), std::ios::trunc);
    if(!out.is_open()) return false;
    writeLine(out, "# nick|id,id,id,...");
    for(unsigned i=0;i<users.size();++i){
        ITER_STEP(1);
        const User& u = users.at(i);
        if(!u.isPremium()) continue;
        const FavoriteList* fl = u.favorites();
        if(!fl || fl->size()==0) continue;
        std::string line = u.nick(); line += '|';
        for(unsigned j=0;j<fl->size();++j){
            const Song* s = fl->at(j);
            if(!s) continue;
            if(j>0) line += ',';
            line += s->id();
        }
        writeLine(out, line);
    }
    return true;
}

bool FileStore::saveFollows(const std::string& path, const DynArray<User>& users){
    std::ofstream out(path.c_str(), std::ios::trunc);
    if(!out.is_open()) return false;
    writeLine(out, "# follower|followed");
    for(unsigned i=0;i<users.size();++i){
        ITER_STEP(1);
        const User& u = users.at(i);
        const User* f = u.follows();
        if(!f) continue;
        std::string line = u.nick(); line += '|'; line += f->nick();
        writeLine(out, line);
    }
    return true;
}
