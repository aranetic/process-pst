#include <cstdint>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <pstsdk/pst.h>

#include "utilities.h"
#include "document.h"

using namespace std;
using boost::any;
using namespace boost::posix_time;
using namespace pstsdk;

namespace {
    // Rather than spend quite so much time checking for exceptions, let's
    // build a simple wrapper function which checks for us.
    template <typename R, typename T>
    bool has_prop(const T &o, R (T::*pmf)() const) {
        try {
            (o.*pmf)();
            return true;
        } catch (key_not_found<prop_id> &) {
            return false;
        }
    }

    wstring recipient_address(const recipient &r) {
        const_table_row props(r.get_property_row());

        // Email addresses may be stored in either PidTagPrimarySmtpAddress
        // or PidTagEmailAddress.  The latter may or may not be SMTP.
        wstring email;
        if (has_prop(r, &recipient::get_email_address))
            email = r.get_email_address(); // PidTagPrimarySmtpAddress
        else if (props.prop_exists(0x3003)) // PidTagEmailAddress
            email = props.read_prop<wstring>(0x3003);

        wstring display_name;
        if (has_prop(r, &recipient::get_name))
            display_name = r.get_name();
        // TODO: Ignore display_name if same as email.

        // TODO: Format address without display name.
        // TODO: Format address without email?

        return rfc822_quote(display_name) + L" <" + email + L">";
    }
}

void document::initialize_fields() {
    // Most of our fields will initialize themselves to sensible default
    // values, but primitive variable types won't.
    m_type = unknown;
    m_has_text = false;
}

void document::initialize_from_message(const pstsdk::message &m) {
    property_bag props(m.get_property_bag());

    set_type(document::message);

    vector<wstring> to;
    // TODO: vector<wstring> cc;
    // TODO: vector<wstring> bcc;
    if (m.get_recipient_count() > 0) {
        message::recipient_iterator i(m.recipient_begin());
        for (; i != m.recipient_end(); ++i) {
            recipient r(*i);
            wstring address(recipient_address(r));
            switch (r.get_type()) {
                case mapi_to:
                    to.push_back(address);
                    break;

                case mapi_cc:
                case mapi_bcc:
                default: // TODO: Error?
                    break;
            }
        }
    }
    if (!to.empty())
        (*this)[L"#To"] = to;

    if (has_prop(m, &message::get_subject))
        (*this)[L"#Subject"] = wstring(m.get_subject());

    if (props.prop_exists(0x007d)) // PidTagTransportMessageHeaders
        (*this)[L"#Header"] = props.read_prop<wstring>(0x007d);

    if (props.prop_exists(0x0039)) // PidTagClientSubmitTime
        (*this)[L"#DateSent"] = from_time_t(props.read_time_t_prop(0x0039));

    if (props.prop_exists(0x0e06)) // PidTagMessageDeliveryTime
        (*this)[L"#DateReceived"] = from_time_t(props.read_time_t_prop(0x0e06));

    if (m.get_attachment_count() == 0) {
        (*this)[L"#HasAttachments"] = false;
        (*this)[L"#AttachmentCount"] = size_t(0);
    } else {
        (*this)[L"#HasAttachments"] = true;
        (*this)[L"#AttachmentCount"] = size_t(m.get_attachment_count());

        wstring names;
        message::attachment_iterator i(m.attachment_begin());
        for (; i != m.attachment_end(); ++i) {
            if (!names.empty())
                names += L";";
            names += i->get_filename();
        }
        (*this)[L"#AttachmentNames"] = names;
    }

    if (props.prop_exists(0x0e07)) // PidTagMessageFlags
        (*this)[L"#ReadFlag"] =
            (props.read_prop<int32_t>(0x0e07) & 0x1) ? true : false;

    if (props.prop_exists(0x0017)) // PidTagImportance
        (*this)[L"#ImportanceFlag"] =
            (props.read_prop<int32_t>(0x0017) > 1) ? true : false;

    if (props.prop_exists(0x001a)) // PidTagMessageClass
        (*this)[L"#MessageClass"] = props.read_prop<wstring>(0x001a);

    if (props.prop_exists(0x1090)) // PidTagFlagStatus
        (*this)[L"#FlagStatus"] = props.read_prop<int32_t>(0x1090);

    if (has_prop(m, &message::get_body)) {
        m_has_text = true;
        m_text = m.get_body();
    }
}

document::document(const pstsdk::message &m) {
    initialize_fields();
    initialize_from_message(m);
}

document::document(const pstsdk::attachment &a) {
    initialize_fields();
    if (a.is_message()) {
        initialize_from_message(a.open_as_message());
    } else {
        set_type(document::file);
    
        wstring filename(a.get_filename());
        wstring extension;
        wstring::size_type dotpos(filename.rfind(L'.'));
        if (dotpos != wstring::npos)
            extension = filename.substr(dotpos + 1, wstring::npos);
 
        // Extract the native file.
        m_native = a.get_bytes();

        (*this)[L"#FileName"] = filename;
        (*this)[L"#FileExtension"] = extension;
        (*this)[L"#FileSize"] = uint64_t(native().size());
    }
}

any &document::operator[](const wstring &key) {
    return m_tags[key];
}
