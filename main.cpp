#include <iostream>
#include <pstsdk/pst.h>

#include "utilities.h"
#include "edrm.h"

using namespace std;
using namespace pstsdk;
using namespace boost::filesystem;

int main(int argc, char **argv) {
    // Parse our command-line arguments.
    if (argc != 3) {
        wcout << L"Usage: process-pst input.pst output-dir" << endl;
        exit(1);
    }
    string pst_path(argv[1]);
    path output_directory_path(argv[2]);

    // Open our PST.
    shared_ptr<pst> pst_file;
    try {
        pst_file.reset(new pst(string_to_wstring(pst_path)));
    } catch (exception &e) {
        wcerr << L"Could not open PST: " << string_to_wstring(e.what()) << endl;
        exit(1);
    }

    // Refuse to run if our output directory exists.
    if (exists(output_directory_path)) {
        wcerr << L"Will not overwrite existing "
              << string_to_wstring(output_directory_path.string()) << endl;
        exit(1);
    }

    // Create an empty loadfile.  We'll fill this in shortly.
    create_directory(output_directory_path);
    path loadfile_path(output_directory_path / "edrm-loadfile.xml");
    ofstream loadfile(loadfile_path.string());
    convert_to_edrm(pst_file, loadfile, output_directory_path);
    loadfile.close();

    return 0;
}
