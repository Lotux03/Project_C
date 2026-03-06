// Microsoft
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tlhelp32.h>

// Local
#include "example_main.h"
#include "vector_lib.h"

// === Debug event codes ===
//EXCEPTION_DEBUG_EVENT = 1
//CREATE_THREAD_DEBUG_EVENT = 2
//CREATE_PROCESS_DEBUG_EVENT = 3
//EXIT_THREAD_DEBUG_EVENT = 4
//EXIT_PROCESS_DEBUG_EVENT = 5
//LOAD_DLL_DEBUG_EVENT = 6
//UNLOAD_DLL_DEBUG_EVENT = 7
//OUTPUT_DEBUG_STRING_EVENT = 8
//RIP_EVENT = 9


typedef struct {
	int id;
	float value;
} MyStruct;

// Examples
int create_suspended_process(const char* process_name) {
    STARTUPINFOA startup = { 0 };
    PROCESS_INFORMATION proc = { 0 };

    startup.cb = sizeof(startup);
    if (!CreateProcessA(
        NULL,
        (LPSTR)process_name,
        NULL,
        NULL,
        FALSE,
        CREATE_SUSPENDED,
        NULL,
        NULL,
        &startup,
        &proc))
    {
        printf("CreateProcess failed\n");
        return 0;
    }
    printf("Process created!\n");
    printf("PID: %lu\n", proc.dwProcessId);
    printf("Handle: %p\n", proc.hProcess);

    return 1;
}
int silly_debugger() {
    DEBUG_EVENT debug_event;

    while (1)
    {
        WaitForDebugEvent(&debug_event, INFINITE);

        switch (debug_event.dwDebugEventCode)
        {
        case EXCEPTION_DEBUG_EVENT:
            printf("EXCEPTION_DEBUG_EVENT\n");
            break;

        case CREATE_PROCESS_DEBUG_EVENT:
            printf("CREATE_PROCESS_DEBUG_EVENT\n");
            break;

        case LOAD_DLL_DEBUG_EVENT:
            printf("LOAD_DLL_DEBUG_EVENT\n");
            break;

        case EXIT_PROCESS_DEBUG_EVENT:
            printf("EXIT_PROCESS_DEBUG_EVENT\n");
            return 0;

        default:
            printf("Other debug event: %lu\n", debug_event.dwDebugEventCode);
        };

        ContinueDebugEvent(
            debug_event.dwProcessId,
            debug_event.dwThreadId,
            DBG_CONTINUE
        );
    }
}

// Functions
void print_loaded_dll(HANDLE hProcess, LPVOID baseAddress, LPVOID namePtr)
{
    if (!baseAddress || !namePtr) {
        printf("LOAD_DLL_DEBUG_EVENT with NULL address\n");
        return;
    }

    WCHAR dllName[MAX_PATH] = { 0 };
    LPVOID dllNamePtr = NULL;

    // namePtr is a pointer to a pointer; read the actual string pointer first
    if (!ReadProcessMemory(hProcess, namePtr, &dllNamePtr, sizeof(dllNamePtr), NULL)) {
        printf("Loaded DLL at %p (name pointer read failed)\n", baseAddress);
        return;
    }

    // If the string pointer is valid, read the actual DLL name
    if (dllNamePtr && ReadProcessMemory(hProcess, dllNamePtr, dllName, sizeof(dllName) - 2, NULL)) {
        wprintf(L"Loaded DLL: %s at %p\n", dllName, baseAddress);
    }
    else {
        printf("Loaded DLL at %p (name read failed)\n", baseAddress);
    }
}
DWORD get_entry_point(HANDLE hProcess, LPVOID baseAddress)
{
    IMAGE_DOS_HEADER dosHeader;
    IMAGE_NT_HEADERS ntHeaders;

    // Get main module base if baseAddress is NULL
    if (!baseAddress) {
        DWORD pid = GetProcessId(hProcess);
        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
        if (snap == INVALID_HANDLE_VALUE) return 0;

        MODULEENTRY32 me = { 0 };
        me.dwSize = sizeof(me);
        if (!Module32First(snap, &me)) {
            CloseHandle(snap);
            return 0;
        }
        baseAddress = me.modBaseAddr;
        CloseHandle(snap);
    }

    // Read DOS header
    if (!ReadProcessMemory(hProcess, baseAddress, &dosHeader, sizeof(dosHeader), NULL)) return 0;
    if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) return 0;

    // Read NT headers
    LPVOID ntHeaderAddr = (LPBYTE)baseAddress + dosHeader.e_lfanew;
    if (!ReadProcessMemory(hProcess, ntHeaderAddr, &ntHeaders, sizeof(ntHeaders), NULL)) return 0;
    if (ntHeaders.Signature != IMAGE_NT_SIGNATURE) return 0;

    DWORD entryRVA = ntHeaders.OptionalHeader.AddressOfEntryPoint;
    DWORD entryVA = (DWORD)((LPBYTE)baseAddress + entryRVA);

    printf("Entry point RVA: 0x%X, VA: 0x%X\n", entryRVA, entryVA);
    return entryVA;
}
int break_at_entry(const char* process_name)
{
    STARTUPINFOA startup = { 0 };
    PROCESS_INFORMATION proc = { 0 };
    DEBUG_EVENT debug_event;

    startup.cb = sizeof(startup);

    // Create process in debug mode
    if (!CreateProcessA(
        NULL,
        process_name,
        NULL,
        NULL,
        FALSE,
        DEBUG_ONLY_THIS_PROCESS | CREATE_SUSPENDED,
        NULL,
        NULL,
        &startup,
        &proc
    )) {
        printf("CreateProcess failed: %lu\n", GetLastError());
        return -1;
    }

    printf("Process created! PID: %lu\n", proc.dwProcessId);

    // Resume thread to start process
    ResumeThread(proc.hThread);

    DWORD entryVA = 0;
    int entrySet = 0;

    while (1) {
        WaitForDebugEvent(&debug_event, INFINITE);

        switch (debug_event.dwDebugEventCode) {

        case CREATE_PROCESS_DEBUG_EVENT:
            if (!entrySet) {
                entryVA = get_entry_point(proc.hProcess, debug_event.u.CreateProcessInfo.lpBaseOfImage);

                // Set breakpoint at entry
                BYTE origByte;
                ReadProcessMemory(proc.hProcess, (LPCVOID)entryVA, &origByte, 1, NULL);
                BYTE int3 = 0xCC;
                WriteProcessMemory(proc.hProcess, (LPVOID)entryVA, &int3, 1, NULL);
                FlushInstructionCache(proc.hProcess, (LPVOID)entryVA, 1);

                entrySet = 1;
            }
            break;

        case EXCEPTION_DEBUG_EVENT: {
            DWORD addr = (DWORD)debug_event.u.Exception.ExceptionRecord.ExceptionAddress;
            DWORD code = debug_event.u.Exception.ExceptionRecord.ExceptionCode;

            if (code == EXCEPTION_BREAKPOINT) {
                printf("Breakpoint hit at VA: 0x%X\n", addr);

                if (addr == entryVA) {
                    printf("=== ENTRY POINT HIT === 0x%X\n", addr);
                    printf("Press ENTER to continue execution... (work in progress) (aka it prob wont work)\n");
                    getchar(); // wait for user input

                    // Restore original byte
                    BYTE origByte;
                    ReadProcessMemory(proc.hProcess, (LPCVOID)entryVA, &origByte, 1, NULL);
                    WriteProcessMemory(proc.hProcess, (LPVOID)entryVA, &origByte, 1, NULL);
                    FlushInstructionCache(proc.hProcess, (LPVOID)entryVA, 1);
                }
            }
            else {
                // Other exceptions can be printed or skipped
            }
            break;
        }

        case LOAD_DLL_DEBUG_EVENT: {
            LPVOID base = debug_event.u.LoadDll.lpBaseOfDll;
            LPVOID namePtr = debug_event.u.LoadDll.lpImageName;
            print_loaded_dll(proc.hProcess, base, namePtr);
            break;
        }

        case EXIT_PROCESS_DEBUG_EVENT:
            printf("EXIT_PROCESS_DEBUG_EVENT\n");
            return 0;

        default:
            printf("Other debug event: %lu\n", debug_event.dwDebugEventCode);
        }

        ContinueDebugEvent(debug_event.dwProcessId, debug_event.dwThreadId, DBG_CONTINUE);
    }

    return 0;
}

int main() {
    // Get the process name from the user
    //char process[256];
    //printf("Enter the name of the process to create: \n");
    //scanf_s("%255s", process, (unsigned)_countof(process));

    //updated b/c sumthin abt error 2 and error 123, bruh idek anymore.
    char process[512];
    printf("Enter the process path:\n");
    fgets(process, sizeof(process), stdin);

    // Remove trailing newline
    process[strcspn(process, "\n")] = 0;

    // Strip surrounding quotes if present
    if (process[0] == '"' && process[strlen(process) - 1] == '"') {
        process[strlen(process) - 1] = 0;
        memmove(process, process + 1, strlen(process));
    }

	// IT WORKS, I SWEAR TO GOD, I SWEAR ON MY LIFE, I SWEAR ON MY MOTHER, I SWEAR ON MY CHILDREN, I SWEAR ON MY DOG, I SWEAR ON MY CAT, I SWEAR ON MY FISH, I SWEAR ON MY PLANTS, I SWEAR ON THE ENTIRE UNIVERSE, IT WORKS. IT FUCKING WORKS!

    break_at_entry(process);
    //break_at_entry("C:\\Windows\\System32\\notepad.exe"); //hardcoded bc chatpt 5.1 said im a "r_t_rd".



    return 0;
}