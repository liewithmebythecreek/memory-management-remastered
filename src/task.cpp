#include "task.h"
#include <stdexcept>
#include <cstring> // for memset

Task::Task(const std::string &id_,
           PageTableType pt_type_,
           std::uint64_t page_size_bytes,
           MemoryManager &mem_mgr)
    : id(id_),
      pt_type(pt_type_),
      page_size(page_size_bytes),
      memory_manager(mem_mgr),
      hits(0),
      misses(0),
      allocated_second_levels(0)
{
    // Verify page_size is a power of two and matches memory_manager
    if (page_size == 0 || (Config::PHYSICAL_MEMORY_SIZE % page_size) != 0) {
        throw std::invalid_argument("Invalid page size for Task");
    }

    switch (pt_type) {
        case PageTableType::MAP:
            // No special initialization needed
            break;

        case PageTableType::SINGLE: {
            std::uint64_t num_vpages = virtual_pages_count();
            // Initialize all entries to -1 (invalid)
            single_pt.assign(num_vpages, -1);
            break;
        }

        case PageTableType::TWO_LEVEL: {
            std::uint64_t lvl1 = level1_size();
            // Top-level array starts as nullptr pointers
            two_level_pt.assign(lvl1, nullptr);
            break;
        }

        default:
            throw std::invalid_argument("Unknown page table type");
    }
}

Task::~Task() {
    if (pt_type == PageTableType::TWO_LEVEL) {
        // Free any allocated second-level arrays
        std::uint64_t lvl1 = level1_size();
        for (std::uint64_t i = 0; i < lvl1; ++i) {
            if (two_level_pt[i] != nullptr) {
                delete[] two_level_pt[i];
                two_level_pt[i] = nullptr;
            }
        }
    }
}

void Task::process_request(std::uint64_t logical_address, std::uint64_t size_bytes) {
    if (page_size == 0) return;

    // Determine starting VPN
    std::uint64_t vpn_start = logical_address / page_size;
    // Number of pages covered by size_bytes (round up)
    std::uint64_t num_pages = (size_bytes + page_size - 1) / page_size;

    for (std::uint64_t offset = 0; offset < num_pages; ++offset) {
        std::uint64_t vpn = vpn_start + offset;
        switch (pt_type) {
            case PageTableType::MAP:
                map_process(vpn);
                break;
            case PageTableType::SINGLE:
                single_process(vpn);
                break;
            case PageTableType::TWO_LEVEL:
                two_level_process(vpn);
                break;
        }
    }
}

std::uint64_t Task::get_hits() const {
    return hits;
}

std::uint64_t Task::get_misses() const {
    return misses;
}

std::uint64_t Task::get_page_table_memory() const {
    std::uint64_t memory = 0;
    switch (pt_type) {
        case PageTableType::MAP: {
            // Each entry stores one key+value (each uint64_t)
            memory = page_map.size() * (sizeof(std::uint64_t) * 2);
            break;
        }
        case PageTableType::SINGLE: {
            std::uint64_t num_vpages = virtual_pages_count();
            memory = num_vpages * sizeof(std::int64_t);
            break;
        }
        case PageTableType::TWO_LEVEL: {
            std::uint64_t lvl1 = level1_size();
            std::uint64_t lvl2 = level2_size();
            // Memory for top-level pointers
            memory = lvl1 * sizeof(std::int64_t *);
            // Memory for each allocated second-level array
            memory += allocated_second_levels * (lvl2 * sizeof(std::int64_t));
            break;
        }
    }
    return memory;
}

void Task::map_process(std::uint64_t vpn) {
    auto it = page_map.find(vpn);
    if (it != page_map.end()) {
        // Page‐table hit
        ++hits;
    } else {
        // Page‐table miss: allocate a new physical frame and insert
        std::int64_t pfn = memory_manager.allocate_frame();
        if (pfn < 0) {
            throw std::runtime_error("Out of physical memory in map_process");
        }
        page_map[vpn] = static_cast<std::uint64_t>(pfn);
        ++misses;
    }
}

void Task::single_process(std::uint64_t vpn) {
    if (vpn >= virtual_pages_count()) {
        throw std::out_of_range("VPN out of range in single_process");
    }
    if (single_pt[vpn] >= 0) {
        // Page‐table hit
        ++hits;
    } else {
        // Miss: allocate new frame
        std::int64_t pfn = memory_manager.allocate_frame();
        if (pfn < 0) {
            throw std::runtime_error("Out of physical memory in single_process");
        }
        single_pt[vpn] = pfn;
        ++misses;
    }
}

void Task::two_level_process(std::uint64_t vpn) {
    std::uint64_t lvl1 = level1_size();
    std::uint64_t lvl2 = level2_size();

    // Compute two‐level indices
    std::uint64_t index1 = vpn >> Config::LEVEL2_BITS;
    std::uint64_t index2 = vpn & ((static_cast<std::uint64_t>(1) << Config::LEVEL2_BITS) - 1);

    if (index1 >= lvl1) {
        throw std::out_of_range("VPN out of range in two_level_process");
    }

    // If second‐level not created yet, allocate and initialize to -1
    if (two_level_pt[index1] == nullptr) {
        std::int64_t *second = new std::int64_t[lvl2];
        for (std::uint64_t i = 0; i < lvl2; ++i) {
            second[i] = -1;
        }
        two_level_pt[index1] = second;
        ++allocated_second_levels;
    }

    std::int64_t *second = two_level_pt[index1];
    if (second[index2] >= 0) {
        // Page‐table hit
        ++hits;
    } else {
        // Miss: allocate a new frame
        std::int64_t pfn = memory_manager.allocate_frame();
        if (pfn < 0) {
            throw std::runtime_error("Out of physical memory in two_level_process");
        }
        second[index2] = pfn;
        ++misses;
    }
}

std::uint64_t Task::virtual_pages_count() const {
    return Config::VIRTUAL_MEMORY_SIZE / page_size;
}

std::uint64_t Task::level1_size() const {
    return (static_cast<std::uint64_t>(1) << Config::LEVEL1_BITS);
}

std::uint64_t Task::level2_size() const {
    return (static_cast<std::uint64_t>(1) << Config::LEVEL2_BITS);
}
