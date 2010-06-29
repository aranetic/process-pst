#include <cstdlib>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <vector>
#include <iconv.h>

#include "utilities.h"
#include "md5.h"

using namespace std;

// Convert from the current locale's 8-bit encoding to a wstring.
wstring string_to_wstring(const string &str) {
    size_t output_length = ::mbstowcs(NULL, str.c_str(), 0);
    if (output_length == size_t(-1))
        throw runtime_error("Cannot convert malformed string to wstring");

    vector<wchar_t> wvec(output_length);
    output_length = ::mbstowcs(&wvec[0], str.c_str(), wvec.size());
    if (output_length == size_t(-1))
        throw runtime_error("Cannot convert malformed string to wstring");

    return wstring(wvec.begin(), wvec.end());
}

// Convert from a wstring to the current locale's 8-bit encoding.
string wstring_to_string(const wstring &wstr) {
    size_t output_length = ::wcstombs(NULL, wstr.c_str(), 0);
    if (output_length == size_t(-1))
        throw runtime_error("Cannot convert malformed wstring to string");

    vector<char> vec(output_length);
    output_length = ::wcstombs(&vec[0], wstr.c_str(), vec.size());
    if (output_length == size_t(-1))
        throw runtime_error("Cannot convert malformed wstring to string");

    return string(vec.begin(), vec.end());
}

namespace {
    // This will need to be customized for various platforms.  Sadly,
    // "WCHAR_T" does not appear to work as a source character set
    // with GNU iconv.
    const char *wchar_t_encoding = "UTF-32LE";
}

/// Convert from a wstring to a UTF-8 encoded string, regardless of the
/// current locale's encoding.
string wstring_to_utf8(const wstring &wstr) {
    // Handle the empty string, which breaks iconv on some platforms.
    if (wstr.empty())
        return string();

    // Open a conversion context.
    //iconv_t cd(::iconv_open("UTF-8", "WCHAR_T"));
    iconv_t cd(::iconv_open("UTF-8", const_cast<char *>(wchar_t_encoding)));
    if (cd == (iconv_t)(-1))
        throw std::runtime_error("Unable to convert a wstring to UTF-8");

    // Allocate up to 4 UTF-8 bytes per Unicode character, which is all that
    // is currently permitted by Unicode (and by Windows' UTF-16 encoding).
    vector<char> utf8(wstr.size() * 4);
    
    // Attempt to perform the conversion.
    const char *inbuf(reinterpret_cast<const char *>(wstr.c_str()));
    size_t inbytesleft(wstr.size() * sizeof(wchar_t));
    char *outbuf(&utf8[0]);
    size_t outbytesleft(utf8.size());
    size_t result = ::iconv(cd, const_cast<char **>(&inbuf), &inbytesleft,
                            &outbuf, &outbytesleft);

    // Close our conversion context.
    ::iconv_close(cd);

    // Check for conversion failure.
    if (result == size_t(-1) || inbytesleft > 0) {
        perror("wstring_to_utf8");
        throw runtime_error("Error converting a wstring to UTF-8");
    }
    
    // Convert our output to a string and return it.
    size_t output_size(utf8.size() - outbytesleft);
    return string(utf8.begin(), utf8.begin() + output_size);
}

string md5(const vector<uint8_t> &v) {
    // Calculate the MD5 sum.
    md5_state_s pms;
    md5_init(&pms);
    md5_append(&pms, &v[0], v.size());
    md5_byte_t digest[16];
    md5_finish(&pms, digest);

    // Convert to hexadecimal string.
    ostringstream out;
    for (size_t i = 0; i < 16; i++)
        out << hex << setw(2) << setfill('0') << uint32_t(digest[i]);
    return out.str();
}

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
