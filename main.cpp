// -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*-
//
// process-pst: Convert PST files to RCF822 *.eml files and load files
// Copyright (c) 2010 Aranetic LLC
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU Affero General Public License (the "License")
// as published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but it
// is provided on an "AS-IS" basis and WITHOUT ANY WARRANTY; without even
// the implied warranties of MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NONINFRINGEMENT.  See the GNU Affero General Public License
// for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
