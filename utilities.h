#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>

extern std::wstring string_to_wstring(const std::string &str);
extern std::wstring rfc822_quote(const std::wstring &str);

#endif // UTILITIES_H
