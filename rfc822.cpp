#include <cctype>
#include <algorithm>

#include <boost/foreach.hpp>
#include <boost/serialization/pfto.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>

#include "rfc822.h"

using namespace std;
using namespace boost::archive::iterators;

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
