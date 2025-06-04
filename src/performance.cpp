#include "performance.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>

Performance::Performance(const std::string &trace_file, std::uint64_t page_size_)
    : trace_filename(trace_file),
      page_size(page_size_)
{
    if (page_size == 0) {
        throw std::invalid_argument("Page size must be non-zero in Performance");
    }
    if (Config::PHYSICAL_MEMORY_SIZE % page_size != 0) {
        throw std::invalid_argument("Page size must evenly divide physical memory in Performance");
    }
}

void Performance::run_all() {
    // Iterate through each PageTableType
    run_single(PageTableType::MAP);
    run_single(PageTableType::SINGLE);
    run_single(PageTableType::TWO_LEVEL);
}

void Performance::run_single(PageTableType pt_type) {
    // Create a fresh memory manager for this run
    MemoryManager mem_mgr(page_size);

    // Create IO module to process the trace
    IO_Module io(trace_filename, pt_type, page_size, mem_mgr);

    // Start timing
    auto start = std::chrono::high_resolution_clock::now();
    io.run();
    auto end = std::chrono::high_resolution_clock::now();

    double duration_ms = std::chrono::duration<double, std::milli>(end - start).count();

    // Retrieve tasks map
    const auto &tasks = io.get_tasks();

    // Print report for this implementation
    print_report(pt_type, duration_ms, tasks, mem_mgr);
}

void Performance::print_report(PageTableType pt_type,
                               double duration_ms,
                               const std::unordered_map<std::string, std::shared_ptr<Task>> &tasks,
                               MemoryManager &mem_mgr) const {
    std::string pt_name = pt_type_to_string(pt_type);
    std::cout << "==============================\n";
    std::cout << "Page Table Implementation: " << pt_name << "\n";
    std::cout << "Execution Time: " << std::fixed << std::setprecision(3)
              << duration_ms << " ms\n";
    std::cout << "------------------------------\n";

    std::uint64_t total_pt_mem = 0;
    std::uint64_t total_frames_allocated = 0;

    std::cout << "Per-Task Statistics:\n";
    std::cout << std::left << std::setw(10) << "TaskID"
              << std::right << std::setw(10) << "Hits"
              << std::setw(10) << "Misses"
              << std::setw(15) << "PT Memory(B)"
              << std::setw(15) << "FramesAlloc" << "\n";

    for (const auto &kv : tasks) {
        const std::string &task_id = kv.first;
        const auto &task_ptr = kv.second;
        std::uint64_t hits = task_ptr->get_hits();
        std::uint64_t misses = task_ptr->get_misses();
        std::uint64_t pt_mem = task_ptr->get_page_table_memory();
        std::uint64_t frames_alloc = misses; // each miss allocated one frame

        total_pt_mem += pt_mem;
        total_frames_allocated += frames_alloc;

        std::cout << std::left << std::setw(10) << task_id
                  << std::right << std::setw(10) << hits
                  << std::setw(10) << misses
                  << std::setw(15) << pt_mem
                  << std::setw(15) << frames_alloc << "\n";
    }

    std::cout << "------------------------------\n";
    std::cout << "Total PageTable Memory: " << total_pt_mem << " bytes\n";

    // Physical memory allocated to all tasks is equal to frames allocated * page_size
    std::uint64_t phys_mem_allocated = total_frames_allocated * page_size;
    std::cout << "Total Physical Memory Allocated: " << phys_mem_allocated << " bytes\n";

    std::uint64_t free_mem = mem_mgr.free_memory_size();
    std::cout << "Physical Memory Free: " << free_mem << " bytes\n";
    std::cout << "==============================\n";
    std::cout << std::endl;
}

std::string Performance::pt_type_to_string(PageTableType pt_type) {
    switch (pt_type) {
        case PageTableType::MAP:
            return "Map";
        case PageTableType::SINGLE:
            return "Single-Level";
        case PageTableType::TWO_LEVEL:
            return "Two-Level";
        default:
            return "Unknown";
    }
}
