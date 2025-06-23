#ifndef TASKMULTI_H
#define TASKMULTI_H

#include <string>
#include <unordered_map>
#include "tlb.h"

class taskmulti {
private:
    std::string task_id;
    uint32_t total_pages;
    uint32_t page_hits;
    uint32_t page_misses;
    
    // Two-level page table structure
    // First level: page directory (maps directory index to page table)
    // Second level: page table (maps table index to physical page number)
    std::unordered_map<uint32_t, std::unordered_map<uint32_t, uint32_t>> page_directory;
    
    // TLB for this task
    TLB tlb;

public:
    taskmulti(const std::string &id);
    void accessMemory(uint32_t logical_address);
    void printStats() const;
    void access_Memory_neg(uint32_t logical_address);
};

#endif // TASKMULTI_H 