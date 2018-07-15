#include "pe_handler.h"

bool PeHandler::isDll()
{
    const IMAGE_FILE_HEADER* hdr = peconv::get_file_hdr(pe_ptr, v_size);
    if (!hdr) return false;
    if (hdr->Characteristics & IMAGE_FILE_DLL) {
        return true;
    }
    return false;
}

bool PeHandler::setDll()
{
    IMAGE_FILE_HEADER* hdr = const_cast<IMAGE_FILE_HEADER*> (peconv::get_file_hdr(pe_ptr, v_size));
    if (!hdr) return false;

    hdr->Characteristics ^= IMAGE_FILE_DLL;
    return true;
}

bool PeHandler::isDllEntryStub()
{
    BYTE stub32[] = { 0x55, 0x8B, 0xEC, 0x56, 0x8B, 0x75, 0x0C, 0x57 };
    BYTE stub64[] = { 0x48, 0x89, 0x5C, 0x24, 0x08, 0x48, 0x89, 0x74, 0x24, 0x10, 0x57, 0x48, 0x83, 0xEC, 0x20, 0x49 };
    BYTE *stub = stub32;
    size_t stub_size = sizeof(stub32);
    if (is64bit) {
        stub = stub64;
        stub_size = sizeof(stub64);
    }
    
    size_t max_size = this->v_size - this->ep;
    if (max_size < stub_size) {
        return false;
    }
    BYTE *ep_va = this->pe_ptr + this->ep;
    if (memcmp(ep_va, stub, stub_size) == 0) {
        return true;
    }
    return false;
}

BYTE* PeHandler::getCavePtr(size_t neededSize)
{
    BYTE *cave = peconv::find_padding_cave(pe_ptr, v_size, neededSize, IMAGE_SCN_MEM_EXECUTE);
    if (!cave) {
        std::cout << "Cave Not found!" << std::endl;
    }
    return cave;
}

inline long long int get_jmp_delta(ULONGLONG currVA, int instrLen, ULONGLONG destVA)
{
    long long int diff = destVA - (currVA + instrLen);
    return diff;
}

bool PeHandler::dllToExePatch()
{
    BYTE back_stub32[] = {
        0x64, 0xA1, 0x30, 0x00, 0x00, 0x00, //MOV EAX,DWORD PTR FS:[0x30]
        0x8B, 0x40, 0x08, // MOV EAX,DWORD PTR DS:[EAX+0x8]
        0x6A, 0x00, // PUSH 0x0
        0x6A, 0x01, // PUSH 0x1
        0x50, // PUSH EAX
        0xE8, 0xDE, 0xAD, 0xF0, 0x0D, //CALL [ep]
        0xC3
    };

    BYTE *back_stub = back_stub32;
    size_t stub_size = sizeof(back_stub32);

    if (is64bit) {
        //TODO:
        back_stub = nullptr;
        stub_size = 0;
        return false;
    }

    size_t call_offset = stub_size - 6;

    BYTE* ptr = getCavePtr(stub_size);
    if (!ptr) {
        return false;
    }
    DWORD orig_ep = peconv::get_entry_point_rva(this->pe_ptr);
    ULONGLONG ep_va = (ULONGLONG)this->pe_ptr + orig_ep;
    ULONGLONG call_va = (ULONGLONG)ptr + call_offset;

    DWORD jump_delta = (DWORD)get_jmp_delta(call_va, 5, ep_va);
    memmove(back_stub + call_offset + 1, &jump_delta, sizeof(DWORD));
    memmove(ptr, back_stub, stub_size);
    DWORD new_ep = DWORD(ptr - this->pe_ptr);
    return peconv::update_entry_point_rva(this->pe_ptr, new_ep);
}

bool PeHandler::savePe(const char *out_path)
{
    size_t out_size = 0;
    /*in this case we need to use the original module base, because
    * the loaded PE was not relocated */
    ULONGLONG module_base = peconv::get_image_base(pe_ptr);

    BYTE* unmapped_module = peconv::pe_virtual_to_raw(pe_ptr,
        v_size,
        module_base, //the original module base
        out_size // OUT: size of the unmapped (raw) PE
    );
    bool is_ok = false;
    if (unmapped_module) {
        if (peconv::dump_to_file(out_path, unmapped_module, out_size)) {
            is_ok = true;
        }
        peconv::free_pe_buffer(unmapped_module, v_size);
    }
    return is_ok;
}
