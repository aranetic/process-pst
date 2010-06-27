#include <cassert>
#include "utilities.h"

using namespace std;

void string_to_wstring_should_handle_empty_string() {
    assert(L"" == string_to_wstring(""));
}

void string_to_wstring_should_convert_ascii_to_wstring() {
    assert(L"text" == string_to_wstring("text"));
}

void wstring_to_utf8_should_convert_to_utf8() {
    assert("" == wstring_to_utf8(L""));
    assert("text" == wstring_to_utf8(L"text"));
    assert(0x2014 == wstring(L"\u2014")[0]);
    assert("\xE2\x80\x94" == wstring_to_utf8(L"\u2014")); // em-dash
}

void rfc822_quote_should_quote_strings_when_necessary() {
    assert(L"" == rfc822_quote(L""));
    assert(L"John Smith" == rfc822_quote(L"John Smith"));
    assert(L"\"John Q. Smith\"" == rfc822_quote(L"John Q. Smith"));
    assert(L"\"\\\"\\\\\"" == rfc822_quote(L"\"\\"));
    
    wstring escape_chars(L"()<>@,;:.[]"); // Omits " and \, handled above.
    for (size_t i = 0; i < escape_chars.size(); ++i) {
        wstring input(escape_chars.substr(i, 1));
        wstring expected(L"\"" + input + L"\"");
        assert(expected == rfc822_quote(input));
    }
}

void rfc822_email_should_build_email_addresses() {
    assert(L"Foo <foo@bar.com>" == rfc822_email(L"Foo", L"foo@bar.com"));
    assert(L"Foo" == rfc822_email(L"Foo", L""));
    assert(L"\"Foo B.\"" == rfc822_email(L"Foo B.", L""));
    assert(L"foo@bar.com" == rfc822_email(L"", L"foo@bar.com"));
    assert(L"foo@bar.com" == rfc822_email(L"foo@bar.com", L"foo@bar.com"));
    assert(L"\"Foo B.\" <foo@bar.com>" ==
           rfc822_email(L"Foo B.", L"foo@bar.com"));
}

int utilities_spec(int argc, char **argv) {
    string_to_wstring_should_handle_empty_string();
    string_to_wstring_should_convert_ascii_to_wstring();

    wstring_to_utf8_should_convert_to_utf8();

    rfc822_quote_should_quote_strings_when_necessary();
    rfc822_email_should_build_email_addresses();

    return 0;
}

