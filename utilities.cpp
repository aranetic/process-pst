#include <cstdlib>
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
