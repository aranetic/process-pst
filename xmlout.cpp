#include "utilities.h"
#include "xmlout.h"

using namespace std;

/// Convert a string to UTF-8 and escape any XML metacharacters.
string xml_quote(const wstring &wstr) {
    string utf8(wstring_to_utf8(wstr));
    string out;
    for (string::const_iterator i = utf8.begin(); i != utf8.end(); ++i) {
        char c(*i);
        switch (c) {
            case '<':  out += "&lt;"; break;
            case '>':  out += "&gt;"; break;
            case '\'': out += "&apos;"; break;
            case '"':  out += "&quot;"; break;
            case '&':  out += "&amp;"; break;
            default:   out += c;
        }
    }
    return out;
}
