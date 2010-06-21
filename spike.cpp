#include <iostream>
#include <pstsdk/pst.h>

using namespace std;
using namespace pstsdk;

wstring string_to_wstring(const string &str) {
    // TODO: We need to call mbstowcs here.
    return wstring(str.begin(), str.end());
}

void process_message(const message &m) {
    try {
        wcout << m.get_subject() << endl;
    } catch (key_not_found<prop_id> &) {
        wcout << L"(No subject)" << endl;
    }
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
