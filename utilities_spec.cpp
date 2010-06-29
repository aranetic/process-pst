#include <cassert>
#include "utilities.h"

using namespace std;

void string_to_wstring_should_convert_native_8_bit_to_unicode() {
    assert(L"" == string_to_wstring(""));
    assert(L"text" == string_to_wstring("text"));
}

void wstring_to_string_should_convert_unicode_to_native_8_bit() {
    assert("" == wstring_to_string(L""));
    assert("text" == wstring_to_string(L"text"));
}

void wstring_to_utf8_should_convert_to_utf8() {
    assert("" == wstring_to_utf8(L""));
    assert("text" == wstring_to_utf8(L"text"));
    assert(0x2014 == wstring(L"\u2014")[0]);
    assert("\xE2\x80\x94" == wstring_to_utf8(L"\u2014")); // em-dash
}

void md5_should_calculate_md5_hash_for_vector() {
    string s("Data");
    vector<uint8_t> v(s.begin(), s.end());
    assert("f6068daa29dbb05a7ead1e3b5a48bbee" == md5(v));
}

void xml_quote_should_convert_wstring_and_escape_metacharacters() {
    assert("" == xml_quote(L""));
    assert("test" == xml_quote(L"test"));
    assert("\xE2\x80\x94" == xml_quote(L"\u2014"));
    assert("&lt;&amp;&quot;&apos;&gt;" == xml_quote(L"<&\"'>"));
}

int utilities_spec(int argc, char **argv) {
    string_to_wstring_should_convert_native_8_bit_to_unicode();
    wstring_to_string_should_convert_unicode_to_native_8_bit();

    wstring_to_utf8_should_convert_to_utf8();

    md5_should_calculate_md5_hash_for_vector();

    xml_quote_should_convert_wstring_and_escape_metacharacters();

    return 0;
}

