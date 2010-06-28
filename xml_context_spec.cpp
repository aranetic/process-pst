#include <cassert>
#include <sstream>

#include "xml_context.h"

using namespace std;

void xml_context_should_output_xml_document() {
    ostringstream out;

    xml_context x(out);
    x.lt("Foo").gt();
    x  .lt("Bar").attr("Baz", L"& Moby").gt();
    x  .end_tag("Bar");
    x  .lt("Quux").slash_gt();
    x.end_tag("Foo");

    const char *expected =
        "<?xml version='1.0' encoding='UTF-8'?>\n"
        "<Foo>\n"
        "  <Bar Baz='&amp; Moby'>\n"
        "  </Bar>\n"
        "  <Quux/>\n"
        "</Foo>\n";
    assert(expected == out.str());
}

int xml_context_spec(int argc, char **argv) {
    xml_context_should_output_xml_document();

    return 0;
}

