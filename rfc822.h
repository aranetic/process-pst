#ifndef RFC822_H
#define RFC822_H

#include <string>

namespace boost { namespace posix_time { class ptime; } }
class document;

extern std::wstring rfc822_quote(const std::wstring &str);
extern std::wstring rfc822_email(const std::wstring &display_name,
                                 const std::wstring &email);

extern std::string base64(const std::string &input);
extern std::string base64_wrapped(const std::string &input);
extern bool contains_special_characters(const std::string &str);
extern std::string header_encode(const std::wstring &str);
extern std::string header_encode_email(const std::wstring &email);
extern std::string header(const std::string &name, const std::wstring &value);
extern std::string header(const std::string &name,
                          const std::vector<std::wstring> &emails);
extern std::string header(const std::string &name,
                          const boost::posix_time::ptime &time);

extern void document_to_rfc822(std::ostream &out, const document &d);

#endif // RFC822_H

