// -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*-
//
// process-pst: Convert PST files to RCF822 *.eml files and load files
// Copyright (c) 2010 Aranetic LLC
// Look for the latest version at http://github.com/aranetic/process-pst
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU Affero General Public License (the "License")
// as published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but it
// is provided on an "AS-IS" basis and WITHOUT ANY WARRANTY; without even
// the implied warranties of MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NONINFRINGEMENT.  See the GNU Affero General Public License
// for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

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

