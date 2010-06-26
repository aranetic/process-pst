#include <pstsdk/pst.h>
#include "document.h"

using namespace std;
using boost::any;
using namespace pstsdk;

namespace {
    // Rather than spend quite so much time checking for exceptions, let's
    // build a simple wrapper function which checks for us.
    template <typename R, typename T>
    bool has_prop(const pstsdk::message &m, R (T::*pmf)() const) {
        try {
            (m.*pmf)();
            return true;
        } catch (key_not_found<prop_id> &) {
            return false;
        }
    }
}

document::document(const pstsdk::message &m) {
    if (has_prop(m, &message::get_subject))
        (*this)[L"#Subject"] = wstring(m.get_subject());
}

any &document::operator[](const wstring &key) {
    return m_tags[key];
}
