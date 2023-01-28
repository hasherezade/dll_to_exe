#include <stdio.h>
#include <windows.h>

#include <peconv.h>
#include "pe_handler.h"

#define VERSION "1.1"


bool create_new_process(IN LPSTR path, OUT PROCESS_INFORMATION& pi)
{
    STARTUPINFO si;
    memset(&si, 0, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);

    memset(&pi, 0, sizeof(PROCESS_INFORMATION));

    if (!CreateProcessA(
        NULL,
        path,
        NULL, //lpProcessAttributes
        NULL, //lpThreadAttributes
        FALSE, //bInheritHandles
        0, //dwCreationFlags
        NULL, //lpEnvironment 
        NULL, //lpCurrentDirectory
        &si, //lpStartupInfo
        &pi //lpProcessInformation
    ))
    {
        std::cout << "[ERROR] CreateProcess failed, Error = " << std::hex << GetLastError() << std::endl;
        return false;
    }
    std::cout << "[OK] Process: " << std::dec << pi.dwProcessId << " created from the converted file:" << std::endl;
    return true;
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        std::cout << "DLL to EXE converter v"<< VERSION << " \n- for 32 & 64 bit DLLs -" << std::endl;
        std::cout << "args: <input_dll> <output_exe> [test_run?]" << std::endl;
        system("pause");
        return 0;
    }
    char *filename = argv[1];
    char *outfile = argv[2];
    bool test_run = false;
    if (argc > 3 && argv[3][0] == '1') {
        test_run = true;
        std::cout << "Test run enabled!\n";
    }

    PeHandler hndl(filename);
    if (!hndl.isDll()) {
        std::cout << "It is not a DLL!" << std::endl;
        return -1;
    }
    hndl.setDll();
    hndl.clearGuardFlag();
    if (hndl.dllToExePatch()) {
        std::cout << "[OK] Converted successfuly."<< std::endl;
    } else {
        std::cout << "Could not convert!" << std::endl;
        return -1;
    }
    if (hndl.savePe(outfile)) {
        std::cout << "[OK] Module dumped to: " << outfile << std::endl;
        if (test_run) {
            PROCESS_INFORMATION pi = { 0 };
            if (create_new_process(outfile, pi) && pi.hThread) {
                WaitForSingleObject(pi.hThread, INFINITE);
                DWORD exitCode = 0;
                GetExitCodeProcess(pi.hProcess, &exitCode);
                std::cout << "[OK] Process finished with Exit Code: " << exitCode << "\n";
            } else {
                return -1;
            }
        }
    }
    return 0;
}
