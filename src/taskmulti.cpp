#include "taskmulti.h"
#include "../include/config.h"
#include "../include/memory_manager.h"
#include "../include/tlb.h"
#include <iostream>
#include <unordered_map>

taskmulti::taskmulti(const std::string &id) : task_id(id) {
    total_pages = PHYSICAL_MEMORY_SIZE / (MIN_PAGE_SIZE_KB * 1024);
    page_hits = 0;
    page_misses = 0;
}

void taskmulti::accessMemory(uint32_t logical_address) {
    // Calculate page directory index and page table index
    uint32_t page_directory_index = (logical_address >> 22) & 0x3FF;  // Bits 31-22
    uint32_t page_table_index = (logical_address >> 12) & 0x3FF;      // Bits 21-12
    
    // Combine directory and table indices to form virtual page number
    uint32_t virtual_page = (page_directory_index << 10) | page_table_index;
    uint32_t physical_page;

    // First, try to find the mapping in the TLB
    if (tlb.lookup(virtual_page, physical_page)) {
        page_hits++;
        std::cout << "TLB hit for task " << task_id 
                  << ": Virtual page " << virtual_page 
                  << " -> Physical page " << physical_page << std::endl;
        return;
    }

    // TLB miss - need to check page table
    if (page_directory.find(page_directory_index) == page_directory.end()) {
        // Create new page table for this directory entry
        page_directory[page_directory_index] = std::unordered_map<uint32_t, uint32_t>();
    }

    // Check if page table entry exists
    if (page_directory[page_directory_index].find(page_table_index) != page_directory[page_directory_index].end()) {
        page_hits++;
        physical_page = page_directory[page_directory_index][page_table_index];
        std::cout << "Page hit for task " << task_id 
                  << ": Directory index " << page_directory_index 
                  << ", Table index " << page_table_index << std::endl;
    } else {
        page_misses++;
        std::cout << "Page miss for task " << task_id 
                  << ": Directory index " << page_directory_index 
                  << ", Table index " << page_table_index << std::endl;
        
        // Allocate new physical page
        physical_page = MemoryManager::getInstance().allocatePage();
        page_directory[page_directory_index][page_table_index] = physical_page;
        
        std::cout << "Allocated physical page number " << physical_page
                  << " for directory index " << page_directory_index 
                  << ", table index " << page_table_index << std::endl;
    }

    // Add the mapping to the TLB
    tlb.add(virtual_page, physical_page);
}

void taskmulti::printStats() const {
    std::cout << "Task " << task_id << " - Page Table Hits: " << page_hits
              << ", Page Table Misses: " << page_misses << "\n";
    tlb.printStats();
}

void taskmulti::access_Memory_neg(uint32_t logical_address) {
    uint32_t page_directory_index = (logical_address >> 22) & 0x3FF;
    uint32_t page_table_index = (logical_address >> 12) & 0x3FF;
    uint32_t virtual_page = (page_directory_index << 10) | page_table_index;

    if (page_directory.find(page_directory_index) != page_directory.end() &&
        page_directory[page_directory_index].find(page_table_index) != page_directory[page_directory_index].end()) {
        
        uint32_t physical_page_number = page_directory[page_directory_index][page_table_index];
        MemoryManager::getInstance().deallocatePage(physical_page_number);
        page_directory[page_directory_index].erase(page_table_index);
        
        // Invalidate the TLB entry
        tlb.invalidate(virtual_page);
        
        // If the page table is empty, remove it from the directory
        if (page_directory[page_directory_index].empty()) {
            page_directory.erase(page_directory_index);
        }
        
        std::cout << task_id << " - Deallocated virtual page (dir: " << page_directory_index 
                  << ", table: " << page_table_index << ", physical: " << physical_page_number << ")\n";
    } else {
        std::cout << "Page is not allocated" << std::endl;
    }
}
