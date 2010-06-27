#ifndef EDRM_H
#define EDRM_H

#include <string>

namespace boost { class any; }

extern std::wstring edrm_tag_data_type(const boost::any &value);
extern std::wstring edrm_tag_value(const boost::any &value);

#endif // EDRM_H
