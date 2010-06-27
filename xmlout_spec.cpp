#include <cassert>

#include "xmlout.h"

using namespace std;

void xml_quote_should_convert_wstring_and_escape_metacharacters() {
    assert("" == xml_quote(L""));
    assert("test" == xml_quote(L"test"));
    assert("\xE2\x80\x94" == xml_quote(L"\u2014"));
    assert("&lt;&amp;&quot;&apos;&gt;" == xml_quote(L"<&\"'>"));
}

int xmlout_spec(int argc, char **argv) {
    xml_quote_should_convert_wstring_and_escape_metacharacters();

    return 0;
}

