// -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*-
//
// process-pst: Convert PST files to RCF822 *.eml files and load files
// Copyright (c) 2010 Aranetic LLC
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

#include <stdexcept>

#include "utilities.h"
#include "xml_context.h"

using namespace std;

void xml_context::indent() {
    for (int i = 0; i < m_indent; ++i)
        m_out << "  ";
}

xml_context::xml_context(ostream &out) : m_out(out), m_indent(0) {
    m_out << "<?xml version='1.0' encoding='UTF-8'?>" << endl;
}

xml_context &xml_context::lt(const string &tag_name) {
    indent();
    m_out << "<" << tag_name;
    return *this;
}

xml_context &xml_context::attr(const string &name, const wstring &value) {
    m_out << " " << name << "='" << xml_quote(value) << "'";
    return *this;
}

void xml_context::gt() {
    m_out << ">" << endl;
    ++m_indent;
}

void xml_context::slash_gt() {
    m_out << "/>" << endl;
}

void xml_context::end_tag(const string &tag_name) {
    if (m_indent < 1)
        throw runtime_error("Unbalanced tags in XML");
    --m_indent;
    indent();
    m_out << "</" << tag_name << ">" << endl;
}
