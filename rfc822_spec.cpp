#include <cassert>

#include "rfc822.h"

using namespace std;

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

    // TODO: Make sure we encoding strings containing internal NULLs.
}

int rfc822_spec(int argc, char **argv) {
    base64_should_encode_string();

    return 0;
}
