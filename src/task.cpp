// task.cpp
#include "task.h"
#include "../include/config.h"
#include "../include/memory_manager.h"
#include <iostream>

task::task(const std::string &id) : task_id(id) {
    total_pages = PHYSICAL_MEMORY_SIZE / (MIN_PAGE_SIZE_KB * 1024);
    page_hits = 0;
    page_misses = 0;
}

void task::accessMemory(uint32_t logical_address) {
    uint32_t page_size = MIN_PAGE_SIZE_KB * 1024;
    uint32_t logical_page_number = logical_address / page_size;
    uint32_t physical_page;

    // First, try to find the mapping in the TLB
    if (tlb.lookup(logical_page_number, physical_page)) {
        page_hits++;
        std::cout << "TLB hit for task " << task_id 
                  << ": Logical page " << logical_page_number 
                  << " -> Physical page " << physical_page << std::endl;
        return;
    }

    // TLB miss - need to check page table
    if (page_table.find(logical_page_number) != page_table.end()) {
        page_hits++;
        physical_page = page_table[logical_page_number];
        std::cout << "Page hit for task " << task_id 
                  << ": Page number " << logical_page_number << std::endl;
    } else {
        page_misses++;
        std::cout << "Page miss for task " << task_id 
                  << ": Page number " << logical_page_number << std::endl;
        physical_page = MemoryManager::getInstance().allocatePage();
        page_table[logical_page_number] = physical_page;
        std::cout << "Allocated physical page number " << physical_page
                  << " for logical page number " << logical_page_number << std::endl;
    }

    // Add the mapping to the TLB
    tlb.add(logical_page_number, physical_page);
}

void task::printStats() const {
    std::cout << "Task " << task_id << " - Page Table Hits: " << page_hits
              << ", Page Table Misses: " << page_misses << "\n";
    tlb.printStats();
}

void task::access_Memory_neg(uint32_t logical_address) {
    uint32_t page_size = MIN_PAGE_SIZE_KB * 1024;
    uint32_t logical_page_number = logical_address / page_size;
    
    if (page_table.find(logical_page_number) != page_table.end()) {
        uint32_t physical_page_number = page_table[logical_page_number];
        MemoryManager::getInstance().deallocatePage(physical_page_number);
        page_table.erase(logical_page_number);
        
        // Invalidate the TLB entry
        tlb.invalidate(logical_page_number);
        
        std::cout << task_id << " - Deallocated virtual page " << logical_page_number
                  << " (physical page " << physical_page_number << ")\n";
    } else {
        std::cout << "Page is not allocated" << std::endl;
    }
}
