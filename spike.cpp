#include <iostream>
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

template <typename R, typename T, typename D>
R prop_or(const T &obj, R (T::*pmf)() const, D default_value) {
    try {
        return (obj.*pmf)();
    } catch (key_not_found<prop_id> &) {
        return default_value;
    }
}

void process_property(const property_bag &bag, prop_id id) {
    wcout << L"  " << id << ": ";
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
