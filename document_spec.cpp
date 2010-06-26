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
}

void document_should_have_an_id_a_type_and_a_content_type() {
    document d;
    d.set_id(L"DOC1").set_type(document::message);
    assert(L"DOC1" == d.id());
    assert(document::message == d.type());

    d.set_content_type(L"message/rfc822");
    assert(L"message/rfc822" == d.content_type());
}

void document_tags_should_be_accessible_using_subscript_operator() {
    document d;
    d[L"#Subject"] = wstring(L"Hello!");
    assert(L"Hello!" == any_cast<wstring>(d[L"#Subject"]));
    d[L"#Subject"] = wstring(L"Hello, again!");
    assert(L"Hello, again!" == any_cast<wstring>(d[L"#Subject"]));
}

void document_tags_should_default_to_boost_any_empty() {
    document d;
    assert(d[L"#Nonexistent"].empty());
}

void document_from_message_should_fill_in_basic_edrm_data() {
    pst test_pst(L"test_data/flags_jane_doe.pst");
    message m(find_by_subject(test_pst, L"Unread email (do not open)"));
    document d(m);

    // DocId
    assert(document::message == d.type());
    // MimeType
    // #From (From and Sender?)
    // #To
    // #CC
    // #BCC
    assert(L"Unread email (do not open)" == any_cast<wstring>(d[L"#Subject"]));
    assert(L"Return-Path:" == any_cast<wstring>(d[L"#Header"]).substr(0, 12));
    assert(from_iso_string("20100624T191617Z") ==
           any_cast<ptime>(d[L"#DateSent"]));
    assert(from_iso_string("20100624T191619Z") ==
           any_cast<ptime>(d[L"#DateReceived"]));
    // #HasAttachments, #AttachmentCount, #AttachmentNames
    assert(false == any_cast<bool>(d[L"#ReadFlag"]));
    assert(false == any_cast<bool>(d[L"#ImportanceFlag"]));
    assert(L"IPM.Note" == any_cast<wstring>(d[L"#MessageClass"]));
    assert(d[L"#FlagStatus"].empty());
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
    assert(2 == any_cast<int32_t>(d[L"#FlagStatus"]));
}

void document_from_attachment_should_fill_in_basic_edrm_data() {
    pst test_pst(L"pstsdk/test/sample1.pst");
    message m(find_by_subject(test_pst, L"Here is a sample message"));
    document d(*m.attachment_begin());

    // DocId
    assert(document::file == d.type());
    // MimeType
    assert(L"leah_thumper.jpg" == any_cast<wstring>(d[L"#FileName"]));
    assert(L"jpg" == any_cast<wstring>(d[L"#FileExtension"]));
    assert(96808 == any_cast<uint64_t>(d[L"#FileSize"]));
    // Unsupported: #DateCreated, #DateAccessed, #DateModified, #DatePrinted
    // (plus Microsoft Office metadata, but that's not our problem for now)
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

    document_tags_should_be_accessible_using_subscript_operator();
    document_tags_should_default_to_boost_any_empty();

    document_from_message_should_fill_in_basic_edrm_data();
    document_from_message_should_mark_read_messages();
    document_from_message_should_mark_important_messages();
    document_from_message_should_include_flag_status();

    document_from_attachment_should_fill_in_basic_edrm_data();
    document_from_attachment_should_recognize_submessage_attachment();
    
    return 0;
}
