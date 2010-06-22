#include <iostream>

using namespace std;

int main(int argc, char **argv) {
    if (argc != 2) {
        wcout << L"Usage: process-pst input.pst output-dir" << endl;
        exit(1);
    }
}
