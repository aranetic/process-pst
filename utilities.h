#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>

extern std::wstring string_to_wstring(const std::string &str);
extern std::wstring rfc822_quote(const std::wstring &str);
extern std::wstring rfc822_email(const std::wstring &display_name,
                                 const std::wstring &email);

#endif // UTILITIES_H
