#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tlhelp32.h>

// most of this is just slop
// but some good code is in here to0 (made by me)

// Version string intentionally inlined to avoid symbol/macro conflicts with system headers
// "v1.1.0"

// Constants and defines
#define MAX_RESULTS 10000
#define SCAN_PAGE_SIZE 4096
#define MIN_ADDRESS 0x00400000
#define MAX_ADDRESS 0x7FFFFFFF

// Data structures for memory search results
typedef struct {
	DWORD address;
	DWORD value;
} MemoryResult;

typedef struct {
	MemoryResult* results;
	int count;
	int capacity;
} ScanResults;

// Functions
HANDLE StartPausedProc(const char* cmd, PHANDLE hThread) {
	PROCESS_INFORMATION procInfo = { 0 };
	printf("Created ProcessInfo: %p, PID: %lu, ThreadID: %lu\n", (void*)&procInfo, procInfo.dwProcessId, procInfo.dwThreadId);

	STARTUPINFOA starInfo = { 0 };
	printf("Created StartupInfo: %p, cb: %lu\n", (void*)&starInfo, starInfo.cb);

	starInfo.cb = sizeof(starInfo);
	printf("Set StartupInfo.cb: %lu\n", (unsigned long)starInfo.cb);
	if (!CreateProcessA(
		NULL,
		cmd,
		NULL,
		NULL,
		FALSE,
		CREATE_SUSPENDED,
		NULL,
		NULL,
		&starInfo,
		&procInfo
	)) {
		printf("CreateProcess failed: %lu\n", GetLastError());
		return NULL;
	}
	printf("Process Created: %s, %p, PID: %lu, ThreadID: %lu\n", cmd, (void*)&procInfo, procInfo.dwProcessId, procInfo.dwThreadId);

	*hThread = procInfo.hThread;
	return procInfo.hProcess;

}

int ReadProcessMemoryEx(HANDLE hProcess, LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize) {
	SIZE_T bytesRead = 0;
	if (!ReadProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, &bytesRead)) {
		printf("ReadProcessMemory failed: %lu\n", GetLastError());
		return -1;
	}
	return (int)bytesRead;
}

int CloseHandles(HANDLE hProcess, HANDLE hThread) {
	if (hThread) {
		CloseHandle(hThread);
		printf("Closed thread handle: %p\n", (void*)hThread);
	}
	if (hProcess) {
		CloseHandle(hProcess);
		printf("Closed process handle: %p\n", (void*)hProcess);
	}
	return 0;
}

// Initialize scan results structure
ScanResults* InitScanResults() {
	ScanResults* pResults = (ScanResults*)malloc(sizeof(ScanResults));
	if (!pResults) {
		printf("Failed to allocate ScanResults\n");
		return NULL;
	}
	pResults->results = (MemoryResult*)malloc(sizeof(MemoryResult) * MAX_RESULTS);
	if (!pResults->results) {
		printf("Failed to allocate results buffer\n");
		free(pResults);
		return NULL;
	}
	pResults->count = 0;
	pResults->capacity = MAX_RESULTS;
	return pResults;
}

// Free scan results
void FreeScanResults(ScanResults* pResults) {
	if (pResults) {
		if (pResults->results) {
			free(pResults->results);
		}
		free(pResults);
	}
}

// Add result to scan results
int AddResult(ScanResults* pResults, DWORD address, DWORD value) {
	if (!pResults) return -1;
	if (pResults->count >= pResults->capacity) {
		printf("Warning: Maximum results (%d) reached. Stopping scan.\n", MAX_RESULTS);
		return -1;
	}
	pResults->results[pResults->count].address = address;
	pResults->results[pResults->count].value = value;
	pResults->count++;
	return 0;
}

// Print scan results
void PrintScanResults(ScanResults* pResults, int maxDisplay) {
	if (!pResults) return;

	printf("\n=== Scan Results ===\n");
	printf("Total matches found: %d\n", pResults->count);

	int displayCount = (pResults->count < maxDisplay) ? pResults->count : maxDisplay;
	printf("Displaying first %d results:\n", displayCount);

	for (int i = 0; i < displayCount; i++) {
		printf("[%d] Address: 0x%08lX, Value: %lu (0x%08lX)\n",
			i + 1,
			pResults->results[i].address,
			pResults->results[i].value,
			pResults->results[i].value);
	}

	if (pResults->count > maxDisplay) {
		printf("... and %d more results\n", pResults->count - maxDisplay);
	}
	printf("\n");
}

// Scan process memory for a specific DWORD value
ScanResults* ScanProcessMemory(HANDLE hProcess, DWORD searchValue, DWORD startAddr, DWORD endAddr) {
	ScanResults* pResults = InitScanResults();
	if (!pResults) return NULL;

	printf("Starting memory scan for value: %lu (0x%08lX)\n", searchValue, searchValue);
	printf("Scanning from 0x%08lX to 0x%08lX\n", startAddr, endAddr);

	DWORD buffer[1024];
	DWORD bytesRead = 0;
	DWORD currentAddr = startAddr;

	while (currentAddr < endAddr) {
		// Attempt to read a page of memory
		if (!ReadProcessMemory(hProcess, (LPCVOID)currentAddr, buffer, sizeof(buffer), &bytesRead)) {
			// Skip this page if we can't read it
			currentAddr += SCAN_PAGE_SIZE;
			continue;
		}

		// Search through the bytes we read
		DWORD dwordsRead = bytesRead / sizeof(DWORD);
		for (DWORD i = 0; i < dwordsRead; i++) {
			if (buffer[i] == searchValue) {
				DWORD foundAddr = currentAddr + (i * sizeof(DWORD));
				if (AddResult(pResults, foundAddr, searchValue) < 0) {
					printf("Scan complete (max results reached)\n");
					return pResults;
				}
			}
		}

		currentAddr += bytesRead;

		// Progress indicator
		if ((currentAddr % 0x100000) == 0) {
			printf("Scanned: 0x%08lX - Found %d matches so far\n", currentAddr, pResults->count);
		}
	}

	printf("Scan complete. Found %d matches.\n", pResults->count);
	return pResults;
}

// Scan for multiple values and return intersecting addresses
ScanResults* ScanAndFilter(HANDLE hProcess, DWORD searchValue) {
	return ScanProcessMemory(hProcess, searchValue, MIN_ADDRESS, MAX_ADDRESS);
}

// Verify a specific address still contains the expected value
int VerifyAddress(HANDLE hProcess, DWORD address, DWORD expectedValue) {
	DWORD buffer;
	SIZE_T bytesRead = 0;

	if (!ReadProcessMemory(hProcess, (LPCVOID)address, &buffer, sizeof(buffer), &bytesRead)) {
		printf("Failed to verify address 0x%08lX: %lu\n", address, GetLastError());
		return 0;
	}

	if (buffer == expectedValue) {
		printf("Address 0x%08lX verified: Value matches (%lu)\n", address, buffer);
		return 1;
	} else {
		printf("Address 0x%08lX verification failed: Expected %lu, got %lu\n", 
			address, expectedValue, buffer);
		return 0;
	}
}

// Write value to a process address (for patching)
int WriteToAddress(HANDLE hProcess, DWORD address, DWORD value) {
	SIZE_T bytesWritten = 0;

	if (!WriteProcessMemory(hProcess, (LPVOID)address, &value, sizeof(value), &bytesWritten)) {
		printf("Failed to write to address 0x%08lX: %lu\n", address, GetLastError());
		return 0;
	}

	printf("Successfully wrote %lu to address 0x%08lX\n", value, address);
	return 1;
}

// Get memory information about a specific region
int GetMemoryRegionInfo(HANDLE hProcess, DWORD address) {
	MEMORY_BASIC_INFORMATION mbi;

	if (!VirtualQueryEx(hProcess, (LPCVOID)address, &mbi, sizeof(mbi))) {
		printf("Failed to query memory at 0x%08lX: %lu\n", address, GetLastError());
		return 0;
	}

	printf("\n=== Memory Region Info at 0x%08lX ===\n", address);
	printf("Base Address: 0x%08lX\n", (DWORD)mbi.BaseAddress);
	printf("Allocation Base: 0x%08lX\n", (DWORD)mbi.AllocationBase);
	printf("Region Size: %lu bytes (0x%lX)\n", mbi.RegionSize, mbi.RegionSize);
	printf("State: %s\n", 
		(mbi.State == MEM_COMMIT) ? "Committed" : 
		(mbi.State == MEM_RESERVE) ? "Reserved" : 
		(mbi.State == MEM_FREE) ? "Free" : "Unknown");
	printf("Protect: %s\n",
		(mbi.Protect == PAGE_EXECUTE) ? "Execute" :
		(mbi.Protect == PAGE_EXECUTE_READ) ? "Execute/Read" :
		(mbi.Protect == PAGE_EXECUTE_READWRITE) ? "Execute/Read/Write" :
		(mbi.Protect == PAGE_READWRITE) ? "Read/Write" :
		(mbi.Protect == PAGE_READONLY) ? "Read Only" :
		(mbi.Protect == PAGE_NOACCESS) ? "No Access" : "Unknown");
	printf("\n");

	return 1;
}

// Refine results by searching only in regions with specific address range
ScanResults* RefineResults(ScanResults* pCurrentResults, DWORD minAddr, DWORD maxAddr) {
	if (!pCurrentResults) return NULL;

	ScanResults* pRefined = InitScanResults();
	if (!pRefined) return NULL;

	printf("Refining %d results to addresses between 0x%08lX and 0x%08lX\n", 
		pCurrentResults->count, minAddr, maxAddr);

	for (int i = 0; i < pCurrentResults->count; i++) {
		DWORD addr = pCurrentResults->results[i].address;
		if (addr >= minAddr && addr <= maxAddr) {
			AddResult(pRefined, addr, pCurrentResults->results[i].value);
		}
	}

	printf("Refined to %d results.\n", pRefined->count);
	return pRefined;
}

// Compare two scan results to find addresses that changed
ScanResults* CompareScanResults(ScanResults* pOldResults, HANDLE hProcess) {
	if (!pOldResults || pOldResults->count == 0) {
		printf("No previous results to compare.\n");
		return NULL;
	}

	ScanResults* pChanged = InitScanResults();
	if (!pChanged) return NULL;

	printf("Comparing %d addresses for changes...\n", pOldResults->count);

	for (int i = 0; i < pOldResults->count; i++) {
		DWORD address = pOldResults->results[i].address;
		DWORD oldValue = pOldResults->results[i].value;
		DWORD newValue = 0;
		SIZE_T bytesRead = 0;

		if (ReadProcessMemory(hProcess, (LPCVOID)address, &newValue, sizeof(newValue), &bytesRead)) {
			if (newValue != oldValue) {
				AddResult(pChanged, address, newValue);
				printf("Address 0x%08lX changed: %lu -> %lu\n", address, oldValue, newValue);
			}
		}
	}

	printf("Found %d addresses with changed values.\n", pChanged->count);
	return pChanged;
}

// Save scan results to a file
int SaveResultsToFile(ScanResults* pResults, const char* filename) {
	if (!pResults) {
		printf("No results to save.\n");
		return 0;
	}

	FILE* pFile = fopen(filename, "w");
	if (!pFile) {
		printf("Failed to open file: %s\n", filename);
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
	printf("Results saved to: %s\n", filename);
	return 1;
}

// Load scan results from a file
ScanResults* LoadResultsFromFile(const char* filename) {
	FILE* pFile = fopen(filename, "r");
	if (!pFile) {
		printf("Failed to open file: %s\n", filename);
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
		if (lineNum <= 3) continue; // Skip header lines

		DWORD address = 0, value = 0;
		if (sscanf_s(buffer, "[%*d] Address: 0x%lX, Value: %lu", &address, &value) == 2) {
			AddResult(pResults, address, value);
		}
	}

	fclose(pFile);
	printf("Loaded %d results from: %s\n", pResults->count, filename);
	return pResults;
}

// Print help/tutorial for memory search
void PrintMemorySearchHelp() {
	printf("\n");
	printf("=== Project C %s ===\n", "v1.1.0");
	printf("Author: Lotux + Claude Haiku 4.5\n");
	printf("This tool was made with the help of Claude (yes i wrote most of this)\n");
	printf("\n");
	printf("This is a Cheat Engine-like memory scanner for Windows processes.\n");
	printf("\n");
	printf("BASIC WORKFLOW:\n");
	printf("1. Enter the process path (the program to scan)\n");
	printf("2. Once loaded, press Enter to start the search menu\n");
	printf("3. Perform a scan by entering a value you want to find\n");
	printf("4. The tool will search the process memory for that value\n");
	printf("5. Review results and verify addresses\n");
	printf("6. Optionally write new values to found addresses\n");
	printf("\n");
	printf("ADVANCED FEATURES:\n");
	printf("- Compare multiple scans to find changing values\n");
	printf("- Refine results by address range\n");
	printf("- Query memory region information\n");
	printf("- Save and load results from files\n");
	printf("- Verify addresses still contain expected values\n");
	printf("\n");
	printf("TIPS:\n");
	printf("- Process starts SUSPENDED - you can search before resuming\n");
	printf("- For finding variables, look for their current value\n");
	printf("- Use multiple scans and filtering for better precision\n");
	printf("- Save results between sessions for later analysis\n");
	printf("\n");
}

// Interactive memory search loop
void InteractiveMemorySearch(HANDLE hProcess, HANDLE hThread) {
	int menuChoice = 0;
	ScanResults* pCurrentResults = NULL;
	ScanResults* pPreviousResults = NULL;

	while (1) {
		printf("\n=== Project C - Memory Search Menu ===\n");
		printf("1. New scan for value\n");
		printf("2. Display current results\n");
		printf("3. Verify specific address\n");
		printf("4. Clear results\n");
		printf("5. Refine results by address range\n");
		printf("6. Write value to address\n");
		printf("7. Get memory region info\n");
		printf("8. Compare with previous scan (find changes)\n");
		printf("9. Save results to file\n");
		printf("10. Load results from file\n");
		printf("11. Resume suspended process\n");
		printf("12. Exit search\n");
		printf("Enter choice: ");

		if (scanf_s("%d", &menuChoice) != 1) {
			while (getchar() != '\n');
			printf("Invalid input. Please enter a number.\n");
			continue;
		}
		while (getchar() != '\n');

		system("cls");

		switch (menuChoice) {
			case 1: {
				printf("Enter value to search for: ");
				DWORD searchValue = 0;
				if (scanf_s("%lu", &searchValue) != 1) {
					while (getchar() != '\n');
					printf("Invalid input.\n");
					break;
				}
				while (getchar() != '\n');

				// Save current results as previous before scanning
				if (pPreviousResults) {
					FreeScanResults(pPreviousResults);
				}
				pPreviousResults = pCurrentResults;

				// Perform the scan
				pCurrentResults = ScanAndFilter(hProcess, searchValue);

				if (pCurrentResults) {
					PrintScanResults(pCurrentResults, 20);
				}
				break;
			}

			case 2: {
				if (pCurrentResults == NULL) {
					printf("No current results. Perform a scan first.\n");
				} else {
					PrintScanResults(pCurrentResults, 20);
				}
				break;
			}

			case 3: {
				if (pCurrentResults == NULL || pCurrentResults->count == 0) {
					printf("No results to verify. Perform a scan first.\n");
					break;
				}

				printf("Enter result index to verify (1-%d): ", pCurrentResults->count);
				int index = 0;
				if (scanf_s("%d", &index) != 1) {
					while (getchar() != '\n');
					printf("Invalid input.\n");
					break;
				}
				while (getchar() != '\n');

				if (index < 1 || index > pCurrentResults->count) {
					printf("Invalid index.\n");
					break;
				}

				MemoryResult* result = &pCurrentResults->results[index - 1];
				VerifyAddress(hProcess, result->address, result->value);
				break;
			}

			case 4: {
				if (pCurrentResults) {
					FreeScanResults(pCurrentResults);
					pCurrentResults = NULL;
					printf("Results cleared.\n");
				}
				break;
			}

			case 5: {
				if (pCurrentResults == NULL || pCurrentResults->count == 0) {
					printf("No results to refine. Perform a scan first.\n");
					break;
				}

				printf("Enter minimum address (hex, e.g., 0x400000): ");
				DWORD minAddr = 0;
				if (scanf_s("%X", &minAddr) != 1) {
					while (getchar() != '\n');
					printf("Invalid input.\n");
					break;
				}
				while (getchar() != '\n');

				printf("Enter maximum address (hex, e.g., 0x7FFFFFFF): ");
				DWORD maxAddr = 0;
				if (scanf_s("%X", &maxAddr) != 1) {
					while (getchar() != '\n');
					printf("Invalid input.\n");
					break;
				}
				while (getchar() != '\n');

				ScanResults* pRefined = RefineResults(pCurrentResults, minAddr, maxAddr);
				if (pRefined) {
					FreeScanResults(pCurrentResults);
					pCurrentResults = pRefined;
					PrintScanResults(pCurrentResults, 20);
				}
				break;
			}

			case 6: {
				printf("Enter address to write to (hex, e.g., 0x400000): ");
				DWORD address = 0;
				if (scanf_s("%X", &address) != 1) {
					while (getchar() != '\n');
					printf("Invalid input.\n");
					break;
				}

				printf("Enter value to write: ");
				DWORD value = 0;
				if (scanf_s("%lu", &value) != 1) {
					while (getchar() != '\n');
					printf("Invalid input.\n");
					break;
				}
				while (getchar() != '\n');

				WriteToAddress(hProcess, address, value);
				break;
			}

			case 7: {
				printf("Enter address to query (hex, e.g., 0x400000): ");
				DWORD address = 0;
				if (scanf_s("%X", &address) != 1) {
					while (getchar() != '\n');
					printf("Invalid input.\n");
					break;
				}
				while (getchar() != '\n');

				GetMemoryRegionInfo(hProcess, address);
				break;
			}

			case 8: {
				if (pPreviousResults == NULL) {
					printf("No previous scan to compare. Perform at least 2 scans first.\n");
					break;
				}

				ScanResults* pChanged = CompareScanResults(pPreviousResults, hProcess);
				if (pChanged && pChanged->count > 0) {
					if (pCurrentResults) {
						FreeScanResults(pCurrentResults);
					}
					pCurrentResults = pChanged;
					PrintScanResults(pCurrentResults, 20);
				} else if (pChanged) {
					printf("No addresses changed since last scan.\n");
					FreeScanResults(pChanged);
				}
				break;
			}

			case 9: {
				if (pCurrentResults == NULL || pCurrentResults->count == 0) {
					printf("No results to save.\n");
					break;
				}

				printf("Enter filename to save to: ");
				char filename[256];
				if (fgets(filename, sizeof(filename), stdin) == NULL) {
					printf("Invalid input.\n");
					break;
				}

				// Remove trailing newline
				size_t len = strlen(filename);
				if (len > 0 && filename[len - 1] == '\n') {
					filename[len - 1] = '\0';
				}

				SaveResultsToFile(pCurrentResults, filename);
				break;
			}

			case 10: {
				printf("Enter filename to load from: ");
				char filename[256];
				if (fgets(filename, sizeof(filename), stdin) == NULL) {
					printf("Invalid input.\n");
					break;
				}

				// Remove trailing newline
				size_t len = strlen(filename);
				if (len > 0 && filename[len - 1] == '\n') {
					filename[len - 1] = '\0';
				}

				ScanResults* pLoaded = LoadResultsFromFile(filename);
				if (pLoaded && pLoaded->count > 0) {
					if (pCurrentResults) {
						FreeScanResults(pCurrentResults);
					}
					pCurrentResults = pLoaded;
					PrintScanResults(pCurrentResults, 20);
				} else if (pLoaded) {
					printf("File loaded but contains no valid results.\n");
					FreeScanResults(pLoaded);
				}
				break;
			}

			case 11: {
				DWORD resumeCount = ResumeThread(hThread);
				if (resumeCount == 0xFFFFFFFF) {
					printf("Failed to resume process thread: %lu\n", GetLastError());
				} else {
					printf("Process thread resumed successfully (resume count: %lu)\n", resumeCount);
					printf("Process is now running. Scans will show live values.\n");
				}
				break;
			}

			case 12: {
				if (pCurrentResults) {
					FreeScanResults(pCurrentResults);
				}
				if (pPreviousResults) {
					FreeScanResults(pPreviousResults);
				}
				printf("Exiting memory search.\n");
				return;
			}

			default:
				printf("Invalid choice. Please try again.\n");
		}
	}
}

int main() {
	// Print initial help
	PrintMemorySearchHelp();

	// Get the process path from the user
	char processName[512];
	{
		printf("Enter the process path:\n");
		fgets(processName, sizeof(processName), stdin);
		// Remove trailing newline from fgets
		size_t len = strlen(processName);
		if (len > 0 && processName[len - 1] == '\n') {
			processName[len - 1] = '\0';
		}
	}

	// Start the process in a suspended state and get the thread handle
	HANDLE hThread = NULL;
	HANDLE hProcess = StartPausedProc(processName, &hThread);
	{
		// Check if process and thread were created successfully
		if (!hProcess || !hThread) {
			fprintf(stderr, "Failed to create process or thread\n");
			return 1;
		}
	}

	//LOOP
	{
		printf("\nStarting interactive memory search mode.\n");
		printf("Note: Process is currently SUSPENDED. Resume it if needed before searching.\n");
		printf("You can still search memory while paused, but the values won't change.\n\n");

		printf("Press Enter to start the search menu: ");
		getchar();

		InteractiveMemorySearch(hProcess, hThread);
	}

	// Cleanup
	printf("\nClosing handles and exiting.\n");
	CloseHandles(hProcess, hThread);
	return 0;
}