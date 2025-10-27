#ifndef STRINGUTIL_H
#define STRINGUTIL_H
#include <string>
#include "DynArray.h"

namespace StringUtil {
std::string trim(const std::string& s);
void split(const std::string& s, char delim, DynArray<std::string>& out);
}

#endif // STRINGUTIL_H
