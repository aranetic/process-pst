#include <cassert>
#include "utilities.h"

using namespace std;

void string_to_wstring_should_handle_empty_string() {
    assert(L"" == string_to_wstring(""));
}

void string_to_wstring_should_convert_ascii_to_wstring() {
    assert(L"text" == string_to_wstring("text"));
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

int utilities_spec(int argc, char **argv) {
    string_to_wstring_should_handle_empty_string();
    string_to_wstring_should_convert_ascii_to_wstring();

    rfc822_quote_should_quote_strings_when_necessary();

    return 0;
}

