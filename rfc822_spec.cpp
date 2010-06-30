#include <cassert>

#include "utilities.h"
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

    // Note that header_encode_email relies on all Unicode characters being
    // quoted as follows when re-encoding addresses to be 7-bit clean.
    assert(L"\"Foo\u2014Bar\" <foo@bar.com>" ==
           rfc822_email(L"Foo\u2014Bar", L"foo@bar.com"));
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
    assert("" == header_encode(L""));
    assert("Re: The fridge" == header_encode(L"Re: The fridge"));

    string utf8("Re: The fridge\xE2\x80\x94it's evil!");
    wstring wstr(L"Re: The fridge\u2014it's evil!");
    assert("=?UTF-8?B?" + base64(utf8) + "?=" == header_encode(wstr));
}

void header_encode_email_should_encode_special_characters() {
    assert("" == header_encode_email(L""));

    // Pass all simple ASCII cases through unchanged.
    const wchar_t *unchanged[] = {
        L"Foo <foo@bar.com>", L"Foo", L"\"Foo B.\"",
        L"foo@bar.com", L"\"Foo B.\" <foo@bar.com>",
        NULL
    };
    for (const wchar_t **iter = unchanged; *iter != NULL; ++iter)
        assert(wstring_to_string(*iter) == header_encode_email(*iter));

    // Two current limitations here: 1) We don't attempt to encode Unicode
    // characters appearing in the email address itself, and 2) we assume
    // that all Unicode characters in the name part were produced by
    // rfc822_email, and are therefore automatically quoted.
    assert(header_encode(L"Foo\u2014Bar") + " <foo@bar.com>" ==
           header_encode_email(L"\"Foo\u2014Bar\" <foo@bar.com>"));
    assert(header_encode(L"Foo\u2014Bar") ==
           header_encode_email(L"\"Foo\u2014Bar\""));
    assert(header_encode(L" \" \u2014 \\ ") ==
           header_encode_email(L"\" \\\" \u2014 \\\\ \""));
    assert(header_encode(L"\u2014") ==
           header_encode_email(L"\"\u2014"));
}

void header_should_turn_a_string_into_a_freeform_header() {
    assert("Subject: Re: The fridge" == header("Subject", L"Re: The fridge"));
    wstring wstr(L"Re: The fridge\u2014it's evil!");
    assert("Subject: " + header_encode(wstr) == header("Subject", wstr));
}

int rfc822_spec(int argc, char **argv) {
    rfc822_quote_should_quote_strings_when_necessary();
    rfc822_email_should_build_email_addresses();

    base64_should_encode_string();

    contains_special_characters_should_detect_non_ascii_characters();

    header_encode_should_encode_special_characters();
    header_encode_email_should_encode_special_characters();

    header_should_turn_a_string_into_a_freeform_header();
    //header_should_turn_a_list_of_strings_into_a_structured_header()

    //document_to_rfc822_should_include_headers_text_and_html();

    return 0;
}
