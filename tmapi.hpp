/* C++ Wrapper around the Target Manager API. Written by Starman aka PHTNC. */
/* This project is still in development. Modification and redistribution is allowed. */

#ifndef _TARGET_MANAGER_API_
#define _TARGET_MANAGER_API_

#define FUNC_OK     0
#define FUNC_ERROR -1

// Use this, if you want to make sure, that the command was executed successfully
#define RETURN_OK(function)(return function >= FUNC_OK)
#define RETURN_ERROR(function)(return function < FUNC_OK)

#define SUCCEEDED(function)(function >= FUNC_OK)

#include <libloaderapi.h>
#include <vector>
#include <string>

#include "tmapi_types.hpp"

class TMAPI
{
public:
    struct ProcessInfo
    {
        // Returns the Process ID of a SELF process, that is running on the console (SPRX modules are not returned!)
        // SPRX modules are very similar to DLLs.
        u32 ProcessID;
    };

    // Initializes the Target Manager API and loads all needed functions into the process memory.
    // Support for the 64 bit version will be implemented at a later date.
    bool InitLibrary(std::string);
    // Connects to the specified target. In most cases, it's 0.
    bool ConnectTarget(int);
    // Attaches to the specified process.
    bool AttachProcess(u32);
    // Attaches to the current running process.
    bool AttachProcess(void);
    // Disconnects from the current target
    long DisconnectTarget(void) const;
    // Writes bytes into the specified address. Writes only in the current attached process.
    long WriteProcessMemory(u64, size_t, const u8*) const;
    // Writes bytes into the specified address. Process can be specified.
    long WriteProcessMemory(u32, u64, size_t, const u8*) const;
    // Reads data from the memory and passes the data in an unsigned char array.
    long ReadProcessMemory(u64, size_t, u8*);
    long ReadProcessMemory(u32, u64, size_t, u8*);
    // Powers the target off, true to force the shutdown.
    long PowerOff(bool) const;
    // Powers on the specified target.
    long PowerOn(int) const;
    // Reboots the target, however, Sony prefers to call it resetting.
    long ResetTarget(u64) const;
    // Retrieves all running SELF processes and returns the values into a vector.
    std::vector<TMAPI::ProcessInfo> GetProcessList(void) const;
    
protected:
    // Current Process ID.
    u32 cProcID;

private:
    // Target value, it's the only mutable var.
    int TARGET;
    // Library instance
    HINSTANCE hlib;

    // TMAPI functions are put with __cdecl on the stack.
    // The parameters were taken from the official TMAPI document.
    long (__cdecl* SNPS3Connect)(int, const char*);
    long (__cdecl* SNPS3InitTargetComms)(void);
    long (__cdecl* SNPS3Disconnect)(int);
    long (__cdecl* SNPS3CloseTargetComms)(void);
    long (__cdecl* SNPS3ProcessList)(int, u32*, u32*);
    long (__cdecl* SNPS3ProcessSetMemory)(int, u32, u32, u64, u64, int, const u8*);
    long (__cdecl* SNPS3ProcessGetMemory)(int, u32, u32, u64, u64, int, u8*);
    long (__cdecl* SNPS3PowerOff)(int, u32);
    long (__cdecl* SNPS3PowerOn)(int);
    long (__cdecl* SNPS3Reset)(int, u64);
};



#endif // _TARGET_MANAGER_API_