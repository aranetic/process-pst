#include <iostream>
#include <pstsdk/pst.h>

using namespace std;
using namespace pstsdk;

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
          << endl;
}

void process_message(const message &m) {
    wcout << prop_or(m, &message::get_subject, L"(null)") << endl;

    if (m.get_recipient_count() > 0)
        for_each(m.recipient_begin(), m.recipient_end(), process_recipient);
    if (m.get_attachment_count() > 0)
        for_each(m.attachment_begin(), m.attachment_end(), process_attachment);
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
