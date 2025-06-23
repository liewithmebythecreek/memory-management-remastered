#include "memory_manager.h"
#include "config.h"
#include <stdexcept>

MemoryManager::MemoryManager() {
    total_pages = PHYSICAL_MEMORY_SIZE / (MIN_PAGE_SIZE_KB * 1024);
    for (uint32_t i = 0; i < total_pages; ++i) {
        free_pages.insert(i);
    }
}

MemoryManager& MemoryManager::getInstance() {
    static MemoryManager instance;
    return instance;
}

uint32_t MemoryManager::allocatePage() {
    if (free_pages.empty()) {
        uint32_t page_to_replace;
        bool replaced = false;
        while (!page_allocation_order.empty()) {
            page_to_replace = page_allocation_order.front();
            page_allocation_order.pop();

            if (allocated_pages.count(page_to_replace)) {
                // This page is allocated, so we can replace it.
                allocated_pages.erase(page_to_replace);
                free_pages.insert(page_to_replace);
                replaced = true;
                std::cout << "Replaced page " << page_to_replace << std::endl;
                break;
            }
        }
        if (!replaced) {
            throw std::runtime_error("Out of physical memory and no pages to replace!");
        }
    }
    auto it = free_pages.begin();
    uint32_t page_number = *it;
    free_pages.erase(it);
    allocated_pages.insert(page_number);
    return page_number;
}

void MemoryManager::deallocatePage(uint32_t page_number) {
    if (allocated_pages.count(page_number) == 0) return;
    allocated_pages.erase(page_number);
    free_pages.insert(page_number);
}

uint32_t MemoryManager::getFreePageCount() const {
    return free_pages.size();
}

uint32_t MemoryManager::getAllocatedPageCount() const {
    return allocated_pages.size();
}
