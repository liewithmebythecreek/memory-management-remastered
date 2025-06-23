#include "../include/tlb.h"
#include <iostream>

TLB::TLB() : hits(0), misses(0) {}

bool TLB::lookup(uint32_t virtual_page, uint32_t& physical_page) {
    auto it = tlb_map.find(virtual_page);
    if (it != tlb_map.end()) {
        // TLB hit - move to front of LRU list
        lru_list.splice(lru_list.begin(), lru_list, it->second);
        physical_page = it->second->physical_page;
        hits++;
        return true;
    }
    
    misses++;
    return false;
}

void TLB::add(uint32_t virtual_page, uint32_t physical_page) {
    // If the TLB is full, remove the least recently used entry
    if (lru_list.size() >= TLB_SIZE) {
        tlb_map.erase(lru_list.back().virtual_page);
        lru_list.pop_back();
    }
    
    // Add new entry at the front of the LRU list
    TLBEntry entry{virtual_page, physical_page, false};
    lru_list.push_front(entry);
    tlb_map[virtual_page] = lru_list.begin();
}

void TLB::invalidate(uint32_t virtual_page) {
    auto it = tlb_map.find(virtual_page);
    if (it != tlb_map.end()) {
        lru_list.erase(it->second);
        tlb_map.erase(it);
    }
}

void TLB::invalidateAll() {
    lru_list.clear();
    tlb_map.clear();
}

void TLB::getStats(uint32_t& hit_count, uint32_t& miss_count) const {
    hit_count = hits;
    miss_count = misses;
}

void TLB::printStats() const {
    uint32_t total = hits + misses;
    double hit_rate = total > 0 ? (static_cast<double>(hits) / total) * 100 : 0;
    
    std::cout << "\n=== TLB Statistics ===\n";
    std::cout << "Hits: " << hits << "\n";
    std::cout << "Misses: " << misses << "\n";
    std::cout << "Hit Rate: " << hit_rate << "%\n";
}
