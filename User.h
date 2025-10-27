#ifndef USER_H
#define USER_H
#include <string>
#include "FavoriteList.h"

enum class UserType : unsigned char { Standard = 0, Premium = 1 };

class User {
private:
    std::string _nick, _city, _country;
    UserType    _type;
    FavoriteList _favorites;  // solo se usa si es Premium
    User*        _follows;    // sigue a otro Premium
    std::string  _regDate;    // YYYY-MM-DD

public:
    User()
        : _nick(""), _city(""), _country(""), _type(UserType::Standard),
        _favorites(), _follows(nullptr), _regDate("1970-01-01") {}

    User(const std::string& n, const std::string& c, const std::string& co, UserType t)
        : _nick(n), _city(c), _country(co), _type(t), _favorites(), _follows(nullptr), _regDate("1970-01-01") {}

    User(const std::string& n, const std::string& c, const std::string& co, UserType t, const std::string& regDate)
        : _nick(n), _city(c), _country(co), _type(t), _favorites(), _follows(nullptr), _regDate(regDate) {}

    // getters
    const std::string& nick()    const { return _nick; }
    const std::string& city()    const { return _city; }
    const std::string& country() const { return _country; }
    const std::string& regDate() const { return _regDate; }
    UserType type() const { return _type; }
    bool isPremium() const { return _type == UserType::Premium; }

    // setters mínimos
    void setCity(const std::string& c)    { _city = c; }
    void setCountry(const std::string& c) { _country = c; }

    // favoritos (solo si Premium)
    FavoriteList* favorites() { return isPremium() ? &_favorites : nullptr; }
    const FavoriteList* favorites() const { return isPremium() ? &_favorites : nullptr; }

    // seguir a Premium (evita self-follow, repetidos y nulos)
    void follow(User* p) {
        if (!p) { _follows = nullptr; return; } // permitir "dejar de seguir" con nullptr
        if (p == this) return;                  // no puedes seguirte a ti mismo
        if (!p->isPremium()) return;            // solo seguir a Premium
        if (_follows == p) return;              // ya lo sigues: no hagas nada
        _follows = p;                           // reemplaza al anterior si existía
    }


    User* follows() const { return _follows; }
};

#endif // USER_H
