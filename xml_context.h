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

#ifndef XML_CONTEXT_H
#define XML_CONTEXT_H

#include <string>
#include <iostream>

#include <boost/utility.hpp>

/// A somewhat eccentric XML writer class refactored out of the EDRM
/// output code.  The API is decidedly odd, but it seems to match our
/// needs.
class xml_context : boost::noncopyable {
    std::ostream &m_out;
    int m_indent;

    void indent();

public:
    xml_context(std::ostream &out);

    xml_context &lt(const std::string &tag_name);
    xml_context &attr(const std::string &name, const std::wstring &value);
    void gt();
    void slash_gt();

    void end_tag(const std::string &tag_name);
};

#endif // XML_CONTEXT_H
