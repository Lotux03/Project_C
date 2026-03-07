# Visual Guide - Memory Search System

## Main Menu Layout

```
═══════════════════════════════════════════════════════════════
                    MEMORY SEARCH TOOL HELP
═══════════════════════════════════════════════════════════════

This is a Cheat Engine-like memory scanner for Windows processes.

BASIC WORKFLOW:
  1. Enter the process path (the program to scan)
  2. Once loaded, press Enter to start the search menu
  3. Perform a scan by entering a value you want to find
  4. The tool will search the process memory for that value
  5. Review results and verify addresses
  6. Optionally write new values to found addresses

ADVANCED FEATURES:
  • Compare multiple scans to find changing values
  • Refine results by address range
  • Query memory region information
  • Save and load results from files
  • Verify addresses still contain expected values

TIPS:
  • Process starts SUSPENDED - you can search before resuming
  • For finding variables, look for their current value
  • Use multiple scans and filtering for better precision
  • Save results between sessions for later analysis

═══════════════════════════════════════════════════════════════
Enter the process path:
C:\Windows\notepad.exe
═══════════════════════════════════════════════════════════════
```

---

## Interactive Menu Layout

```
═══════════════════════════════════════════════════════════════
                  MEMORY SEARCH MENU
═══════════════════════════════════════════════════════════════

  [1] New scan for value
  [2] Display current results
  [3] Verify specific address
  [4] Clear results
  [5] Refine results by address range
  [6] Write value to address
  [7] Get memory region info
  [8] Compare with previous scan (find changes)
  [9] Save results to file
  [10] Load results from file
  [11] Exit search

Enter choice: ▌
```

---

## Scan Result Display

```
═══════════════════════════════════════════════════════════════
Starting memory scan for value: 42 (0x0000002A)
Scanning from 0x00400000 to 0x7FFFFFFF

Scanned: 0x00500000 - Found 3 matches so far
Scanned: 0x00600000 - Found 5 matches so far
Scanned: 0x00700000 - Found 8 matches so far
Scanned: 0x00800000 - Found 12 matches so far
...
Scan complete. Found 42 matches.

═════════════════ SCAN RESULTS ═════════════════
Total matches found: 42
Displaying first 20 results:

  [1] Address: 0x00402000, Value: 42 (0x0000002A)
  [2] Address: 0x00402004, Value: 42 (0x0000002A)
  [3] Address: 0x00402008, Value: 42 (0x0000002A)
  [4] Address: 0x0040200C, Value: 42 (0x0000002A)
  [5] Address: 0x00402010, Value: 42 (0x0000002A)
  [6] Address: 0x00402014, Value: 42 (0x0000002A)
  [7] Address: 0x00402018, Value: 42 (0x0000002A)
  [8] Address: 0x0040201C, Value: 42 (0x0000002A)
  [9] Address: 0x00402020, Value: 42 (0x0000002A)
  [10] Address: 0x00402024, Value: 42 (0x0000002A)
  ... and 32 more results

═══════════════════════════════════════════════════════════════
```

---

## Address Verification

```
═══════════════════════════════════════════════════════════════
[Option 3: Verify Specific Address]

Enter result index to verify (1-42): 5
Address 0x00402010 verified: Value matches (42)

═══════════════════════════════════════════════════════════════

OR (if value changed)

Enter result index to verify (1-42): 3
Address 0x00402008 verification failed: Expected 42, got 99

═══════════════════════════════════════════════════════════════
```

---

## Memory Region Info Display

```
═══════════════════════════════════════════════════════════════
[Option 7: Get Memory Region Info]

Enter address to query (hex, e.g., 0x400000): 0x00402000

═══ Memory Region Info at 0x00402000 ═══
Base Address: 0x00400000
Allocation Base: 0x00400000
Region Size: 8192 bytes (0x2000)
State: Committed
Protect: Read/Write

═══════════════════════════════════════════════════════════════
```

---

## Compare Scans Workflow

```
SCAN 1 (Initial State):
┌──────────────────────────────────┐
│ Address → Value                  │
│ 0x00402000 → 42                  │
│ 0x00402004 → 42                  │
│ 0x00402008 → 42                  │
│ 0x0040200C → 10 (different!)     │
│ 0x00402010 → 42                  │
└──────────────────────────────────┘
         ↓ (User modifies target)
         
SCAN 2 (After Modification):
┌──────────────────────────────────┐
│ Address → Value                  │
│ 0x00402000 → 42  ✓ (unchanged)   │
│ 0x00402004 → 99  ✗ (CHANGED)     │
│ 0x00402008 → 42  ✓ (unchanged)   │
│ 0x0040200C → 10  ✓ (unchanged)   │
│ 0x00402010 → 50  ✗ (CHANGED)     │
└──────────────────────────────────┘
         ↓ COMPARE
         
RESULT (Changed Addresses):
┌──────────────────────────────────┐
│ [1] Address: 0x00402004 → 99     │
│ [2] Address: 0x00402010 → 50     │
│                                  │
│ These are the variables!         │
└──────────────────────────────────┘
```

---

## Write to Memory Flow

```
═══════════════════════════════════════════════════════════════
[Option 6: Write Value to Address]

Enter address to write to (hex, e.g., 0x400000): 0x00402004
Enter value to write: 999

Successfully wrote 999 to address 0x00402004

═══════════════════════════════════════════════════════════════

(Verify it worked)

Enter choice: 3
Enter result index to verify (1-2): 1
Address 0x00402004 verified: Value matches (999)

═══════════════════════════════════════════════════════════════
```

---

## Save/Load Results

```
SAVE OPERATION:
═══════════════════════════════════════════════════════════════
[Option 9: Save Results to File]

Enter filename to save to: my_scan.txt
Results saved to: my_scan.txt

File Contents (my_scan.txt):
───────────────────────────────────────────────────────────────
=== Scan Results ===
Total matches: 2

[1] Address: 0x00402004, Value: 999 (0x000003E7)
[2] Address: 0x00402010, Value: 50 (0x00000032)
───────────────────────────────────────────────────────────────


LOAD OPERATION:
═══════════════════════════════════════════════════════════════
[Option 10: Load Results from File]

Enter filename to load from: my_scan.txt
Loaded 2 results from: my_scan.txt

=== Scan Results ===
Total matches found: 2
Displaying first 20 results:

[1] Address: 0x00402004, Value: 999 (0x000003E7)
[2] Address: 0x00402010, Value: 50 (0x00000032)

═══════════════════════════════════════════════════════════════
```

---

## Refine by Address Range

```
═══════════════════════════════════════════════════════════════
[Option 5: Refine by Address Range]

Current results: 1000 matches

Enter minimum address (hex, e.g., 0x400000): 0x00400000
Enter maximum address (hex, e.g., 0x7FFFFFFF): 0x00500000

Refining 1000 results to addresses between 0x00400000 and 0x00500000
Refined to 47 results.

=== Scan Results ===
Total matches found: 47
Displaying first 20 results:

[1] Address: 0x00402000, Value: 42 (0x0000002A)
[2] Address: 0x00402004, Value: 42 (0x0000002A)
...

═══════════════════════════════════════════════════════════════
```

---

## Memory Region Types (Reference)

```
┌─────────────────────────────────────────────────┐
│       TYPICAL 32-BIT PROCESS MEMORY MAP          │
├─────────────────────────────────────────────────┤
│                                                 │
│  0x00000000-0x0000FFFF │  Reserved (NULL)       │
│                                                 │
│  0x00010000-0x0FFFFFFF │  Available (user)      │
│                        │  ↓                     │
│                        ├─ .EXE loaded here      │
│                        ├─ .DLLs loaded here     │
│                        └─ Code/Data sections    │
│                                                 │
│  0x10000000-0x7FFEFFFF │  Available (heaps)     │
│                        │  ↓                     │
│                        └─ Dynamically allocated │
│                                                 │
│  0x7FFF0000-0x7FFFFFFF │  Stack (grows down)    │
│                        │  ↑                     │
│                        └─ Local variables       │
│                                                 │
│  0x80000000-0xFFFFFFFF │  Kernel space          │
│                        │  (inaccessible)        │
│                                                 │
└─────────────────────────────────────────────────┘
```

---

## Progress Indicators

```
Scan Progress:
┌──────────────────────────────────────────────────┐
│ Starting memory scan for value: 42               │
│ Scanning from 0x00400000 to 0x7FFFFFFF           │
│                                                  │
│ Scanned: 0x00400000 - Found 0 matches           │
│ Scanned: 0x00500000 - Found 3 matches so far    │
│ Scanned: 0x00600000 - Found 5 matches so far    │
│ Scanned: 0x00700000 - Found 8 matches so far    │
│ Scanned: 0x00800000 - Found 10 matches so far   │
│ ...                                              │
│ Scan complete. Found 42 matches.                │
│                                                  │
│ [Progress updates every 1MB]                    │
└──────────────────────────────────────────────────┘
```

---

## Error Handling Examples

```
EXAMPLE 1: Process Not Found
═══════════════════════════════════════════════════════════════
Enter the process path:
C:\NotExist\Program.exe

CreateProcess failed: 2
Failed to create process or thread

═══════════════════════════════════════════════════════════════


EXAMPLE 2: No Results to Verify
═══════════════════════════════════════════════════════════════
[Option 3: Verify]

No results to verify. Perform a scan first.

═══════════════════════════════════════════════════════════════


EXAMPLE 3: Max Results Reached
═══════════════════════════════════════════════════════════════
Scan complete (max results reached)
Warning: Maximum results (10000) reached. Stopping scan.

[Consider refining search with second scan]

═══════════════════════════════════════════════════════════════


EXAMPLE 4: Invalid Input
═══════════════════════════════════════════════════════════════
Enter value to search for: abc

Invalid input.

[Retry with numeric value]

═══════════════════════════════════════════════════════════════
```

---

## Quick Reference - What Each Menu Option Does

```
┌────────────────────────────────────────────────────────────┐
│ OPTION │ PURPOSE              │ INPUT        │ OUTPUT       │
├────────┼──────────────────────┼──────────────┼──────────────┤
│ 1      │ Search memory        │ Value (dec)  │ List of addr │
│ 2      │ Show results         │ None         │ Table/list   │
│ 3      │ Verify address       │ Index (1-N)  │ Match/change │
│ 4      │ Clear memory         │ Confirm      │ Freed        │
│ 5      │ Filter by range      │ Min, Max hex │ Filtered     │
│ 6      │ Modify memory        │ Addr, Value  │ Written      │
│ 7      │ Memory properties    │ Address hex  │ Region info  │
│ 8      │ Find changes         │ None         │ Changed list │
│ 9      │ Export results       │ Filename     │ File saved   │
│ 10     │ Import results       │ Filename     │ List loaded  │
│ 11     │ Exit                 │ Confirm      │ Cleanup      │
└────────┴──────────────────────┴──────────────┴──────────────┘
```

---

## Complete Session Example

```
════════════════════════════════════════════════════════════════
                         START
════════════════════════════════════════════════════════════════

[Help screen displays...]

Enter the process path:
C:\Windows\notepad.exe

[Process loaded in suspended state]

Press Enter to start the search menu: [ENTER]

=== Memory Search Menu ===
Enter choice: 1
Enter value to search for: 100
[Scanning...]
Scan complete. Found 15 matches.

[Displays first 20 results - showing 15]

Enter choice: 6
Enter address to write to: 0x00402000
Enter value to write: 200
Successfully wrote 200 to address 0x00402000

Enter choice: 3
Enter result index to verify (1-15): 1
Address 0x00402000 verified: Value matches (200)

Enter choice: 9
Enter filename to save to: my_scan.txt
Results saved to: my_scan.txt

Enter choice: 11
Exiting memory search.
Closing handles and exiting.

════════════════════════════════════════════════════════════════
                         SUCCESS
════════════════════════════════════════════════════════════════
```

---

This visual guide should help you understand exactly how the system works and what to expect at each step!
