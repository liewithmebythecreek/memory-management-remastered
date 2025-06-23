#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <unordered_set>
#include <cstdint>
#include <iostream>
#include <queue>
class MemoryManager {
private:
    std::unordered_set<uint32_t> free_pages;
    std::unordered_set<uint32_t> allocated_pages;
    std::queue<uint32_t> page_allocation_order;
    uint32_t total_pages;

    MemoryManager(); // constructor

public:
    static MemoryManager& getInstance(); // singleton

    MemoryManager(const MemoryManager&) = delete;
    void operator=(const MemoryManager&) = delete;

    uint32_t allocatePage();
    void deallocatePage(uint32_t page_number);
    uint32_t getFreePageCount() const;
    uint32_t getAllocatedPageCount() const;
};

#endif
