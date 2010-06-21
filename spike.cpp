#include <iostream>
#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <pstsdk/pst.h>

using namespace std;
using namespace std::placeholders;
using namespace pstsdk;
using namespace boost::posix_time;

wstring string_to_wstring(const string &str) {
    // TODO: We need to call mbstowcs here.
    return wstring(str.begin(), str.end());
}

struct prop_id_name_info {
    prop_id id;
    const wchar_t *name;
};

// These values can be found in [MS-OXPROPS].pdf, along with some
// documentation about each field.
prop_id_name_info prop_name_map[] = {
    { 0x0002, L"PidTagAlternateRecipientAllowed" },
    { 0x0017, L"PidTagImportance" },
    { 0x001a, L"PidTagMessageClass" },
    { 0x0023, L"PidTagOriginatorDeliveryReportRequested" },
    { 0x0026, L"PidTagPriority" },
    { 0x0029, L"PidTagReadReceiptRequested" },
    { 0x002b, L"PidTagRecipientReassignmentProhibited" },
    { 0x002e, L"PidTagOriginalSensitivity" },
    { 0x0036, L"PidTagSensitivity" },
    { 0x0037, L"PidTagSubject" },
    { 0x0039, L"PidTagClientSubmitTime" },
    { 0x003b, L"PidTagSentRepresentingSearchKey" },
    { 0x0041, L"PidTagSentRepresentingEntryId" },
    { 0x0042, L"PidTagSentRepresentingName" },
    { 0x0060, L"PidTagStartDate" },
    { 0x0061, L"PidTagEndDate" },
    { 0x0062, L"PidTagOwnerAppointmentId" },
    { 0x0063, L"PidTagResponseRequested" },
    { 0x0064, L"PidTagSentRepresentingAddressType" },
    { 0x0065, L"PidTagSentRepresentingEmailAddress" },
    { 0x0070, L"PidTagConversationTopic" },
    { 0x0071, L"PidTagConversationIndex" },
    { 0x0c17, L"PidTagReplyRequested" },
    { 0x0c19, L"PidTagSenderEntryId" },
    { 0x0c1a, L"PidTagSenderName" },
    { 0x0c1d, L"PidTagSenderSearchKey" },
    { 0x0c1e, L"PidTagSenderAddressType" },
    { 0x0c1f, L"PidTagSenderEmailAddress" },
    { 0x0e01, L"PidTagDeleteAfterSubmit" },
    { 0x0e03, L"PidTagDisplayCc" },
    { 0x0e04, L"PidTagDisplayTo" },
    { 0x0e06, L"PidTagMessageDeliveryTime" },
    { 0x0e07, L"PidTagMessageFlags" },
    { 0x0e08, L"PidTagMessageSize" },
    { 0x0e1f, L"PidTagRtfInSync" },
    { 0x0e23, L"PidTagInternetArticleNumber" },
    { 0x0e27, L"PidTagSecurityDescriptor" },
    { 0x0e79, L"PidTagTrustSender" },
    { 0x1000, L"PidTagBody" },
    { 0x1009, L"PidTagRtfCompressed" },
    { 0x1013, L"PidTagBodyHtml" },
    { 0x1035, L"PidTagInternetMessageId" },
    { 0x1080, L"PidTagIconIndex" },
    { 0x10f4, L"PidTagAttributeHidden" },
    { 0x10f5, L"PidTagAttributeSystem" },
    { 0x10f6, L"PidTagAttributeReadOnly" },
    { 0x3007, L"PidTagCreationTime" },
    { 0x3008, L"PidTagLastModificationTime" },
    { 0x300b, L"PidTagSearchKey" },
    { 0x3fde, L"PidTagInternetCodepage" },
    { 0x3ff1, L"PidTagMessageLocaleId" },
    { 0x3ff8, L"PidTagCreatorName" },
    { 0x3ff9, L"PidTagCreatorEntryId" },
    { 0x3ffa, L"PidTagLastModifierName" },
    { 0x3ffb, L"PidTagLastModifierEntryId" },
    { 0x3ffd, L"PidTagMessageCodepage" },
    { 0x4019, L"PidTagSenderFlags" },
    { 0x401a, L"PidTagSentRepresentingFlags" },
    // I can't find these in the docs.
    //{ 0x405d, L"" },
    //{ 0x6540, L"" },
    //{ 0x6610, L"" },
    { 0, NULL }
};

wstring property_name(prop_id id) {
    if (id >= 0x8000) {
        return L"(named property)";
    } else {
        // Look this up in our map of known names.
        for (prop_id_name_info *i = prop_name_map; i->name != NULL; i++) {
            if (i->id == id)
                return i->name;
        }
        
        ostringstream out;
        out << "0x" << hex << setw(4) << setfill('0') << id;
        return string_to_wstring(out.str());
    }
}

template <typename R, typename T, typename D>
R prop_or(const T &obj, R (T::*pmf)() const, D default_value) {
    try {
        return (obj.*pmf)();
    } catch (key_not_found<prop_id> &) {
        return default_value;
    }
}

void process_property(const property_bag &bag, prop_id id) {
    wcout << L"  " << property_name(id) << ": ";
    prop_type type(bag.get_prop_type(id));
    switch (type) {
        case prop_type_null:
            wcout << "null";
            break;

        case prop_type_short:
            wcout << bag.read_prop<boost::int16_t>(id);
            break;

        case prop_type_long:
            wcout << bag.read_prop<boost::int32_t>(id);
            break;

        case prop_type_float:
            wcout << bag.read_prop<float>(id);
            break;

        case prop_type_double:
            wcout << bag.read_prop<double>(id);
            break;

        case prop_type_boolean:
            wcout << (bag.read_prop<bool>(id) ? L"true" : L"false");
            break;

        case prop_type_longlong:
            wcout << bag.read_prop<boost::int64_t>(id);
            break;

        case prop_type_string:
        case prop_type_wstring:
            wcout << bag.read_prop<wstring>(id);
            break;

        case prop_type_apptime:
        case prop_type_systime:
            wcout << from_time_t(bag.read_time_t_prop(id));
            break;

        case prop_type_binary:
            wcout << L"(binary data)";
            break;

        default:
            wcout << L"(Unsupported value of type " << type << L")";
            break;
    }
    wcout << endl;
}

void process_property_bag(const property_bag &bag) {
    std::vector<prop_id> ids(bag.get_prop_list());
    for_each(ids.begin(), ids.end(), bind(process_property, bag, _1));
}

void process_recipient(const recipient &r) {
    wcout << "  ";
    switch (prop_or(r, &recipient::get_type, recipient_type(0))) {
        case mapi_to: wcout << "To"; break;
        case mapi_cc: wcout << "CC"; break;
        case mapi_bcc: wcout << "BCC"; break;
        default: wcout << "(Unknown type)"; break;
    }
    wcout << ": " << prop_or(r, &recipient::get_name, L"(anonymous)") << " <"
          << prop_or(r, &recipient::get_email_address, L"(no email)")
          << ">" << endl;
}

void process_attachment(const attachment &a) {
    wcout << "  Attachment: "
          << prop_or(a, &attachment::get_filename, L"(no filename)") << " ("
          << prop_or(a, &attachment::size, 31337) << " bytes)"
          << (a.is_message() ? " SUBMESSAGE" : "")
          << endl;
}

void process_message(const message &m) {
    wcout << prop_or(m, &message::get_subject, L"(null)") << endl;

    if (m.get_recipient_count() > 0)
        for_each(m.recipient_begin(), m.recipient_end(), process_recipient);
    if (m.get_attachment_count() > 0)
        for_each(m.attachment_begin(), m.attachment_end(), process_attachment);

    process_property_bag(m.get_property_bag());
}

void process_folder(const folder &f) {
    // This slick little approach is adapted from the pstsdk sample code.
    for_each(f.message_begin(), f.message_end(), process_message);
    for_each(f.sub_folder_begin(), f.sub_folder_end(), process_folder);
}

int main(int argc, char **argv) {
    // Parse our command-line arguments.
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " input.pst" << endl;
        exit(1);
    }
    string pst_path(argv[1]);

    // Open our pst.
    pst pst_db(string_to_wstring(pst_path));
    process_folder(pst_db.open_root_folder());
}
