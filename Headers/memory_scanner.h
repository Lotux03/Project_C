#pragma once
#ifndef MEMORY_SCANNER_H
#define MEMORY_SCANNER_H

#include <windows.h>

#define MAX_RESULTS 10000
#define SCAN_PAGE_SIZE 4096
#define MIN_ADDRESS 0x00400000
#define MAX_ADDRESS 0x7FFFFFFF

typedef struct {
	DWORD address;
	DWORD value;
} MemoryResult;

typedef struct {
	MemoryResult* results;
	int count;
	int capacity;
} ScanResults;

// Process management
HANDLE StartPausedProc(const char* cmd, PHANDLE hThread);
int CloseHandles(HANDLE hProcess, HANDLE hThread);

// Scan results
ScanResults* InitScanResults(void);
void FreeScanResults(ScanResults* pResults);
int AddResult(ScanResults* pResults, DWORD address, DWORD value);

// Memory operations
ScanResults* ScanProcessMemory(HANDLE hProcess, DWORD searchValue, DWORD startAddr, DWORD endAddr);
ScanResults* ScanAndFilter(HANDLE hProcess, DWORD searchValue);
int VerifyAddress(HANDLE hProcess, DWORD address, DWORD expectedValue);
int WriteToAddress(HANDLE hProcess, DWORD address, DWORD value);
int GetMemoryRegionInfo(HANDLE hProcess, DWORD address, MEMORY_BASIC_INFORMATION* pmbi);
ScanResults* RefineResults(ScanResults* pCurrentResults, DWORD minAddr, DWORD maxAddr);
ScanResults* CompareScanResults(ScanResults* pOldResults, HANDLE hProcess);

// File I/O
int SaveResultsToFile(ScanResults* pResults, const char* filename);
ScanResults* LoadResultsFromFile(const char* filename);

#endif
