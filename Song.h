#ifndef SONG_H
#define SONG_H

#include <string>

class Song {
private:
    std::string _id9;        // 9 dígitos (AAAAA|BB|CC)
    std::string _name;
    unsigned    _durationSec;
    unsigned    _plays;

    std::string _basePath;   // sin sufijo: .../audio/<nombre>
    std::string _artist;     // cantante
    std::string _album;      // álbum
    std::string _coverPath;  // ruta portada (png)

public:
    Song();
    Song(const std::string& id9, const std::string& name, unsigned durationSec, const std::string& basePath);
    Song(const std::string& id9, const std::string& name, unsigned durationSec,
         const std::string& basePath, const std::string& artist,
         const std::string& album, const std::string& coverPath);
    Song(const Song& other);
    Song& operator=(const Song& other);
    ~Song();

    // getters
    const std::string& id()        const;
    const std::string& name()      const;
    unsigned           duration()  const;
    unsigned           plays()     const;
    const std::string& basePath()  const;
    const std::string& artist()    const;
    const std::string& album()     const;
    const std::string& cover()     const;

    // setters / acciones
    void setName(const std::string& n);
    void setDuration(unsigned s);
    void setBasePath(const std::string& bp);
    void setArtist(const std::string& a);
    void setAlbum(const std::string& a);
    void setCover(const std::string& p);
    void incPlays();

    // rutas de audio (según tipo de usuario)
    std::string path128() const;
    std::string path320() const;

    // utilidades ID 9 dígitos
    std::string artistId() const;  // primeros 5
    std::string albumId()  const;  // sig. 2
    std::string trackId()  const;  // últimos 2

    // igualdad por id
    bool operator==(const Song& o) const;
};

#endif // SONG_H
