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

#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <vector>

extern std::wstring string_to_wstring(const std::string &str);
extern std::string wstring_to_string(const std::wstring &wstr);
extern std::string wstring_to_utf8(const std::wstring &wstr);
extern std::string md5(const std::vector<uint8_t> &v);
extern std::string xml_quote(const std::wstring &wstr);

#endif // UTILITIES_H
