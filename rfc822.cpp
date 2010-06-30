#include <cctype>
#include <algorithm>

#include <boost/foreach.hpp>
#include <boost/serialization/pfto.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>

#include "utilities.h"
#include "rfc822.h"

using namespace std;
using namespace boost::archive::iterators;

/// Quote everything except RFC 822 "atom" characters and spaces.  This
/// is normally used for the human-readable parts of email addresses.
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

/// Given a display_name (from Outlook) and an email address, build an
/// RFC822-formatted email address.  Note that our return value still
/// contains potential Unicode characters at this point, because we'll
/// serialize it to XML, where Unicode is both easy and convenient.
wstring rfc822_email(const wstring &display_name, const wstring &email) {
    if (display_name.empty() && email.empty())
        throw runtime_error("Can't build address without name or email!");
    else if (display_name.empty() || display_name == email)
        return email;
    else if (email.empty())
        return rfc822_quote(display_name);
    else
        return rfc822_quote(display_name) + L" <" + email + L">";
}

namespace {
    // See the following for information on Base64 encoding with boost:
    // http://www.boost.org/doc/libs/1_42_0/libs/serialization/doc/dataflow.html
    typedef transform_width<const char *, 6, 8> break_into_6bit_chunks;
    typedef base64_from_binary<break_into_6bit_chunks> base64_iterator;
    typedef insert_linebreaks<base64_iterator, 72> base64_lines_iterator;
}

/// Encode a string using Base64.
string base64(const string &input) {
    // Let Boost do the heavy conversion work.
    string output;
    copy(base64_iterator(BOOST_MAKE_PFTO_WRAPPER(input.data())),
         base64_iterator(BOOST_MAKE_PFTO_WRAPPER(input.data() + input.size())),
         insert_iterator<string>(output, output.begin()));

    // Add padding so the decoder can tell how many bytes were actually
    // encoded.
    size_t leftover_bits = (output.size() * 6) % 8;
    if (leftover_bits == 4)
        output += "==";
    else if (leftover_bits == 2)
        output += "=";

    return output;
}

/// Does 'str' contain anything other than printable ASCII characters and
/// spaces?
bool contains_special_characters(const string &str) {
    BOOST_FOREACH(char c, str) {
        if (c >= 0x80 || (!isgraph(c) && c != ' '))
            return true;
    }
    return false;
}

/// Encode a free-form RFC822 header value if necessary.  Currently, this
/// uses B-encoding, but there's no reason why it couldn't prefer Q-encoding
/// in some cases.
string header_encode(const wstring &str) {
    string utf8_str(wstring_to_utf8(str));
    if (contains_special_characters(utf8_str))
        return "=?UTF-8?B?" + base64(utf8_str) + "?=";
    else
        return utf8_str;
}

/// Encode an email address for use in a structured RFC822 header.
string header_encode_email(const wstring &email) {
    // Pass the simple cases through unchanged.  Note that if the email
    // address contains Unicode characters, but isn't properly quoted, we
    // just pass it straight through and hope for the best.
    string utf8_str(wstring_to_utf8(email));
    if (email.size() == 0 || !contains_special_characters(utf8_str) ||
        email[0] != L'"')
        return utf8_str;

    // Parse the name back out of the email address.  This isn't terribly
    // robust, because it assumes that our input was produced by
    // rfc822_email.
    wstring unquoted_name;
    size_t i;
    for (i = 1; i < email.size() && email[i] != L'"'; ++i) {
        wchar_t c = email[i];
        if (c == L'\\' && (i+1) < email.size())
            unquoted_name += email[++i];
        else
            unquoted_name += c;
    }
    wstring remainder;
    if (i < email.size())
        remainder = email.substr(i+1, string::npos);

    // Now that the name has been unquoted, encode it.  Note we should do
    // something more clever with Unicode characters in 'remainder'.
    return header_encode(unquoted_name) + wstring_to_utf8(remainder);
}

/// Encode a freeform header.
string header(const string &name, const wstring &value) {
    return name + ": " + header_encode(value);
}

