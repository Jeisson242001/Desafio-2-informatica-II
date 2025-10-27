#include "StringUtil.h"
#include <cctype>

std::string StringUtil::trim(const std::string& s){
    if(s.empty()) return s;
    std::size_t i=0, j=s.size();
    while(i<j && std::isspace((unsigned char)s[i])) ++i;
    while(j>i && std::isspace((unsigned char)s[j-1])) --j;
    return s.substr(i, j-i);
}

void StringUtil::split(const std::string& s, char delim, DynArray<std::string>& out){
    out.clear();
    std::size_t start = 0;
    for(std::size_t i=0;i<s.size();++i){
        if(s[i]==delim){
            out.push_back(s.substr(start, i-start));
            start = i+1;
        }
    }
    // último segmento
    if(start <= s.size()) out.push_back(s.substr(start));
}
