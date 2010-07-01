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

#include <cassert>
#include "utilities.h"

using namespace std;

void string_to_wstring_should_convert_native_8_bit_to_unicode() {
    assert(L"" == string_to_wstring(""));
    assert(L"text" == string_to_wstring("text"));
}

void wstring_to_string_should_convert_unicode_to_native_8_bit() {
    assert("" == wstring_to_string(L""));
    assert("text" == wstring_to_string(L"text"));
}

void wstring_to_utf8_should_convert_to_utf8() {
    assert("" == wstring_to_utf8(L""));
    assert("text" == wstring_to_utf8(L"text"));
    assert(0x2014 == wstring(L"\u2014")[0]);
    assert("\xE2\x80\x94" == wstring_to_utf8(L"\u2014")); // em-dash
}

void md5_should_calculate_md5_hash_for_vector() {
    string s("Data");
    vector<uint8_t> v(s.begin(), s.end());
    assert("f6068daa29dbb05a7ead1e3b5a48bbee" == md5(v));
}

void xml_quote_should_convert_wstring_and_escape_metacharacters() {
    assert("" == xml_quote(L""));
    assert("test" == xml_quote(L"test"));
    assert("\xE2\x80\x94" == xml_quote(L"\u2014"));
    assert("&lt;&amp;&quot;&apos;&gt;" == xml_quote(L"<&\"'>"));
}

int utilities_spec(int argc, char **argv) {
    string_to_wstring_should_convert_native_8_bit_to_unicode();
    wstring_to_string_should_convert_unicode_to_native_8_bit();

    wstring_to_utf8_should_convert_to_utf8();

    md5_should_calculate_md5_hash_for_vector();

    xml_quote_should_convert_wstring_and_escape_metacharacters();

    return 0;
}

