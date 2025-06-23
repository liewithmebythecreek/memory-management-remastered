🧠 Virtual Memory Simulator (C++)
Simulates how modern operating systems manage virtual memory using paging, TLBs, and different page table strategies — including single-level and multi-level implementations.

📁 Directory Structure
makefile
Copy
Edit
.
├── include/               # All header files
│   ├── config.h
│   ├── memory_manager.h
│   ├── task.h
│   ├── taskmulti.h
│   ├── tlb.h
├── src/                   # All source files
│   ├── memory_manager.cpp
│   ├── task.cpp
│   ├── taskmulti.cpp
│   ├── io.cpp             # Single-level trace reader
│   ├── iomulti.cpp        # Multi-level trace reader
│   ├── test.cpp           # Trace generator
│   ├── tlb.cpp
├── bin/                   # Compiled executables (output)
├── makefile               # Build system
└── README.md              # This file
💡 Overview
This simulator provides insight into:

✅ Virtual to physical address translation

✅ TLB caching with LRU replacement

✅ Single-Level Page Tables

✅ Two-Level Page Tables (hierarchical)

✅ FIFO page replacement (via global memory manager)

✅ Per-task memory stats and TLB behavior

🧱 Components
🔹 MemoryManager (Singleton)
Manages a pool of physical pages

Supports FIFO replacement

Tracks free and allocated pages

🔹 TLB
Simulates a Translation Lookaside Buffer

LRU cache with 64 entries

Lookup, insert, invalidate, and usage statistics

🔹 task
Per-task memory simulation with single-level page table

Tracks hits/misses and uses TLB

🔹 taskmulti
Per-task memory simulation with two-level page table

Maps logical address → directory index → table index → physical page

Uses TLB and memory manager

🧪 Trace File Format
Each line in the trace:

php-template
Copy
Edit
T<task_id>: 0x<virtual_address>: <size>KB
Example:

makefile
Copy
Edit
T1: 0x00423000: 8KB
T2: 0x00A31000: 4KB
⚙️ Build Instructions
🔧 Prerequisites
g++ with C++11 support

make

Windows (MSYS) or Linux

🏗️ Build Commands
From the root of the project:

bash
Copy
Edit
# Build multi-level simulator (default)
make

# Build single-level simulator
make single

# Generate trace file (trace.txt)
make trace

# Clean build artifacts
make clean
Executables are generated in the bin/ folder:

bin/multilevel_pagetable

bin/single_pagetable

bin/test

▶️ Running the Simulation
🔹 Step 1: Generate Trace File
bash
Copy
Edit
make trace               # Default: 1M lines, 10 tasks
./bin/test 10000 5       # Custom: 10K lines, 5 tasks
🔹 Step 2: Run Simulators
bash
Copy
Edit
# Multi-level page table
./bin/multilevel_pagetable trace.txt

# Single-level page table
./bin/single_pagetable trace.txt
🔍 Internals
🧠 Virtual Address Breakdown (2-Level Page Table)
css
Copy
Edit
[ 10 bits | 10 bits | 12 bits ]
   └────┬────┘  └────┬────┘
 Page Dir     Page Table   Offset
📊 Sample Output
yaml
Copy
Edit
TLB hit for task T1: Logical page 12 -> Physical page 100
Page miss for task T2: Directory 1, Table 45
Allocated physical page number 34
Task T1 - Page Table Hits: 20, Misses: 2
=== TLB Statistics ===
Hits: 15
Misses: 7
Hit Rate: 68.18%
🚧 Limitations
Simulates memory in software — not real OS paging

32-bit address space only

Simplified TLB with no context switching

Windows-specific test.cpp threading (WinAPI) — porting to Linux requires:

pthread.h

Replace CreateThread, WaitForMultipleObjects, CRITICAL_SECTION

👨‍💻 Author
Nitin Kumar
Roll No: 2023AIB1012