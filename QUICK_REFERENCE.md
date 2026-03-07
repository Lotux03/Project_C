# Quick Reference Guide - Memory Search System

## Getting Started (3 Steps)

1. **Run the program**
   ```
   C:\Users\Hella\source\repos\Project_C\x64\Release\Project_C.exe
   ```

2. **Enter process path when prompted**
   ```
   C:\Users\Hella\Downloads\Project_C.exe
   ```

3. **Press Enter and use the interactive menu**

---

## Core Functions Quick Reference

### Search Functions
| Function | Purpose |
|----------|---------|
| `ScanProcessMemory()` | Main scanning engine |
| `ScanAndFilter()` | Quick scan wrapper |
| `CompareScanResults()` | Find changed values |
| `RefineResults()` | Filter by address range |

### Verification Functions
| Function | Purpose |
|----------|---------|
| `VerifyAddress()` | Check if address still has value |
| `GetMemoryRegionInfo()` | Query memory properties |

### Modification Functions
| Function | Purpose |
|----------|---------|
| `WriteToAddress()` | Write value to memory |

### File Functions
| Function | Purpose |
|----------|---------|
| `SaveResultsToFile()` | Export results |
| `LoadResultsFromFile()` | Import results |

---

## Menu Commands Quick Reference

```
1. New scan for value      → Find all instances of a value
2. Display current results → Show first 20 matches
3. Verify specific address → Check if address still matches
4. Clear results           → Free memory
5. Refine by address range → Filter results
6. Write value to address  → Modify process memory
7. Get memory region info  → Query memory details
8. Compare with previous   → Find value changes
9. Save results to file    → Export for later
10. Load results from file  → Import previous scan
11. Exit search             → Cleanup and exit
```

---

## Common Workflows

### Find a Game Variable
```
Menu 1 → Enter current value
Menu 8 → Change value in game
Menu 2 → View results
Menu 3 → Verify one match
Menu 6 → Write new value and test
```

### Narrow Down Large Result Set
```
Menu 1 → Scan with initial value (5000 results)
Menu 1 → Scan with second value (100 results)
Menu 5 → Filter to DLL range (10 results)
Menu 3 → Verify each one
```

### Save and Load Progress
```
Session 1:
  Menu 1 → Scan
  Menu 9 → Save "my_scan.txt"
  
Session 2:
  Menu 10 → Load "my_scan.txt"
  Menu 8 → Compare to find changes
```

---

## Data Formats

### Address Format (Hex)
```
0x00400000
0x7FFFFFFF
0x12AB5678
```

### Value Format (Decimal)
```
42
1337
2147483647
```

### File Format (Save/Load)
```
=== Scan Results ===
Total matches: 5

[1] Address: 0x00ABC123, Value: 42 (0x0000002A)
[2] Address: 0x00ABC456, Value: 42 (0x0000002A)
```

---

## Useful Address Ranges (32-bit Windows)

| Range | Purpose |
|-------|---------|
| 0x00400000 - 0x00500000 | Program code section |
| 0x00500000 - 0x00600000 | Program data section |
| 0x00610000+ | Heap memory |
| 0x7FFF0000+ | Stack (varies) |

---

## Tips & Tricks

### Finding Variables Faster
- Don't search for common values (0, 1, 100)
- Look for specific in-game values
- Use multiple scans + compare method
- Refine results by address range

### Verifying Addresses
- Check memory region info first
- Verify address before writing
- Always test with safe values first
- Check if address persists between runs

### Memory Safety
- Always verify before writing
- Don't modify system DLLs
- Check address permissions first
- Make backups before major changes

### Performance
- Large result sets slow down compare
- Clear unused results to free RAM
- Save results to avoid re-scanning
- Use address range filtering

---

## Error Messages & Solutions

### "ReadProcessMemory failed: 2"
**Cause:** File not found or wrong path
**Solution:** Check process path is correct

### "ReadProcessMemory failed: 5"
**Cause:** Access denied
**Solution:** Run as administrator or check permissions

### "Maximum results (10000) reached"
**Cause:** Too many matches found
**Solution:** Refine search with second scan or use address filter

### "No current results"
**Cause:** No scan performed yet
**Solution:** Select option 1 to perform a scan first

---

## Example Session

```
Enter the process path:
C:\Windows\notepad.exe

Created ProcessInfo: ...
Process Created: notepad.exe, ...

Starting interactive memory search mode.
Note: Process is currently SUSPENDED. Resume it if needed before searching.
You can still search memory while paused, but the values won't change.

Press Enter to start the search menu: 

=== Memory Search Menu ===
1. New scan for value
2. Display current results
... (11 options total)
Enter choice: 1

Enter value to search for: 42
Starting memory scan for value: 42 (0x0000002A)
Scanning from 0x00400000 to 0x7FFFFFFF
Scanned: 0x00500000 - Found 3 matches so far
Scanned: 0x00600000 - Found 5 matches so far
...
Scan complete. Found 42 matches.

=== Scan Results ===
Total matches found: 42
Displaying first 20 results:
[1] Address: 0x00402000, Value: 42 (0x0000002A)
[2] Address: 0x00402004, Value: 42 (0x0000002A)
...
```

---

## Integration Notes

- Process starts **SUSPENDED** for safe searching
- Use existing thread handle management
- Memory reads are non-destructive
- Writes require explicit user confirmation
- All handles properly cleaned up on exit

---

## What's Next?

The system is ready for GUI conversion with:
- ✓ Solid search algorithms
- ✓ Robust error handling  
- ✓ Memory management
- ✓ File I/O support
- ✓ Address verification
- ✓ Change tracking

Consider Windows Forms or WPF for GUI implementation!

---

**Created:** Part of Project_C Enhancement
**Author:** Memory Search System Implementation
**Status:** Complete and functional
**Ready for:** GUI integration or direct usage
