// task.h
#ifndef TASK_H
#define TASK_H

#include <string>
#include <unordered_map>
#include "tlb.h"

class task {
private:
    std::string task_id;
    uint32_t total_pages;
    uint32_t page_hits;
    uint32_t page_misses;
    std::unordered_map<uint32_t, uint32_t> page_table;
    TLB tlb;  // Add TLB for this task

public:
    task(const std::string &id);
    void accessMemory(uint32_t logical_address);
    void printStats() const;
    void access_Memory_neg(uint32_t logical_address);
};

#endif
