#ifndef CONFIG_H
#define CONFIG_H

// Minimum page size (for alignment) in KB
#define MIN_PAGE_SIZE_KB 8

// Page sizes supported (in KB) - can be used for multiple page size simulations
#define PAGE_SIZE_KB_1 1
#define PAGE_SIZE_KB_4 4
#define PAGE_SIZE_KB_16 16

// Virtual and physical memory sizes (in bytes)
#define VIRTUAL_MEMORY_SIZE (1ULL << 32)   // 4 GB virtual memory
#define PHYSICAL_MEMORY_SIZE (1ULL << 30)  // 1 GB physical memory

// Number of entries per level in a multi-level page table
#define LEVEL1_ENTRIES 1024
#define LEVEL2_ENTRIES 1024

// Base addresses for segments (hex format for clarity)
#define TEXT_BASE_ADDR     0x00000000
#define DATA_BASE_ADDR     0x10000000
#define STACK_BASE_ADDR    0x70000000
#define HEAP_BASE_ADDR     0x40000000
#define SHARED_LIB_ADDR    0x60000000

// Number of tasks to simulate in test module
#define MAX_TASKS 10

#endif // CONFIG_H
