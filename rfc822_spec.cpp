#include <cassert>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "rfc822.h"
#include "utilities.h"
#include "document.h"

using namespace std;
using namespace boost::posix_time;

const wchar_t *long_wchar_t_string =
    L"The quick brown fox jumped over the lazy dog\u2014or did she?  "
    L"The quick brown fox jumped over the lazy dog\u2014or did she?  "
    L"The quick brown fox jumped over the lazy dog\u2014or did she?";

const char *long_utf8_string =
    "The quick brown fox jumped over the lazy dog\xE2\x80\x94or did she?  "
    "The quick brown fox jumped over the lazy dog\xE2\x80\x94or did she?  "
    "The quick brown fox jumped over the lazy dog\xE2\x80\x94or did she?";

const char *long_utf8_string_base64 =
"VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wZWQgb3ZlciB0aGUgbGF6eSBkb2figJRvciBkaWQg\r\n"
"c2hlPyAgVGhlIHF1aWNrIGJyb3duIGZveCBqdW1wZWQgb3ZlciB0aGUgbGF6eSBkb2figJRv\r\n"
"ciBkaWQgc2hlPyAgVGhlIHF1aWNrIGJyb3duIGZveCBqdW1wZWQgb3ZlciB0aGUgbGF6eSBk\r\n"
"b2figJRvciBkaWQgc2hlPw==\r\n";

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

void base64_wrapped_should_encode_string_with_line_breaks() {
    assert("YQ==\r\n" == base64_wrapped("a"));
    assert(long_utf8_string_base64 == base64_wrapped(long_utf8_string));
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

void header_should_turn_a_list_of_emails_into_a_structured_header() {
    vector<wstring> addresses;
    addresses.push_back(L"Foo <foo@bar.com>");
    addresses.push_back(L"\"Foo\u2014Bar\" <foo@bar.com>");
    assert("To: Foo <foo@bar.com>,\r\n  " +
           header_encode_email(L"\"Foo\u2014Bar\" <foo@bar.com>") ==
           header("To", addresses));
}

void header_should_format_dates() {
    assert("Date: 31 Jan 2002 23:59:59 GMT" ==
           header("Date", from_iso_string("20020131T235959Z")));
}

void document_to_rfc822_should_include_headers_text_and_html() {
    document d;

    vector<wstring> from;
    from.push_back(L"Foo <foo@example.com>");
    d[L"#From"] = from;

    vector<wstring> to;
    to.push_back(L"Bar <bar@example.com>");
    d[L"#To"] = to;

    vector<wstring> cc;
    cc.push_back(L"Baz <baz@example.com>");
    cc.push_back(L"Moby <moby@example.com>");
    d[L"#CC"] = cc;

    vector<wstring> bcc;
    bcc.push_back(L"Quux <quux@example.com>");
    d[L"#BCC"] = bcc;

    d[L"#Subject"] = wstring(L"Re: The fridge");
    d[L"#DateSent"] = from_iso_string("20020131T235959Z");
    d[L"#Header"] = wstring(L"Subject: Re: the fridge\r\n");

    d.set_text(long_wchar_t_string);

    // TODO: HTML body
    string html("<p>The quick brown fox jumped over the lazy dog.</p>");
    d.set_html(vector<uint8_t>(html.begin(), html.end()));

    const char *expected =
        "From: Foo <foo@example.com>\r\n"
        "Subject: Re: The fridge\r\n"
        "Date: 31 Jan 2002 23:59:59 GMT\r\n"
        "To: Bar <bar@example.com>\r\n"
        "CC: Baz <baz@example.com>,\r\n"
        "  Moby <moby@example.com>\r\n"
        "BCC: Quux <quux@example.com>\r\n"
        "MIME-Version: 1.0\r\n"
        "Content-Type: multipart/alternative; boundary=\"=_boundary\"\r\n"
        "X-Note: Exported from PST by"
        " http://github.com/aranetic/process-pst\r\n"
        "X-Note: See load file metadata for original headers\r\n"
        "\r\n"
        "--=_boundary\r\n"
        "Content-Type: text/plain; charset=UTF-8\r\n"
        "Content-Transfer-Encoding: base64\r\n"
        "\r\n"
"VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wZWQgb3ZlciB0aGUgbGF6eSBkb2figJRvciBkaWQg\r\n"
"c2hlPyAgVGhlIHF1aWNrIGJyb3duIGZveCBqdW1wZWQgb3ZlciB0aGUgbGF6eSBkb2figJRv\r\n"
"ciBkaWQgc2hlPyAgVGhlIHF1aWNrIGJyb3duIGZveCBqdW1wZWQgb3ZlciB0aGUgbGF6eSBk\r\n"
"b2figJRvciBkaWQgc2hlPw==\r\n"
        "--=_boundary\r\n"
        "Content-Type: text/html\r\n"
        "Content-Transfer-Encoding: base64\r\n"
        "\r\n"
"PHA+VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wZWQgb3ZlciB0aGUgbGF6eSBkb2cuPC9wPg==\r\n"
        "--=_boundary--\r\n";
    
    ostringstream out;
    document_to_rfc822(out, d);
    assert(expected == out.str());
}

int rfc822_spec(int argc, char **argv) {
    rfc822_quote_should_quote_strings_when_necessary();
    rfc822_email_should_build_email_addresses();

    base64_should_encode_string();
    base64_wrapped_should_encode_string_with_line_breaks();

    contains_special_characters_should_detect_non_ascii_characters();

    header_encode_should_encode_special_characters();
    header_encode_email_should_encode_special_characters();

    header_should_turn_a_string_into_a_freeform_header();
    header_should_turn_a_list_of_emails_into_a_structured_header();
    header_should_format_dates();

    document_to_rfc822_should_include_headers_text_and_html();

    return 0;
}
