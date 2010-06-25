#include "document.h"

using namespace std;
using boost::any;

any &document::operator[](const wstring &key) {
    return m_tags[key];
}
