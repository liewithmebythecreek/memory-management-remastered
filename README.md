# Virtual Memory Simulator

A C++ implementation of virtual memory management simulation featuring both single-level and multi-level page tables with TLB caching.

## Description

This project simulates how modern operating systems manage virtual memory using:

- ✅ Single-Level Page Table
- ✅ Multi-Level Page Table (2-level)
- ✅ Translation Lookaside Buffer (TLB) for faster translation

The simulation models the process of mapping logical/virtual addresses to physical addresses using paging and TLB caching, providing insights into memory management mechanisms.

## Project Structure

```
.
├── include/               # Header files
│   ├── memory_manager.h   # Memory management interface
│   ├── tlb.h              # TLB interface
│   ├── task.h             # Single-level task interface
│   ├── taskmulti.h        # Multi-level task interface
│   └── config.h           # Configuration constants
├── src/                   # Source implementation files
│   ├── memory_manager.cpp # Memory allocation implementation
│   ├── tlb.cpp            # TLB implementation
│   ├── io.cpp             # Single-level I/O operations
│   ├── task.cpp           # Single-level task implementation
│   ├── iomulti.cpp        # Multi-level I/O operations
│   └── taskmulti.cpp      # Multi-level task implementation
├── test.cpp               # Trace file generator
├── Makefile               # Build automation
├── bin/                   # Compiled executables
│   ├── single_pagetable   # Single-level executable
│   ├── multilevel_pagetable # Multi-level executable
│   └── test               # Trace generator executable
├── obj/                   # Object files
└── README.md              # This file
```

## Requirements

- g++ compiler (C++11 support)
- Make (for building)
- Windows or Linux

## Build Instructions

1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd memory-manager
   ```

2. Build the project:

   ```bash
   # Build multilevel page table (default)
   make

   # Build single-level page table
   make single

   # Generate trace file
   make trace

   # Clean build files
   make clean
   ```

## Running the Simulator

1. Generate a trace file (optional, uses defaults):
   ```bash
   make trace
   ```

2. Or generate a custom trace file:
   ```bash
   bin/test <total_lines> <num_tasks>
   ```

3. Run the simulators:
   ```bash
   # Run multilevel page table simulator
   bin/multilevel_pagetable trace.txt

   # Run single-level page table simulator
   bin/single_pagetable trace.txt
   ```

## Features

- 🧠 TLB cache simulation with hit/miss tracking
- 🧮 Physical memory page allocation and deallocation
- 📊 Per-task statistics (page hits/misses, TLB hits/misses)
- 🧵 Multi-threaded trace file generation
- 🛠️ Cross-platform build system (Windows/Linux)

## Trace File Format

The trace file contains memory access patterns in the format:
```
T<task_id>: 0x<address>: <size>KB
```

Example:
```
T1: 0x00423000: 8KB
T2: 0x00A31000: 4KB
```

## Implementation Details

### Multi-level Page Table

- Two-level page table structure
- Page directory (10 bits) + Page table (10 bits) + Offset (12 bits)
- 4KB page size
- TLB caching for faster translation

### Single-level Page Table

- Direct mapping from virtual to physical pages
- 4KB page size
- TLB caching for faster translation

## Known Limitations

- This is a **simulation**, not actual OS memory management
- Physical memory is simulated in software
- TLB is simplified to basic key-value storage
- Limited to 32-bit addressing
- **Platform Dependency**: The trace generator uses platform-specific threading. On Linux, replace Windows threading with pthreads.

## Author

- **Nitin Kumar** (Roll No: 2023AIB1012)

_AI Lab 3 Coursework_

## License

This project is part of the AI Lab 3 coursework.
