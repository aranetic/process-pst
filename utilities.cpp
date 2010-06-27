#include <cstdlib>
#include <cctype>
#include <stdexcept>
#include <vector>

#include "utilities.h"

using namespace std;

wstring string_to_wstring(const string &str) {
    size_t output_length = ::mbstowcs(NULL, str.c_str(), 0);
    if (output_length == size_t(-1))
        throw runtime_error("Cannot convert malformed string to wstring");

    vector<wchar_t> wvec(output_length);
    output_length = ::mbstowcs(&wvec[0], str.c_str(), wvec.size());
    if (output_length == size_t(-1))
        throw runtime_error("Cannot convert malformed string to wstring");

    wstring wstr(wvec.begin(), wvec.end());
    return wstr;
}

wstring rfc822_quote(const wstring &str) {
    // We quote everything except RFC 822 "atom" characters and spaces.
    bool needs_quoting = false;
    for (size_t i = 0; i < str.size(); ++i) {
        // Switch statements are reasonably fast ways to perform efficient
        // table lookups in most compilers.
        switch (str[i]) {
            case L'(': case L')': case L'<': case L'>':
            case L'@': case L',': case L';': case L':':
            case L'"': case L'\\': case L'.': case L'[':
            case L']':
                needs_quoting = true;
                break;
        }

        if (str[i] > 0x7f) {
            needs_quoting = true;
        } else {
            char c = static_cast<char>(str[i]);
            if (c != ' ' && !isgraph(c))
                needs_quoting = true;
        }

        if (needs_quoting)
            break;
    }

    // No quoting needed, so just return our input.
    if (!needs_quoting)
        return str;

    // Quote it.
    wstring out(L"\"");
    for (size_t i = 0; i < str.size(); ++i) {
        wchar_t w = str[i];
        if (w == L'\\' || w == L'"')
            out += L"\\";
        out += w;
    }
    out += L"\"";
    return out;
}
