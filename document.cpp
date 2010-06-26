#include <pstsdk/pst.h>
#include "document.h"

using namespace std;
using boost::any;
using namespace pstsdk;

document::document(const pstsdk::message &m) {
    try {
        (*this)[L"#Subject"] = wstring(m.get_subject());
    } catch (key_not_found<prop_id>&) {
    }
}

any &document::operator[](const wstring &key) {
    return m_tags[key];
}
