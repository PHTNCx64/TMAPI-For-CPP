/* C++ Wrapper around the Target Manager API. Written by Starman aka PHTNC. */
/* This project is still in development. Modification and redistribution is allowed. */

#include "tmapi.hpp"

bool TMAPI::InitLibrary(std::string libpath)
{
	// Loads the DLL into the process memory.
    hlib = LoadLibraryA(libpath.c_str());

    // If the library was not loaded, we return false.
    if(hlib)
    {
        // GetProcAddress returns the address of the function from the DLL.
        
        // Alternative way of loading libraries in Windows, works and also is more readable for me
        // It's also type-safe..
        SNPS3InitTargetComms = (long (__cdecl*)(void))GetProcAddress(hlib, "SNPS3InitTargetComms");
        SNPS3CloseTargetComms = (long (__cdecl*)(void))GetProcAddress(hlib, "SNPS3CloseTargetComms");
        SNPS3Connect = (long (__cdecl*)(int, const char*))GetProcAddress(hlib, "SNPS3Connect");
        SNPS3PowerOff = (long (__cdecl*)(int, u32))GetProcAddress(hlib, "SNPS3PowerOff");
        SNPS3Disconnect = (long (__cdecl*)(int))GetProcAddress(hlib, "SNPS3Disconnect");
        SNPS3PowerOn = (long (__cdecl*)(int))GetProcAddress(hlib, "SNPS3PowerOn");
        SNPS3ProcessGetMemory = (long (__cdecl*)(int, u32, u32, u64, u64, int, u8*))GetProcAddress(hlib, "SNPS3ProcessGetMemory");
        SNPS3ProcessSetMemory = (long (__cdecl*)(int, u32, u32, u64, u64, int, const u8*))GetProcAddress(hlib, "SNPS3ProcessSetMemory");
        SNPS3Reset = (long (__cdecl*)(int, u64))GetProcAddress(hlib, "SNPS3Reset");
        SNPS3ProcessList = (long (__cdecl*)(int, u32*, u32*))GetProcAddress(hlib, "SNPS3ProcessList");

        // Let's make sure that all functions have been loaded successfully.
        if(SUCCEEDED(SNPS3InitTargetComms()))
        {
            return SNPS3InitTargetComms != NULL
            && SNPS3CloseTargetComms    != NULL
            && SNPS3Connect             != NULL
            && SNPS3Disconnect          != NULL
            && SNPS3PowerOn             != NULL
            && SNPS3PowerOff            != NULL
            && SNPS3ProcessSetMemory    != NULL
            && SNPS3ProcessGetMemory    != NULL
            && SNPS3Reset               != NULL
            && SNPS3ProcessList         != NULL;
        }
        return false;
    }
    return false;
}

bool TMAPI::ConnectTarget(int target)
{
    // The 2nd parameter is const char*, only NULL (0) is the known valid value for this function.
    if(SUCCEEDED(SNPS3Connect(target, NULL)))
    {
        TARGET = target;
        return true;
    }
    return false;
}


// Please do not replace new and delete[] with malloc and free. C functions should not be used in this library,
// except you rewrite this library for the C programming languange.
std::vector<TMAPI::ProcessInfo> TMAPI::GetProcessList(void) const
{
    std::vector<TMAPI::ProcessInfo> proc;

    u32 Count;
    u32* Buffer;
    // Let's get the amount of processes first.
    if(SUCCEEDED(SNPS3ProcessList(TARGET, &Count, 0)))
    {
        if(Count > 0)
        {

            // Allocating as much space for the buffer, as much processes exists.
            // Before we reserve anything for the vector, we need to allocate some memory in the buffer, so the DLL writes the process IDs in the buffer
            Buffer = new u32[Count];

            // A check to make sure, that the buffer was actually allocated.
            if(Buffer == nullptr)
                return proc;

            proc.reserve(Count);
            // Getting the process list and writing the process IDs into the buffer
            if(SUCCEEDED(SNPS3ProcessList(TARGET, &Count, Buffer)))
            {
                    // Writing process ids values into the process ID values in the process info structure.
                    for(size_t i = 0; i < Count; i++)
                    {
                        ProcessInfo info = {Buffer[i]};
                        proc.push_back(info);
                    }
                    // deleting the array, to prevent memory leaking.
                    delete[] Buffer;
                    return proc;
            }
            // deleting the array, to prevent memory leaking
            delete[] Buffer;
            return proc;
        }
        // Nothing was manually allocated yet, so there's no need to delete the array.
        return proc;
    }
    // Nothing was manually allocated yet, so there's no need to delete the array,
    return proc;
}

long TMAPI::DisconnectTarget(void) const
{
    // It's important to always return the result.
    long Dscnnct = SNPS3Disconnect(TARGET);

    if(SUCCEEDED(Dscnnct))
    {
        TARGET = -1;
        return Dscnnct;
    }
    return Dscnnct;
}

bool TMAPI::AttachProcess(u32 processID)
{
    // We need to make sure that the specified process is valid, so we can 
    std::vector<TMAPI::ProcessInfo> proc = GetProcessList();

    if(proc.size() > 0)
    {
        for(size_t i = 0; i < proc.size(); i++)
        {
            if(proc.at(i).ProcessID == processID)
            {
                cProcID = proc.at(i).ProcessID;
				return true;
            }
        }
        return false;
    }
    return false;
}

// Every value above 0 is a forced shutdown. Conversion from bool to u32 happens implicitly
long TMAPI::PowerOff(bool force) const
{
    return SNPS3PowerOff(TARGET, force);
}

// Conversion from u32 to int happens implicitly
long TMAPI::WriteProcessMemory(u64 address, size_t size, const u8* data) const
{
    return SNPS3ProcessSetMemory(TARGET, 0, cProcID, 0, address, size, data);
}

// Conversion from u32 to int happens implicitly
long TMAPI::WriteProcessMemory(u32 processID, u64 address,size_t size, const u8* data) const
{
    return SNPS3ProcessSetMemory(TARGET, 0, processID, 0, address, size, data);
}

// Conversion from u32 to int happens implicitly
long TMAPI::ReadProcessMemory(u64 address, size_t size, u8* data)
{
    return SNPS3ProcessGetMemory(TARGET, 0, cProcID, 0, address, size, data);
}