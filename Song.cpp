#include "Song.h"

Song::Song()
    : _id9("000000000"), _name(""), _durationSec(0), _plays(0),
    _basePath(""), _artist(""), _album(""), _coverPath("") {}

Song::Song(const std::string& id9, const std::string& name, unsigned durationSec, const std::string& basePath)
    : _id9(id9), _name(name), _durationSec(durationSec), _plays(0),
    _basePath(basePath), _artist(""), _album(""), _coverPath(basePath + ".png") {}

Song::Song(const std::string& id9, const std::string& name, unsigned durationSec,
           const std::string& basePath, const std::string& artist,
           const std::string& album, const std::string& coverPath)
    : _id9(id9), _name(name), _durationSec(durationSec), _plays(0),
    _basePath(basePath), _artist(artist), _album(album), _coverPath(coverPath) {}

Song::Song(const Song& other)
    : _id9(other._id9), _name(other._name), _durationSec(other._durationSec), _plays(other._plays),
    _basePath(other._basePath), _artist(other._artist), _album(other._album), _coverPath(other._coverPath) {}

Song& Song::operator=(const Song& other) {
    if (this == &other) return *this;
    _id9=other._id9; _name=other._name; _durationSec=other._durationSec; _plays=other._plays;
    _basePath=other._basePath; _artist=other._artist; _album=other._album; _coverPath=other._coverPath;
    return *this;
}

Song::~Song(){}

const std::string& Song::id()        const { return _id9; }
const std::string& Song::name()      const { return _name; }
unsigned           Song::duration()  const { return _durationSec; }
unsigned           Song::plays()     const { return _plays; }
const std::string& Song::basePath()  const { return _basePath; }
const std::string& Song::artist()    const { return _artist; }
const std::string& Song::album()     const { return _album; }
const std::string& Song::cover()     const { return _coverPath; }

void Song::setName(const std::string& n)      { _name = n; }
void Song::setDuration(unsigned s)            { _durationSec = s; }
void Song::setBasePath(const std::string& bp) { _basePath = bp; }
void Song::setArtist(const std::string& a)    { _artist = a; }
void Song::setAlbum(const std::string& a)     { _album = a; }
void Song::setCover(const std::string& p)     { _coverPath = p; }
void Song::incPlays()                         { _plays++; }

std::string Song::path128() const { return _basePath + "_128.ogg"; }
std::string Song::path320() const { return _basePath + "_320.ogg"; }

std::string Song::artistId() const { return (_id9.size() >= 5) ? _id9.substr(0,5) : ""; }
std::string Song::albumId()  const { return (_id9.size() >= 7) ? _id9.substr(5,2) : ""; }
std::string Song::trackId()  const { return (_id9.size() >= 9) ? _id9.substr(7,2) : ""; }

bool  Song::operator==(const Song& o) const { return _id9 == o._id9; }
