#define _CRT_SECURE_NO_WARNINGS
#include "../Headers/memory_scanner.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

HANDLE StartPausedProc(const char* cmd, PHANDLE hThread) {
	PROCESS_INFORMATION procInfo = { 0 };
	STARTUPINFOA starInfo = { 0 };
	starInfo.cb = sizeof(starInfo);
	
	if (!CreateProcessA(
		NULL,
		(LPSTR)cmd,
		NULL,
		NULL,
		FALSE,
		CREATE_SUSPENDED,
		NULL,
		NULL,
		&starInfo,
		&procInfo
	)) {
		return NULL;
	}

	*hThread = procInfo.hThread;
	return procInfo.hProcess;
}

int CloseHandles(HANDLE hProcess, HANDLE hThread) {
	if (hThread) {
		CloseHandle(hThread);
	}
	if (hProcess) {
		CloseHandle(hProcess);
	}
	return 0;
}

ScanResults* InitScanResults(void) {
	ScanResults* pResults = (ScanResults*)malloc(sizeof(ScanResults));
	if (!pResults) {
		return NULL;
	}
	pResults->results = (MemoryResult*)malloc(sizeof(MemoryResult) * MAX_RESULTS);
	if (!pResults->results) {
		free(pResults);
		return NULL;
	}
	pResults->count = 0;
	pResults->capacity = MAX_RESULTS;
	return pResults;
}

void FreeScanResults(ScanResults* pResults) {
	if (pResults) {
		if (pResults->results) {
			free(pResults->results);
		}
		free(pResults);
	}
}

int AddResult(ScanResults* pResults, DWORD address, DWORD value) {
	if (!pResults) return -1;
	if (pResults->count >= pResults->capacity) {
		return -1;
	}
	pResults->results[pResults->count].address = address;
	pResults->results[pResults->count].value = value;
	pResults->count++;
	return 0;
}

ScanResults* ScanProcessMemory(HANDLE hProcess, DWORD searchValue, DWORD startAddr, DWORD endAddr) {
	ScanResults* pResults = InitScanResults();
	if (!pResults) return NULL;

	DWORD buffer[1024];
	DWORD bytesRead = 0;
	DWORD currentAddr = startAddr;

	while (currentAddr < endAddr) {
		if (!ReadProcessMemory(hProcess, (LPCVOID)currentAddr, buffer, sizeof(buffer), (SIZE_T*)&bytesRead)) {
			currentAddr += SCAN_PAGE_SIZE;
			continue;
		}

		DWORD dwordsRead = bytesRead / sizeof(DWORD);
		for (DWORD i = 0; i < dwordsRead; i++) {
			if (buffer[i] == searchValue) {
				DWORD foundAddr = currentAddr + (i * sizeof(DWORD));
				if (AddResult(pResults, foundAddr, searchValue) < 0) {
					return pResults;
				}
			}
		}

		currentAddr += bytesRead;
	}

	return pResults;
}

ScanResults* ScanAndFilter(HANDLE hProcess, DWORD searchValue) {
	return ScanProcessMemory(hProcess, searchValue, MIN_ADDRESS, MAX_ADDRESS);
}

int VerifyAddress(HANDLE hProcess, DWORD address, DWORD expectedValue) {
	DWORD buffer;
	SIZE_T bytesRead = 0;

	if (!ReadProcessMemory(hProcess, (LPCVOID)address, &buffer, sizeof(buffer), &bytesRead)) {
		return 0;
	}

	return (buffer == expectedValue) ? 1 : 0;
}

int WriteToAddress(HANDLE hProcess, DWORD address, DWORD value) {
	SIZE_T bytesWritten = 0;

	if (!WriteProcessMemory(hProcess, (LPVOID)address, &value, sizeof(value), &bytesWritten)) {
		return 0;
	}

	return 1;
}

int GetMemoryRegionInfo(HANDLE hProcess, DWORD address, MEMORY_BASIC_INFORMATION* pmbi) {
	if (!VirtualQueryEx(hProcess, (LPCVOID)address, pmbi, sizeof(MEMORY_BASIC_INFORMATION))) {
		return 0;
	}
	return 1;
}

ScanResults* RefineResults(ScanResults* pCurrentResults, DWORD minAddr, DWORD maxAddr) {
	if (!pCurrentResults) return NULL;

	ScanResults* pRefined = InitScanResults();
	if (!pRefined) return NULL;

	for (int i = 0; i < pCurrentResults->count; i++) {
		DWORD addr = pCurrentResults->results[i].address;
		if (addr >= minAddr && addr <= maxAddr) {
			AddResult(pRefined, addr, pCurrentResults->results[i].value);
		}
	}

	return pRefined;
}

ScanResults* CompareScanResults(ScanResults* pOldResults, HANDLE hProcess) {
	if (!pOldResults || pOldResults->count == 0) {
		return NULL;
	}

	ScanResults* pChanged = InitScanResults();
	if (!pChanged) return NULL;

	for (int i = 0; i < pOldResults->count; i++) {
		DWORD address = pOldResults->results[i].address;
		DWORD oldValue = pOldResults->results[i].value;
		DWORD newValue = 0;
		SIZE_T bytesRead = 0;

		if (ReadProcessMemory(hProcess, (LPCVOID)address, &newValue, sizeof(newValue), &bytesRead)) {
			if (newValue != oldValue) {
				AddResult(pChanged, address, newValue);
			}
		}
	}

	return pChanged;
}

int SaveResultsToFile(ScanResults* pResults, const char* filename) {
	if (!pResults) {
		return 0;
	}

	FILE* pFile = fopen(filename, "w");
	if (!pFile) {
		return 0;
	}

	fprintf(pFile, "=== Scan Results ===\n");
	fprintf(pFile, "Total matches: %d\n\n", pResults->count);

	for (int i = 0; i < pResults->count; i++) {
		fprintf(pFile, "[%d] Address: 0x%08lX, Value: %lu (0x%08lX)\n",
			i + 1,
			pResults->results[i].address,
			pResults->results[i].value,
			pResults->results[i].value);
	}

	fclose(pFile);
	return 1;
}

ScanResults* LoadResultsFromFile(const char* filename) {
	FILE* pFile = fopen(filename, "r");
	if (!pFile) {
		return NULL;
	}

	ScanResults* pResults = InitScanResults();
	if (!pResults) {
		fclose(pFile);
		return NULL;
	}

	char buffer[256];
	int lineNum = 0;

	while (fgets(buffer, sizeof(buffer), pFile)) {
		lineNum++;
		if (lineNum <= 3) continue;

		DWORD address = 0, value = 0;
		if (sscanf_s(buffer, "[%*d] Address: 0x%lX, Value: %lu", &address, &value) == 2) {
			AddResult(pResults, address, value);
		}
	}

	fclose(pFile);
	return pResults;
}
