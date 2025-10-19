#ifndef ADVERTISEMENT_H
#define ADVERTISEMENT_H
#include <string>

class Advertisement {
    std::string _id;
    char        _cat;   // 'C','B','A' (A = AAA)
    std::string _text;
public:
    Advertisement(): _id(""), _cat('C'), _text("") {}
    Advertisement(const std::string& id, char cat, const std::string& text)
        : _id(id), _cat(cat), _text(text) {}

    const std::string& id()   const { return _id; }
    char               category() const { return _cat; }
    const std::string& text()  const { return _text; }

    // peso para selección ponderada
    unsigned weight() const { return (_cat=='A')?3u:((_cat=='B')?2u:1u); }
};

#endif // ADVERTISEMENT_H
