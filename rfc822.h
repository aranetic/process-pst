#ifndef RFC822_H
#define RFC822_H

#include <string>

extern std::wstring rfc822_quote(const std::wstring &str);
extern std::wstring rfc822_email(const std::wstring &display_name,
                                 const std::wstring &email);

extern std::string base64(const std::string &input);
extern bool contains_special_characters(const std::string &str);
extern std::string header_encode(const std::string &utf8_str);

#endif // RFC822_H

