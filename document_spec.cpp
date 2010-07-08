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
#include <stdexcept>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <pstsdk/pst.h>

#include "document.h"

using namespace std;
using boost::any_cast;
using namespace boost::posix_time;
using namespace pstsdk;

bool has_subject(const message &m, const wstring &subject) {
    try {
        return m.get_subject() == subject;
    } catch (key_not_found<prop_id> &) {
        return false;
    }
}

message find_by_subject(const pst &pst_file, const wstring &subject) {
    // I would use find_if/bind here, but in GCC 4.4, it still gives errors
    // which look this this: http://gcc.gnu.org/bugzilla/show_bug.cgi?id=35569
    pst::message_iterator iter(pst_file.message_begin());
    pst::message_iterator end(pst_file.message_end());
    for (; iter != end; ++iter)
        if (has_subject(*iter, subject))
            return *iter;
    throw runtime_error("Cannot find message");
}

void document_should_have_a_zero_arg_constructor() {
    document d;
    assert(document::unknown == d.type());
    assert(!d.has_native());
    assert(!d.has_text());
    assert(!d.has_html());
}

void document_should_have_an_id_a_type_and_a_content_type() {
    document d;
    d.set_id(L"DOC1").set_type(document::message);
    assert(L"DOC1" == d.id());
    assert(document::message == d.type());

    d.set_content_type(L"message/rfc822");
    assert(L"message/rfc822" == d.content_type());
}

void document_should_have_native_text_and_html_fields() {
    document d;

    string data_str("Data");
    vector<uint8_t> data(data_str.begin(), data_str.end());

    d.set_native(data);
    assert(d.has_native());
    assert(data == d.native());

    d.set_text(L"Hello!");
    assert(d.has_text());
    assert(L"Hello!" == d.text());

    d.set_html(data);
    assert(d.has_html());
    assert(data == d.html());
}

void document_should_be_able_to_translate_type_to_string() {
    document d;
    d.set_type(document::message);
    assert(L"Message" == d.type_string());
    d.set_type(document::file);
    assert(L"File" == d.type_string());

    bool caught_exception = false;
    try {
        d.set_type(document::unknown);
        d.type_string();
    } catch (exception &) {
        caught_exception = true;
    }
    assert(caught_exception);
}

void document_tags_should_be_accessible_using_subscript_operator() {
    document d;
    d[L"#Subject"] = wstring(L"Hello!");
    assert(L"Hello!" == any_cast<wstring>(d[L"#Subject"]));
    d[L"#Subject"] = wstring(L"Hello, again!");
    assert(L"Hello, again!" == any_cast<wstring>(d[L"#Subject"]));

    const document &cd(d);
    assert(L"Hello, again!" == any_cast<wstring>(cd[L"#Subject"]));
}

void document_tags_should_default_to_boost_any_empty() {
    document d;
    assert(d[L"#Nonexistent"].empty());

    const document &cd(d);
    assert(cd[L"#Nonexistent"].empty());
}

void document_tags_should_support_iteration() {
    document d;
    d[L"#Subject"] = wstring(L"Hello!");
    document::tag_iterator i(d.tag_begin());
    size_t count = 0;
    for (; i != d.tag_end(); ++i) {
        assert(L"#Subject" == i->first);
        assert(L"Hello!" == any_cast<wstring>(i->second));
        ++count;
    }
    assert(1 == count);
}

void document_from_message_should_fill_in_basic_edrm_data() {
    pst test_pst(L"test_data/flags_jane_doe.pst");
    message m(find_by_subject(test_pst, L"Unread email (do not open)"));
    document d(m);

    // DocId
    assert(document::message == d.type());
    // MimeType
    assert(L"John Doe <pst-test-1@aranetic.com>" ==
           any_cast<vector<wstring> >(d[L"#From"])[0]);
    assert(L"Jane Doe <pst-test-2@aranetic.com>" ==
           any_cast<vector<wstring> >(d[L"#To"])[0]);
    assert(d[L"#CC"].empty());
    assert(d[L"#BCC"].empty());
    assert(L"Unread email (do not open)" == any_cast<wstring>(d[L"#Subject"]));
    assert(L"Return-Path:" == any_cast<wstring>(d[L"#Header"]).substr(0, 12));
    assert(from_iso_string("20100624T191617Z") ==
           any_cast<ptime>(d[L"#DateSent"]));
    assert(from_iso_string("20100624T191619Z") ==
           any_cast<ptime>(d[L"#DateReceived"]));
    assert(false == any_cast<bool>(d[L"#HasAttachments"]));
    assert(0 == any_cast<int32_t>(d[L"#AttachmentCount"]));
    assert(d[L"#AttachmentNames"].empty());
    assert(false == any_cast<bool>(d[L"#ReadFlag"]));
    assert(false == any_cast<bool>(d[L"#ImportanceFlag"]));
    assert(L"IPM.Note" == any_cast<wstring>(d[L"#MessageClass"]));
    assert(d[L"#FlagStatus"].empty());
}

void document_from_message_should_handle_alternative_smtp_recipient_info() {
    pst test_pst(L"pstsdk/test/sample1.pst");
    message m(find_by_subject(test_pst, L"Here is a sample message"));
    document d(m);

    assert(L"Terry Mahaffey <terrymah@microsoft.com>" ==
           any_cast<vector<wstring> >(d[L"#To"])[0]);
}

void document_from_message_should_handle_various_recipient_types() {
    pst test_pst(L"test_data/multiple_to_cc.pst");
    message m(find_by_subject(test_pst, L"Multiple recipients"));
    document d(m);

    vector<wstring> to(any_cast<vector<wstring> >(d[L"#To"]));
    assert(2 == to.size());
    assert(L"John Doe <pst-test-1@aranetic.com>" == to[0]);
    assert(L"Jane Doe <pst-test-2@aranetic.com>" == to[1]);

    vector<wstring> cc(any_cast<vector<wstring> >(d[L"#CC"]));
    assert(2 == cc.size());
    assert(L"pst-test-3@aranetic.com" == cc[0]);
    assert(L"pst-test-4@aranetic.com" == cc[1]);
}

void document_from_message_should_mark_read_messages() {
    pst test_pst(L"test_data/flags_jane_doe.pst");
    message m(find_by_subject(test_pst, L"Needed a response, and has one"));
    document d(m);
    assert(true == any_cast<bool>(d[L"#ReadFlag"]));
}

void document_from_message_should_mark_important_messages() {
    pst test_pst(L"test_data/flags_jane_doe.pst");
    message m(find_by_subject(test_pst, L"This email is important!"));
    document d(m);
    assert(true == any_cast<bool>(d[L"#ImportanceFlag"]));
}

void document_from_message_should_include_flag_status() {
    pst test_pst(L"test_data/flags_jane_doe.pst");
    message m(find_by_subject(test_pst, L"Needs response"));
    document d(m);
    assert(L"2" == any_cast<wstring>(d[L"#FlagStatus"]));
}

void document_from_message_should_include_attachment_metadata() {
    pst test_pst(L"pstsdk/test/sample1.pst");
    message m(find_by_subject(test_pst, L"Here is a sample message"));
    document d(m);

    assert(true == any_cast<bool>(d[L"#HasAttachments"]));
    assert(1 == any_cast<int32_t>(d[L"#AttachmentCount"]));

    vector<wstring> names(any_cast<vector<wstring> >(d[L"#AttachmentNames"]));
    assert(1 == names.size());
    assert(L"leah_thumper.jpg" == names[0]);
}

void document_from_message_should_use_subject_as_message_attachment_filename() {
    pst test_pst(L"test_data/four_nesting_levels.pst");
    message m(find_by_subject(test_pst, L"Outermost message"));
    document d(m);

    vector<wstring> names(any_cast<vector<wstring> >(d[L"#AttachmentNames"]));
    assert(L"Middle message" == names[0]);
}

void document_from_message_should_extract_text_file() {
    pst test_pst(L"test_data/flags_jane_doe.pst");
    message m(find_by_subject(test_pst, L"Unread email (do not open)"));
    document d(m);

    wstring expected(L"This email has never been read.");
    assert(d.has_text());
    assert(expected == d.text().substr(0, expected.size()));
}

void document_from_message_should_extract_binary_html() {
    pst test_pst(L"pstsdk/test/test_unicode.pst");
    message m(find_by_subject(test_pst, L"Test"));
    document d(m);

    assert(d.has_html());
    assert(static_cast<uint8_t>('<') == d.html()[0]);
}

void document_from_message_should_extract_text_html() {
    pst test_pst(L"pstsdk/test/test_ansi.pst");
    message m(find_by_subject(test_pst, L"Post"));
    document d(m);

    assert(d.has_html());
    assert(static_cast<uint8_t>('<') == d.html()[0]);
}

void document_from_attachment_should_fill_in_basic_edrm_data() {
    pst test_pst(L"pstsdk/test/sample1.pst");
    message m(find_by_subject(test_pst, L"Here is a sample message"));
    document d(*m.attachment_begin());

    // DocId
    assert(document::file == d.type());
    assert(L"" == d.content_type()); // No MIME types in this file.
    assert(L"leah_thumper.jpg" == any_cast<wstring>(d[L"#FileName"]));
    assert(L"jpg" == any_cast<wstring>(d[L"#FileExtension"]));
    assert(93142 == any_cast<int64_t>(d[L"#FileSize"]));
    // Unsupported: #DateCreated, #DateAccessed, #DateModified, #DatePrinted
    // (plus Microsoft Office metadata, but that's not our problem for now)
}

void document_from_attachment_should_include_mime_type() {
    // We have to dig around to find an attachment with this field.
    pst test_pst(L"test_data/four_nesting_levels.pst");
    message m1(find_by_subject(test_pst, L"Outermost message"));
    message m2(m1.attachment_begin()->open_as_message());
    message m3(m2.attachment_begin()->open_as_message());
    document d(*m3.attachment_begin());
    
    assert(L"text/plain" == d.content_type());
}

void document_from_attachment_should_extract_native_file() {
    pst test_pst(L"pstsdk/test/sample1.pst");
    message m(find_by_subject(test_pst, L"Here is a sample message"));
    document d(*m.attachment_begin());

    assert(d.has_native());
    assert(93142 == d.native().size());
}

void document_from_attachment_should_recognize_submessage_attachment() {
    pst test_pst(L"pstsdk/test/submessage.pst");
    wstring subj(L"This is a message which has an embedded message attached");
    message m(find_by_subject(test_pst, subj));
    document d(*m.attachment_begin());

    // We only check a few fields, on the assumption this uses the same
    // codepath as regular messages.
    assert(document::message == d.type());
    assert(L"This is an embedded message" == any_cast<wstring>(d[L"#Subject"]));
}

int document_spec(int argc, char **argv) {
    document_should_have_a_zero_arg_constructor();
    document_should_have_an_id_a_type_and_a_content_type();
    document_should_have_native_text_and_html_fields();

    document_tags_should_be_accessible_using_subscript_operator();
    document_tags_should_default_to_boost_any_empty();
    document_tags_should_support_iteration();

    document_from_message_should_fill_in_basic_edrm_data();
    // Add PidTagSentRepresenting* fields to #From?
    document_from_message_should_handle_alternative_smtp_recipient_info();
    document_from_message_should_handle_various_recipient_types();
    document_from_message_should_mark_read_messages();
    document_from_message_should_mark_important_messages();
    document_from_message_should_include_flag_status();
    document_from_message_should_include_attachment_metadata();
    document_from_message_should_use_subject_as_message_attachment_filename();
    // TODO: EDRM native "file" via reassembly.
    document_from_message_should_extract_text_file();
    document_from_message_should_extract_binary_html();
    document_from_message_should_extract_text_html();

    document_from_attachment_should_fill_in_basic_edrm_data();
    document_from_attachment_should_include_mime_type();
    document_from_attachment_should_extract_native_file();
    document_from_attachment_should_recognize_submessage_attachment();
    
    return 0;
}
