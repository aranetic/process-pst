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

#ifndef RFC822_H
#define RFC822_H

#include <string>

namespace boost { namespace posix_time { class ptime; } }
class document;

extern std::wstring rfc822_quote(const std::wstring &str);
extern std::wstring rfc822_email(const std::wstring &display_name,
                                 const std::wstring &email);

extern std::string base64(const std::string &input);
extern std::string base64_wrapped(const std::string &input);
extern bool contains_special_characters(const std::string &str);
extern std::string header_encode(const std::wstring &str);
extern std::string header_encode_email(const std::wstring &email);
extern std::string header(const std::string &name, const std::wstring &value);
extern std::string header(const std::string &name,
                          const std::vector<std::wstring> &emails);
extern std::string header(const std::string &name,
                          const boost::posix_time::ptime &time);

extern void document_to_rfc822(std::ostream &out, const document &d);

#endif // RFC822_H

