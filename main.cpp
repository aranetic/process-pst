#include <iostream>
#include <pstsdk/pst.h>

#include "utilities.h"

using namespace std;
using namespace pstsdk;

int main(int argc, char **argv) {
    // Parse our command-line arguments.
    if (argc != 3) {
        wcout << L"Usage: process-pst input.pst output-dir" << endl;
        exit(1);
    }
    string pst_path(argv[1]);
    string output_directory_path(argv[2]);

    // Open our PST.
    try {
        pst pst_db(string_to_wstring(pst_path));
    } catch (exception &e) {
        wcerr << L"Could not open PST: " << string_to_wstring(e.what());
        exit(1);
    }

    return 0;
}
