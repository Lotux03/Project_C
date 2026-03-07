# Complete Function Reference - Memory Search System

## Data Structures

### MemoryResult
```c
typedef struct {
    DWORD address;  // Memory address where value was found
    DWORD value;    // The DWORD value found at that address
} MemoryResult;
```

### ScanResults
```c
typedef struct {
    MemoryResult* results;  // Dynamic array of search results
    int count;              // Number of results currently stored
    int capacity;           // Maximum capacity (10,000)
} ScanResults;
```

---

## Core Functions

### Memory Result Management

#### `ScanResults* InitScanResults()`
**Purpose:** Create and initialize a new scan results container

**Returns:** Pointer to allocated ScanResults, or NULL on failure

**Example:**
```c
ScanResults* results = InitScanResults();
if (!results) {
    printf("Memory allocation failed\n");
    return;
}
```

---

#### `void FreeScanResults(ScanResults* pResults)`
**Purpose:** Free all memory associated with scan results

**Parameters:**
- `pResults` - Pointer to ScanResults to free

**Example:**
```c
FreeScanResults(results);
results = NULL;  // Good practice
```

---

#### `int AddResult(ScanResults* pResults, DWORD address, DWORD value)`
**Purpose:** Add a new match to the results array

**Parameters:**
- `pResults` - Target ScanResults structure
- `address` - Memory address of the match
- `value` - The value found at that address

**Returns:** 0 on success, -1 if capacity exceeded

**Example:**
```c
if (AddResult(results, 0x00402000, 42) == 0) {
    printf("Result added\n");
} else {
    printf("Max results reached\n");
}
```

---

#### `void PrintScanResults(ScanResults* pResults, int maxDisplay)`
**Purpose:** Display scan results in a formatted table

**Parameters:**
- `pResults` - ScanResults to display
- `maxDisplay` - Maximum number to show (shows "... and X more" if exceeded)

**Example:**
```c
PrintScanResults(results, 20);  // Show first 20
```

---

### Memory Scanning Functions

#### `ScanResults* ScanProcessMemory(HANDLE hProcess, DWORD searchValue, DWORD startAddr, DWORD endAddr)`
**Purpose:** Core memory scanning algorithm - searches process memory for a specific DWORD value

**Parameters:**
- `hProcess` - Handle to target process
- `searchValue` - DWORD value to search for
- `startAddr` - Start address for scan
- `endAddr` - End address for scan

**Returns:** ScanResults pointer with all matches found

**Details:**
- Reads memory in 4KB chunks for efficiency
- Skips inaccessible memory regions gracefully
- Shows progress every 1MB
- Stops automatically if max results (10,000) reached

**Example:**
```c
ScanResults* results = ScanProcessMemory(
    hProcess, 
    42,                    // Search for value 42
    0x00400000,           // Start address
    0x7FFFFFFF            // End address
);
if (results) {
    PrintScanResults(results, 20);
}
```

---

#### `ScanResults* ScanAndFilter(HANDLE hProcess, DWORD searchValue)`
**Purpose:** Wrapper function for full process scan

**Parameters:**
- `hProcess` - Handle to target process
- `searchValue` - DWORD value to search for

**Returns:** ScanResults pointer

**Example:**
```c
ScanResults* results = ScanAndFilter(hProcess, 42);
```

---

### Memory Verification Functions

#### `int VerifyAddress(HANDLE hProcess, DWORD address, DWORD expectedValue)`
**Purpose:** Read a specific address and verify it still contains the expected value

**Parameters:**
- `hProcess` - Handle to target process
- `address` - Address to read
- `expectedValue` - Expected value at that address

**Returns:** 1 if match, 0 if mismatch or error

**Example:**
```c
if (VerifyAddress(hProcess, 0x00402000, 42)) {
    printf("Address still contains correct value\n");
} else {
    printf("Value has changed or address is invalid\n");
}
```

---

#### `int GetMemoryRegionInfo(HANDLE hProcess, DWORD address)`
**Purpose:** Query detailed information about a memory region

**Parameters:**
- `hProcess` - Handle to target process
- `address` - Any address within the region to query

**Returns:** 1 on success, 0 on failure

**Displays:**
- Base address of the region
- Allocation base
- Region size
- Memory state (Committed/Reserved/Free)
- Memory protection flags

**Example:**
```c
GetMemoryRegionInfo(hProcess, 0x00402000);
// Output shows region properties
```

---

### Memory Modification Functions

#### `int WriteToAddress(HANDLE hProcess, DWORD address, DWORD value)`
**Purpose:** Write a DWORD value to process memory

**Parameters:**
- `hProcess` - Handle to target process
- `address` - Target address to write to
- `value` - DWORD value to write

**Returns:** 1 on success, 0 on failure

**WARNING:** Only write to addresses you understand! Can cause crashes.

**Example:**
```c
if (WriteToAddress(hProcess, 0x00402000, 999)) {
    printf("Successfully wrote 999 to memory\n");
} else {
    printf("Write failed\n");
}
```

---

### Analysis Functions

#### `ScanResults* CompareScanResults(ScanResults* pOldResults, HANDLE hProcess)`
**Purpose:** Find which addresses have changed since previous scan

**Parameters:**
- `pOldResults` - Previous scan results
- `hProcess` - Handle to target process

**Returns:** ScanResults containing only changed addresses

**Details:**
- Re-reads each address in previous results
- Compares current values with stored values
- Returns only addresses where values differ

**Example:**
```c
// First scan
ScanResults* scan1 = ScanAndFilter(hProcess, 100);

// [User modifies target process]

// Compare
ScanResults* changed = CompareScanResults(scan1, hProcess);
PrintScanResults(changed, 20);  // Shows changed addresses
```

---

#### `ScanResults* RefineResults(ScanResults* pCurrentResults, DWORD minAddr, DWORD maxAddr)`
**Purpose:** Filter existing results by address range

**Parameters:**
- `pCurrentResults` - Results to filter
- `minAddr` - Minimum address (inclusive)
- `maxAddr` - Maximum address (inclusive)

**Returns:** New ScanResults containing only results within range

**Example:**
```c
// Have 5000 results, narrow to DLL region
ScanResults* refined = RefineResults(
    currentResults,
    0x10000000,  // Min address
    0x20000000   // Max address
);
// Now have fewer, more relevant results
```

---

### File I/O Functions

#### `int SaveResultsToFile(ScanResults* pResults, const char* filename)`
**Purpose:** Export scan results to a text file

**Parameters:**
- `pResults` - ScanResults to save
- `filename` - Output filename

**Returns:** 1 on success, 0 on failure

**File Format:**
```
=== Scan Results ===
Total matches: N

[1] Address: 0x00ABC123, Value: 42 (0x0000002A)
[2] Address: 0x00ABC456, Value: 42 (0x0000002A)
...
```

**Example:**
```c
if (SaveResultsToFile(results, "my_scan.txt")) {
    printf("Results saved\n");
} else {
    printf("Save failed\n");
}
```

---

#### `ScanResults* LoadResultsFromFile(const char* filename)`
**Purpose:** Load previously saved results from file

**Parameters:**
- `filename` - Input filename

**Returns:** ScanResults pointer, or NULL on failure

**Example:**
```c
ScanResults* loaded = LoadResultsFromFile("my_scan.txt");
if (loaded && loaded->count > 0) {
    PrintScanResults(loaded, 20);
} else {
    printf("Load failed\n");
}
```

---

## Helper Functions

#### `void PrintMemorySearchHelp()`
**Purpose:** Display help and tutorial information

**Example:**
```c
PrintMemorySearchHelp();  // Shows in main() on startup
```

---

#### `void InteractiveMemorySearch(HANDLE hProcess)`
**Purpose:** Main interactive menu loop - the primary user interface

**Parameters:**
- `hProcess` - Handle to target process

**Details:**
- Displays 11-option menu
- Handles user input and menu navigation
- Manages result lifecycle
- Provides access to all other functions

**Example:**
```c
InteractiveMemorySearch(hProcess);  // Called from main()
```

---

## Existing Functions (Already in Code)

#### `HANDLE StartPausedProc(const char* cmd, PHANDLE hThread)`
**Purpose:** Launch a process in suspended state

**Used by:** main()

---

#### `int ReadProcessMemoryEx(HANDLE hProcess, LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize)`
**Purpose:** Convenience wrapper for ReadProcessMemory

**Used by:** Scanning functions

---

#### `int CloseHandles(HANDLE hProcess, HANDLE hThread)`
**Purpose:** Safely close process and thread handles

**Used by:** main() for cleanup

---

## Constants (Configurable)

```c
#define MAX_RESULTS 10000         // Maximum scan results per scan
#define SCAN_PAGE_SIZE 4096       // Memory read chunk size
#define MIN_ADDRESS 0x00400000    // Start of user-mode memory
#define MAX_ADDRESS 0x7FFFFFFF    // End of user-mode memory
```

---

## Quick Call Diagram

```
main()
├── PrintMemorySearchHelp()      [Show help screen]
├── StartPausedProc()            [Launch process]
└── InteractiveMemorySearch()    [Main menu loop]
    ├── Option 1: ScanAndFilter()
    │   └── ScanProcessMemory()
    │       └── AddResult() x N
    │
    ├── Option 2: PrintScanResults()
    │
    ├── Option 3: VerifyAddress()
    │
    ├── Option 4: FreeScanResults()
    │
    ├── Option 5: RefineResults()
    │
    ├── Option 6: WriteToAddress()
    │
    ├── Option 7: GetMemoryRegionInfo()
    │
    ├── Option 8: CompareScanResults()
    │   └── VerifyAddress() x N
    │
    ├── Option 9: SaveResultsToFile()
    │
    ├── Option 10: LoadResultsFromFile()
    │
    └── Option 11: Exit
        ├── FreeScanResults()
        └── CloseHandles()
```

---

## Error Handling

All functions include error checking:

```c
// Typical pattern:
if (!function_result) {
    printf("Operation failed: %lu\n", GetLastError());
    // Handle error
} else {
    // Continue
}
```

---

## Memory Management Checklist

```c
// ALLOCATION
ScanResults* results = InitScanResults();

// USAGE
AddResult(results, addr, value);
PrintScanResults(results, 20);
ScanResults* refined = RefineResults(results, min, max);

// DEALLOCATION
FreeScanResults(results);
FreeScanResults(refined);
// results = NULL;  [Optional good practice]
```

---

## Usage Patterns

### Pattern 1: Simple Search
```c
ScanResults* results = ScanAndFilter(hProcess, targetValue);
PrintScanResults(results, 20);
FreeScanResults(results);
```

### Pattern 2: Narrowing Search
```c
ScanResults* r1 = ScanAndFilter(hProcess, value1);
// [User changes something]
ScanResults* r2 = CompareScanResults(r1, hProcess);
PrintScanResults(r2, 20);
FreeScanResults(r1);
FreeScanResults(r2);
```

### Pattern 3: Address Range Filter
```c
ScanResults* all = ScanAndFilter(hProcess, value);
ScanResults* filtered = RefineResults(all, min, max);
PrintScanResults(filtered, 20);
FreeScanResults(all);
FreeScanResults(filtered);
```

### Pattern 4: Save and Load
```c
// Session 1
ScanResults* r1 = ScanAndFilter(hProcess, value);
SaveResultsToFile(r1, "scan1.txt");
FreeScanResults(r1);

// Session 2
ScanResults* r2 = LoadResultsFromFile("scan1.txt");
ScanResults* changed = CompareScanResults(r2, hProcess);
PrintScanResults(changed, 20);
FreeScanResults(r2);
FreeScanResults(changed);
```

---

## Return Values Summary

| Function | Returns | Success | Failure |
|----------|---------|---------|---------|
| InitScanResults | Pointer | Valid ptr | NULL |
| FreeScanResults | void | - | - |
| AddResult | int | 0 | -1 |
| PrintScanResults | void | - | - |
| ScanProcessMemory | Pointer | Valid ptr | NULL |
| ScanAndFilter | Pointer | Valid ptr | NULL |
| VerifyAddress | int | 1 | 0 |
| GetMemoryRegionInfo | int | 1 | 0 |
| WriteToAddress | int | 1 | 0 |
| CompareScanResults | Pointer | Valid ptr | NULL |
| RefineResults | Pointer | Valid ptr | NULL |
| SaveResultsToFile | int | 1 | 0 |
| LoadResultsFromFile | Pointer | Valid ptr | NULL |

---

## Total Function Count
- **15 specialized functions**
- **2 data structures**
- **4 configuration constants**
- **1 interactive menu system**
- **~750 lines of code**

All fully compiled and error-checked!
