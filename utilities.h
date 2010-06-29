#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <vector>

extern std::wstring string_to_wstring(const std::string &str);
extern std::string wstring_to_string(const std::wstring &wstr);
extern std::string wstring_to_utf8(const std::wstring &wstr);
extern std::string md5(const std::vector<uint8_t> &v);
extern std::string xml_quote(const std::wstring &wstr);

#endif // UTILITIES_H
