#ifndef TLB_H
#define TLB_H

#include <unordered_map>
#include <list>
#include <cstdint>
#include <cstddef>

class TLB {
private:
    struct TLBEntry {
        uint32_t virtual_page;
        uint32_t physical_page;
        bool dirty;
    };

    // TLB size (number of entries)
    static const size_t TLB_SIZE = 64;  // Typical TLB sizes range from 32 to 256 entries
    
    // Cache structure using LRU (Least Recently Used) replacement policy
    std::list<TLBEntry> lru_list;
    std::unordered_map<uint32_t, std::list<TLBEntry>::iterator> tlb_map;
    
    // Statistics
    uint32_t hits;
    uint32_t misses;

public:
    TLB();
    
    // Look up a virtual page number in the TLB
    bool lookup(uint32_t virtual_page, uint32_t& physical_page);
    
    // Add a new mapping to the TLB
    void add(uint32_t virtual_page, uint32_t physical_page);
    
    // Invalidate a specific entry
    void invalidate(uint32_t virtual_page);
    
    // Invalidate all entries
    void invalidateAll();
    
    // Get hit rate statistics
    void getStats(uint32_t& hit_count, uint32_t& miss_count) const;
    
    // Print TLB statistics
    void printStats() const;
};

#endif // TLB_H 