#include <iostream>
#include <pstsdk/pst.h>
#include <boost/filesystem.hpp>

#include "utilities.h"

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
    try {
        pst pst_db(string_to_wstring(pst_path));
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
    ofstream file(loadfile_path.string());
    file << "<?xml version='1.0' encoding='UTF-8'?>" << endl
         << "<Root DataInterchangeType='Update'>" << endl
         << "  <Batch>" << endl
         << "    <Documents>" << endl
         << "    </Documents>" << endl
         << "  </Batch>" << endl
         << "</Root>" << endl;
    file.close();

    return 0;
}
