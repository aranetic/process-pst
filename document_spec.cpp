#include <cassert>

#include "document.h"

using namespace std;
using boost::any_cast;

void document_should_have_a_zero_arg_constructor() {
    document d;
    (void) d;
}

void document_tags_should_be_accessible_using_subscript_operator() {
    document d;
    d[L"#Subject"] = wstring(L"Hello!");
    assert(L"Hello!" == any_cast<wstring>(d[L"#Subject"]));
    d[L"#Subject"] = wstring(L"Hello, again!");
    assert(L"Hello, again!" == any_cast<wstring>(d[L"#Subject"]));
}

void document_tags_should_default_to_boost_any_empty_nonexistent_tag() {
    document d;
    assert(d[L"#Nonexistent"].empty());
}

int document_spec(int argc, char **argv) {
    document_should_have_a_zero_arg_constructor();
    document_tags_should_be_accessible_using_subscript_operator();
    document_tags_should_default_to_boost_any_empty_nonexistent_tag();

    return 0;
}
