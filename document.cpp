#include <cstdint>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>
#include <pstsdk/pst.h>

#include "utilities.h"
#include "document.h"

using namespace std;
using boost::any;
using boost::lexical_cast;
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

    // Do out best to extract something resembling an RFC822 email
    // address.
    wstring extract_address(const const_property_object *props,
                            prop_id name_prop, prop_id smtp_address_prop,
                            prop_id email_address_prop) {
        wstring display_name;
        if (props->prop_exists(name_prop))
            display_name = props->read_prop<wstring>(name_prop);

        // Email addresses may be stored as either an SMTP address or a
        // generic email address.  The latter may or may not be SMTP.
        wstring email;
        if (props->prop_exists(smtp_address_prop))
            email = props->read_prop<wstring>(smtp_address_prop);
        else if (props->prop_exists(email_address_prop))
            email = props->read_prop<wstring>(email_address_prop);

        return rfc822_email(display_name, email);
    }

    wstring recipient_address(const recipient &r) {
        const_table_row props(r.get_property_row());
        // PidTagDisplayName, PidTagPrimarySmtpAddress, PidTagEmailAddress.
        return extract_address(&props, 0x3001, 0x39fe, 0x3003);
    }

    wstring attachment_name(const attachment &a) {
        if (a.is_message()) {
            message m(a.open_as_message());
            if (has_prop(m, &message::get_subject))
                return m.get_subject();
        } else {
            if (has_prop(a, &attachment::get_filename))
                return a.get_filename();
        }
        return L"(no name)";
    }
}

void document::initialize_fields() {
    // Most of our fields will initialize themselves to sensible default
    // values, but primitive variable types won't.
    m_type = unknown;
    m_has_text = false;
    m_has_native = false;
}

void document::initialize_from_message(const pstsdk::message &m) {
    property_bag props(m.get_property_bag());

    set_type(document::message);

    vector<wstring> from;
    // Maybe we should use these as 'From', and SentRepresenting as 'Sender'?
    // PidTagSentRepresentingName, , PidTagSentRepresentingEmailAddress
    // 0x0042, ???, 0x0065
    // PidTagSenderName PidTagSenderSmtpAddress PidTagSenderEmailAddress
    if (props.prop_exists(0x0c1a))
        from.push_back(extract_address(&props, 0x0c1a, 0x5d01, 0x0c1f));
    if (!from.empty())
        (*this)[L"#From"] = from;

    vector<wstring> to;
    vector<wstring> cc;
    vector<wstring> bcc;
    if (m.get_recipient_count() > 0) {
        message::recipient_iterator i(m.recipient_begin());
        for (; i != m.recipient_end(); ++i) {
            recipient r(*i);
            wstring address(recipient_address(r));
            switch (r.get_type()) {
                case mapi_to:  to.push_back(address);  break;
                case mapi_cc:  cc.push_back(address);  break;
                case mapi_bcc: bcc.push_back(address); break;
                default:
                    throw runtime_error("Unknown recipient type");
                    break;
            }
        }
    }
    if (!to.empty())
        (*this)[L"#To"] = to;
    if (!cc.empty())
        (*this)[L"#CC"] = cc;
    if (!bcc.empty())
        (*this)[L"#BCC"] = bcc;

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
        (*this)[L"#AttachmentCount"] = int32_t(0);
    } else {
        (*this)[L"#HasAttachments"] = true;
        (*this)[L"#AttachmentCount"] = int32_t(m.get_attachment_count());

        vector<wstring> names;
        message::attachment_iterator i(m.attachment_begin());
        for (; i != m.attachment_end(); ++i) {
            names.push_back(attachment_name(*i));
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
        (*this)[L"#FlagStatus"] =
            lexical_cast<wstring>(props.read_prop<int32_t>(0x1090));

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
        m_has_native = true;
        m_native = a.get_bytes();

        (*this)[L"#FileName"] = filename;
        (*this)[L"#FileExtension"] = extension;
        (*this)[L"#FileSize"] = int64_t(native().size());
    }
}

wstring document::type_string() const {
    switch (type()) {
        case message: return L"Message";
        case file: return L"File";
        default:
            throw runtime_error("Cannot convert document type to string");
    }
}

any &document::operator[](const wstring &key) {
    return m_tags[key];
}
