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
    { 0x003f, L"PidTagReceivedByEntryId" },
    { 0x0040, L"PidTagReceivedByName" },
    { 0x0041, L"PidTagSentRepresentingEntryId" },
    { 0x0042, L"PidTagSentRepresentingName" },
    { 0x0043, L"PidTagReceivedRepresentingEntryId" },
    { 0x0044, L"PidTagReceivedRepresentingName" },
    { 0x0047, L"PidTagMessageSubmissionId" },
    { 0x0051, L"PidTagReceivedBySearchKey" },
    { 0x0052, L"PidTagReceivedRepresentingSearchKey" },
    { 0x0057, L"PidTagMessageToMe" },
    { 0x0058, L"PidTagMessageCcMe" },
    { 0x0060, L"PidTagStartDate" },
    { 0x0061, L"PidTagEndDate" },
    { 0x0062, L"PidTagOwnerAppointmentId" },
    { 0x0063, L"PidTagResponseRequested" },
    { 0x0064, L"PidTagSentRepresentingAddressType" },
    { 0x0065, L"PidTagSentRepresentingEmailAddress" },
    { 0x0070, L"PidTagConversationTopic" },
    { 0x0071, L"PidTagConversationIndex" },
    { 0x0075, L"PidTagReceivedByAddressType" },
    { 0x0076, L"PidTagReceivedByEmailAddress" },
    { 0x0077, L"PidTagReceivedRepresentingAddressType" },
    { 0x0078, L"PidTagReceivedRepresentingEmailAddress" },
    { 0x007d, L"PidTagTransportMessageHeaders" },
    { 0x007f, L"PidTagTnefCorrelationKey" },
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
    { 0x0e2b, L"PidTagToDoItemFlags" },
    //{ 0x0e2f, L"" },
    { 0x0e79, L"PidTagTrustSender" },
    { 0x1000, L"PidTagBody" },
    { 0x1009, L"PidTagRtfCompressed" },
    { 0x1013, L"PidTagBodyHtml" },
    { 0x1035, L"PidTagInternetMessageId" },
    { 0x1080, L"PidTagIconIndex" },
    { 0x1090, L"PidTagFlagStatus" },
    // Prop 0x1095 allegedly "specifies the flag color of the Message object."
    { 0x1095, L"PidTagFollowupIcon" },
    { 0x10f4, L"PidTagAttributeHidden" },
    { 0x10f5, L"PidTagAttributeSystem" },
    { 0x10f6, L"PidTagAttributeReadOnly" },
    { 0x3007, L"PidTagCreationTime" },
    { 0x3008, L"PidTagLastModificationTime" },
    { 0x300b, L"PidTagSearchKey" },
    { 0x3010, L"PidTagTargetEntryId" },
    //{ 0x3014, L"" },
    //{ 0x3015, L"" },
    { 0x3016, L"PidTagConversationIndexTracking" },
    { 0x3a40, L"PidTagSendRichInfo" },
    { 0x3fde, L"PidTagInternetCodepage" },
    { 0x3fea, L"PidTagHasDeferredActionMessages" },
    { 0x3ff1, L"PidTagMessageLocaleId" },
    { 0x3ff8, L"PidTagCreatorName" },
    { 0x3ff9, L"PidTagCreatorEntryId" },
    { 0x3ffa, L"PidTagLastModifierName" },
    { 0x3ffb, L"PidTagLastModifierEntryId" },
    { 0x3ffd, L"PidTagMessageCodepage" },
    { 0x4019, L"PidTagSenderFlags" },
    { 0x401a, L"PidTagSentRepresentingFlags" },
    { 0x401b, L"PidTagReceivedByFlags" },
    { 0x401c, L"PidTagReceivedRepresentingFlags" },
    //{ 0x405d, L"" },
    { 0x5902, L"PidTagInternetMailOverrideFormat" },
    { 0x5909, L"PidTagMessageEditorFormat" },
    { 0x5d01, L"PidTagSenderSmtpAddress" },
    //{ 0x5d02, L"" },
    { 0x5fe5, L"PidTagSessionInitiationProtocolUri" },
    //{ 0x6540, L"" },
    { 0x65e2, L"PidTagChangeKey" },
    { 0x65e3, L"PidTagPredecessorChangeList" },
    //{ 0x6610, L"" },
    //{ 0x6619, L"" },  // Not PidTagUserEntryId, I don't think.
    { 0, NULL }
};

const guid ps_common = { 0x62008, 0, 0, { 0xc0, 0, 0, 0, 0, 0, 0, 0x46 } };
const guid ps_internet_headers = { 0x20386, 0, 0, { 0xc0, 0, 0, 0, 0, 0, 0, 0x46 } };
const guid ps_task = { 0x62003, 0, 0, { 0xc0, 0, 0, 0, 0, 0, 0, 0x46 } };
const guid ps_messaging = { 0x41f28f13, static_cast<ushort>(0x83f4), 0x4114, { 0xa5, 0x84, 0xee, 0xdb, 0x5a, 0x6b, 0x0b, 0xff } };
const guid ps_appointment = { 0x62002, 0, 0, { 0xc0, 0, 0, 0, 0, 0, 0, 0x46 } };
const guid ps_meeting = { 0x6ed8da90, 0x450b, 0x101b, { 0x98, 0xda, 0, 0xaa, 0, 0x3f, 0x13, 0x05 } };

bool operator==(const guid g1, const guid g2) {
    return (g1.data1 == g2.data1 &&
            g1.data2 == g2.data2 &&
            g1.data3 == g2.data3 &&
            memcmp(g1.data4, g2.data4, 8) == 0);
}

wstring guid_name(guid g) {
    if (g == ps_none) {
        return L"ps_none";
    } else if (g == ps_mapi) {
        return L"ps_mapi";
    } else if (g == ps_public_strings) {
        return L"ps_public_strings";
    } else if (g == ps_common) {
        return L"ps_common";
    } else if (g == ps_internet_headers) {
        return L"ps_internet_headers";
    } else if (g == ps_task) {
        return L"ps_task";
    } else if (g == ps_messaging) {
        return L"ps_messaging";
    } else if (g == ps_appointment) {
        return L"ps_appointment";
    } else if (g == ps_meeting) {
        return L"ps_meeting";
    } else {
        wostringstream out;
        out << hex
            << setw(8) << setfill(L'0') << g.data1 << L"-"
            << setw(4) << setfill(L'0') << g.data2 << L"-"
            << setw(4) << setfill(L'0') << g.data3 << L"-";
        for (size_t i = 0; i < 8; i++)
            out << setw(2) << setfill(L'0') << g.data4[i];
        return out.str();
    }
}

shared_db_ptr g_db;

wstring property_name(prop_id id) {
    if (id >= 0x8000) {
        name_id_map prop_names(g_db);
        if (prop_names.prop_id_exists(id)) {
            named_prop prop(prop_names.lookup(id));
            wostringstream out;
            out << guid_name(prop.get_guid()) << L" ";
            if (prop.is_string())
                out << prop.get_name();
            else
                out << L"0x" << hex << setw(4) << setfill(L'0')
                    << prop.get_id();
            return out.str();
        }
    } else {
        // Look this up in our map of known names.
        for (prop_id_name_info *i = prop_name_map; i->name != NULL; i++)
            if (i->id == id)
                return i->name;
    }
        
    ostringstream out;
    out << "0x" << hex << setw(4) << setfill('0') << id;
    return string_to_wstring(out.str());
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
    g_db = pst_db.get_db();
    process_folder(pst_db.open_root_folder());
}
