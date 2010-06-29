#include <cassert>

#include "rfc822.h"

using namespace std;

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

void base64_should_encode_string() {
    assert("" == base64(""));
    assert("YQ==" == base64("a"));
    assert("YWI=" == base64("ab"));
    assert("YWJj" == base64("abc"));
    assert("YWJjZA==" == base64("abcd"));
    assert("YWJjZGU=" == base64("abcde"));
    assert("YWJjZGVm" == base64("abcdef"));
    assert("VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wZWQgb3ZlciB0aGUgbGF6eSBkb2cu" ==
           base64("The quick brown fox jumped over the lazy dog."));

    // TODO: Make sure we encode strings containing internal NULLs.
}

void contains_special_characters_should_detect_non_ascii_characters() {
    assert(!contains_special_characters(""));
    assert(!contains_special_characters("plain text!"));
    assert(contains_special_characters("\t"));
    assert(contains_special_characters("\xE2\x80\x94"));
}

void header_encode_should_encode_special_characters() {
    assert("" == header_encode(""));
    assert("Re: The fridge" == header_encode("Re: The fridge"));

    string utf8("Re: The fridge\xE2\x80\x94it's evil!");
    assert("=?UTF-8?B?" + base64(utf8) + "?=" == header_encode(utf8));
}

int rfc822_spec(int argc, char **argv) {
    rfc822_quote_should_quote_strings_when_necessary();
    rfc822_email_should_build_email_addresses();

    base64_should_encode_string();

    contains_special_characters_should_detect_non_ascii_characters();

    header_encode_should_encode_special_characters();
    //encode_email_address_should_encode_special_characters();

    //freeform_header_should_generate_a_header()
    //email_address_header_should_generate_a_header()

    //document_to_rfc822_should_include_headers_text_and_html();

    return 0;
}
