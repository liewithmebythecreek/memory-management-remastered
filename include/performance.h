#ifndef PERFORMANCE_H
#define PERFORMANCE_H

#include <string>
#include <unordered_map>
#include <memory>
#include <chrono>

#include "memory_manager.h"
#include "task.h"
#include "io_module.h"
#include "config.h"

/**
 * Performance:
 *   - Runs the trace for each page table implementation (MAP, SINGLE, TWO_LEVEL).
 *   - Measures execution time, computes statistics:
 *       * Total page table memory per task.
 *       * Physical memory allocated (frames) per task and overall.
 *       * Available (free) physical memory.
 *       * Number of page hits and misses per task.
 *   - Prints a summary report for each implementation.
 */
class Performance {
public:
    /**
     * Constructor.
     * @param trace_file: path to the trace file.
     * @param page_size: page size in bytes.
     */
    Performance(const std::string &trace_file, std::uint64_t page_size);

    /**
     * Run a single implementation, measure time, collect and print stats.
     * @param pt_type: which PageTableType to test.
     */
    void run_single(PageTableType pt_type);
    /**
     * Run performance tests for all three implementations.
     * Will create a fresh MemoryManager for each run.
     */
    void run_all();

private:
    std::string trace_filename;
    std::uint64_t page_size;

    

    /**
     * Print statistics for a given run.
     * @param pt_type: page table type tested.
     * @param duration_ms: elapsed time in milliseconds.
     * @param tasks: map of task ID to Task instance.
     * @param mem_mgr: MemoryManager instance used in this run.
     */
    void print_report(PageTableType pt_type,
                      double duration_ms,
                      const std::unordered_map<std::string, std::shared_ptr<Task>> &tasks,
                      MemoryManager &mem_mgr) const;

    /**
     * Helper to convert PageTableType to string.
     */
    static std::string pt_type_to_string(PageTableType pt_type);
};

#endif // PERFORMANCE_H
