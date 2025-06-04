#ifndef TASK_H
#define TASK_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include "memory_manager.h"
#include "config.h"

/**
 * PageTableType: choose one of three page table implementations.
 */
enum class PageTableType {
    MAP,
    SINGLE,
    TWO_LEVEL
};

/**
 * Task:
 *   - Each Task instance has its own page table (of chosen type).
 *   - Processes logical address requests (with size), accesses pages.
 *   - On page miss, requests a new physical frame from MemoryManager.
 *   - Tracks number of page hits and misses.
 *   - Computes page table memory usage.
 */
class Task {
public:
    /**
     * Constructor.
     * @param id: unique Task identifier (e.g. "T1").
     * @param pt_type: MAP, SINGLE, or TWO_LEVEL.
     * @param page_size_bytes: must match MemoryManager’s page size.
     * @param mem_mgr: reference to the global MemoryManager instance.
     */
    Task(const std::string &id,
         PageTableType pt_type,
         std::uint64_t page_size_bytes,
         MemoryManager &mem_mgr);

    ~Task();

    /**
     * Process a memory request: given a logical starting address and size (bytes),
     * calculate which pages are accessed. For each page:
     *   - If page is already in page table, count as hit;
     *   - Otherwise, count as miss, allocate a new physical frame, and insert.
     * Note: Assumes addresses and sizes are aligned to the page boundary.
     * @param logical_address: full 64-bit logical address.
     * @param size_bytes: size of allocation in bytes (e.g., 16384 for 16KB).
     */
    void process_request(std::uint64_t logical_address, std::uint64_t size_bytes);

    /** Get total page hits for this Task. */
    std::uint64_t get_hits() const;

    /** Get total page misses for this Task. */
    std::uint64_t get_misses() const;

    /**
     * Get memory used by this Task’s page table (in bytes).
     * MAP: number of entries * (sizeof(uint64_t key) + sizeof(uint64_t value)).
     * SINGLE: number of virtual pages * sizeof(int64_t entry).
     * TWO_LEVEL: top-level pointers + allocated second-level arrays.
     */
    std::uint64_t get_page_table_memory() const;

private:
    std::string id;
    PageTableType pt_type;
    std::uint64_t page_size;
    MemoryManager &memory_manager;

    std::uint64_t hits;
    std::uint64_t misses;

    // For MAP implementation:
    std::unordered_map<std::uint64_t, std::uint64_t> page_map; // VPN -> PFN

    // For SINGLE implementation:
    std::vector<std::int64_t> single_pt; // size = num_virtual_pages; -1 if invalid

    // For TWO_LEVEL implementation:
    std::vector<std::int64_t *> two_level_pt; // size = level1_size; each element is nullptr or points to array of size level2_size
    std::uint64_t allocated_second_levels;     // count of allocated second-level tables

    // Internal helpers:
    void map_process(std::uint64_t vpn);
    void single_process(std::uint64_t vpn);
    void two_level_process(std::uint64_t vpn);

    std::uint64_t virtual_pages_count() const;
    std::uint64_t level1_size() const;
    std::uint64_t level2_size() const;
};

#endif // TASK_H
