#include <stdio.h>
#include <windows.h>

#include "peconv.h"
#include "pe_handler.h"

int main(int argc, char *argv[])
{
    if (argc < 3) {
        std::cout << "args: <input exe> <output_file>" << std::endl;
        system("pause");
        return 0;
    }
    char *filename = argv[1];
    char *outfile = argv[2];

    PeHandler hndl(filename);
    if (!hndl.isDll()) {
        std::cout << "It is not a DLL!" << std::endl;
        return -1;
    }
    if (hndl.is64()) {
        std::cout << "64-bit support not implemented yet!" << std::endl;
        return -1;
    }
    if (!hndl.isDllEntryStub()) {
        std::cout << "Unrecognized DLL stub!" << std::endl;
        return -1;
    }
    hndl.setDll();
    if (hndl.dllToExePatch()) {
        std::cout << "[OK] Converted successfuly."<< std::endl;
    }
    if (hndl.savePe(outfile)) {
        std::cout << "[OK] Module dumped to:" << outfile << std::endl;
    }
    return 0;
}
