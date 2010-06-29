#ifndef RFC822_H
#define RFC822_H

#include <string>

extern std::string base64(const std::string &input);
extern bool contains_special_characters(const std::string &str);

#endif // RFC822_H

