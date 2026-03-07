# Memory Search System - Complete Implementation

## Overview
A comprehensive Cheat Engine-like memory scanner has been built into your Project_C application. This system allows you to search through process memory, find specific values, track memory changes, and even write values back to those addresses.

---

## Core Components

### 1. Data Structures
**MemoryResult** - Stores a single memory location match
- `DWORD address` - Memory address where value was found
- `DWORD value` - The value found at that address

**ScanResults** - Container for multiple search results
- `MemoryResult* results` - Dynamic array of results
- `int count` - Number of results found
- `int capacity` - Maximum capacity (10,000)

### 2. Memory Management Functions

#### `InitScanResults()`
- Allocates memory for a new ScanResults structure
- Pre-allocates buffer for up to 10,000 results
- Returns pointer to initialized ScanResults

#### `FreeScanResults(ScanResults* pResults)`
- Deallocates ScanResults and its internal buffer
- Safe cleanup to prevent memory leaks

#### `AddResult(ScanResults* pResults, DWORD address, DWORD value)`
- Adds a new match to the results array
- Returns 0 on success, -1 if capacity exceeded

---

## Memory Scanning Functions

### `ScanProcessMemory(HANDLE hProcess, DWORD searchValue, DWORD startAddr, DWORD endAddr)`
**Core scanning function**
- Scans process memory in 4KB pages for a specific DWORD value
- Handles inaccessible memory regions gracefully
- Provides progress indicators during scan
- Returns ScanResults pointer with all matches found

**Features:**
- Efficient page-based reading (4096 bytes at a time)
- Skips inaccessible memory regions
- Progress updates every 1MB
- Stops automatically if max results reached

### `ScanAndFilter(HANDLE hProcess, DWORD searchValue)`
- Wrapper function for full process memory scan
- Scans from 0x00400000 to 0x7FFFFFFF (standard user space)
- Simple interface for single-value searches

---

## Verification & Inspection Functions

### `VerifyAddress(HANDLE hProcess, DWORD address, DWORD expectedValue)`
- Reads a specific address and compares to expected value
- Reports match or mismatch
- Useful for confirming addresses still contain expected data

### `GetMemoryRegionInfo(HANDLE hProcess, DWORD address)`
- Queries Windows memory information using VirtualQueryEx
- Returns:
  - Base address of memory region
  - Allocation base
  - Region size
  - Memory state (Committed/Reserved/Free)
  - Memory protection (Execute/Read/Write/etc.)

---

## Memory Modification Functions

### `WriteToAddress(HANDLE hProcess, DWORD address, DWORD value)`
- Writes a DWORD value to a process memory address
- Uses WriteProcessMemory Windows API
- Provides status feedback on success/failure

**WARNING:** Only use on addresses you understand!

---

## Analysis Functions

### `CompareScanResults(ScanResults* pOldResults, HANDLE hProcess)`
- Compares previous scan results with current memory state
- Identifies which addresses have changed values
- Returns new ScanResults containing only changed addresses
- Useful for narrowing down variable locations

**Workflow:**
1. Scan for initial value
2. Change something in the target process
3. Use compare to find what changed

### `RefineResults(ScanResults* pCurrentResults, DWORD minAddr, DWORD maxAddr)`
- Filters existing results by address range
- Keeps only results within specified bounds
- Reduces false positives in large result sets

**Use case:** If you have 5000 results, narrow down to a specific DLL range

---

## File Operations

### `SaveResultsToFile(ScanResults* pResults, const char* filename)`
- Exports scan results to a text file
- Format includes address and value for each match
- Allows saving progress between sessions

### `LoadResultsFromFile(const char* filename)`
- Imports previously saved results
- Parses the save file format
- Returns loaded ScanResults for further analysis

---

## Interactive Menu System

The `InteractiveMemorySearch(HANDLE hProcess)` function provides a complete menu-driven interface with 11 options:

### Menu Options

**1. New scan for value**
- Enter a DWORD value to search for
- Automatically saves previous results for comparison
- Performs full memory scan

**2. Display current results**
- Shows first 20 results (or fewer if less exist)
- Displays address in hex and value in decimal/hex

**3. Verify specific address**
- Select a result by index
- Re-reads the address to verify current value
- Reports if value matches or has changed

**4. Clear results**
- Frees current results from memory
- Useful to save RAM with large result sets

**5. Refine results by address range**
- Filter current results by min/max address
- Enter addresses in hex format (0x400000)
- Keeps only results within range

**6. Write value to address**
- Enter target address (hex format)
- Enter new value (decimal)
- Modifies target process memory

**7. Get memory region info**
- Query memory characteristics at any address
- Shows protection flags, region size, state
- Helps identify code sections, heaps, etc.

**8. Compare with previous scan**
- Requires at least 2 scans performed
- Shows only addresses that changed
- Excellent for finding game variables

**9. Save results to file**
- Export current results to a text file
- Specify filename when prompted
- Results can be loaded later

**10. Load results from file**
- Import previously saved scan results
- Parses text file format
- Restores complete result set

**11. Exit search**
- Properly cleans up all allocated memory
- Closes process and thread handles
- Returns to main program exit

---

## Workflow Examples

### Example 1: Finding a Value
```
1. Load Process_C.exe
2. Scan for value (e.g., 42)
3. Change the value in the program
4. Use "Compare" to find changed addresses
5. Verify the address matches your expectation
6. Write a new value to test
```

### Example 2: Narrowing Down Results
```
1. Scan for common value (e.g., 0)
2. Results: 5000+ matches
3. Scan again for less common value
4. Results narrowed to 100 matches
5. Use "Refine by address" to focus on .data section
6. Final results: 5-10 addresses
```

### Example 3: Persistent Analysis
```
1. Perform scans in session 1
2. Save results to "scan1.txt"
3. Close program
4. Launch program again in session 2
5. Load "scan1.txt"
6. Use "Compare" to see what addresses still exist
7. Verify historical data persistence
```

---

## Technical Details

### Memory Scanning Strategy
- **Page Size:** 4KB (standard Windows virtual memory page)
- **Buffer Size:** 4096 bytes per read
- **Search Range:** 0x00400000 to 0x7FFFFFFF (user-mode space)
- **Max Results:** 10,000 matches per scan
- **Data Type:** 32-bit unsigned integers (DWORD)

### Error Handling
- Gracefully skips inaccessible memory regions
- Validates all input values
- Checks for NULL pointers before operations
- Provides descriptive error messages with Windows error codes

### Memory Safety
- All dynamically allocated memory is tracked
- Proper cleanup on exit or error
- Buffer overflow protection with capacity checks
- Safe string operations with length validation

---

## Integration with Main Program

The memory search system is fully integrated with the existing code:

1. **Process Launch:** Uses existing `StartPausedProc()` function
2. **Memory Access:** Uses Windows ReadProcessMemory/WriteProcessMemory APIs
3. **Handle Management:** Leverages existing `CloseHandles()` function
4. **Error Reporting:** Consistent with existing error messages

The process starts in SUSPENDED state, allowing you to:
- Search memory while paused
- Resume process to see memory changes
- Re-scan after resumption
- Track which values change during execution

---

## Next Steps (For GUI Implementation)

This memory search system provides a solid foundation for a GUI. Consider:

1. **Replace console menu with GUI controls**
2. **Add real-time memory monitoring**
3. **Visual address/value display tables**
4. **Drag-and-drop process selection**
5. **Multiple concurrent scans**
6. **Memory map visualization**
7. **Breakpoint integration**
8. **Watch windows for specific addresses**

---

## Constants You Can Modify

In `main.c`, consider these adjustments:

```c
#define MAX_RESULTS 10000      // Increase for more matches
#define SCAN_PAGE_SIZE 4096    // Standard page size
#define MIN_ADDRESS 0x00400000 // Start of scan
#define MAX_ADDRESS 0x7FFFFFFF // End of scan
```

---

## Summary

You now have a complete, production-ready memory scanning engine that:
✓ Searches process memory efficiently
✓ Tracks and refines results
✓ Verifies address validity
✓ Compares scans for changes
✓ Saves/loads results
✓ Modifies memory values
✓ Provides detailed memory information
✓ Handles errors gracefully
✓ Operates as a complete interactive system

Ready for GUI conversion!
