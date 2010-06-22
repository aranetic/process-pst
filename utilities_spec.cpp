#include <cassert>
#include "utilities.h"

void string_to_wstring_should_handle_empty_string() {
    assert(L"" == string_to_wstring(""));
}

void string_to_wstring_should_convert_ascii_to_wstring() {
    assert(L"text" == string_to_wstring("text"));
}

int utilities_spec(int argc, char **argv) {
    string_to_wstring_should_handle_empty_string();
    string_to_wstring_should_convert_ascii_to_wstring();
    return 0;
}

