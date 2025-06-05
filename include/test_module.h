#ifndef TEST_MODULE_H
#define TEST_MODULE_H

#include <string>
#include <cstdint>
#include <pthread.h>

#include "config.h"

/**
 * TestModule:
 *   - Generates a synthetic trace file for multiple tasks using pthreads.
 *   - Each thread corresponds to one task (T1, T2, ...).
 *   - Each task generates a fixed number of memory requests.
 *   - Addresses are chosen based on section base addresses (config.h) and a random offset,
 *     always aligned to the page size (the minimum page size boundary).
 *   - Output format: "T<ID>: 0x<hex_address>:<Size>" (Size in KB or MB).
 */
class TestModule {
public:
    /**
     * Constructor.
     * @param num_tasks_           : number of concurrent tasks to spawn.
     * @param requests_per_task_   : number of memory references each task generates.
     * @param output_filename_     : path to the trace file to generate.
     * @param page_size_bytes_     : page size in bytes (must match the minimum page size in config).
     */
    TestModule(int num_tasks_, int requests_per_task_,
               const std::string &output_filename_,
               std::uint64_t page_size_bytes_);

    ~TestModule();

     /**
     * Instance method called by each thread to generate its part of the trace.
     * @param task_index: 1-based ID of the task.
     */
    void generate_for_task(int task_index);
    /** 
     * Generate the trace file by spawning one pthread per task.
     */
    void run();

private:
    int num_tasks;
    int requests_per_task;
    std::string output_filename;
    std::uint64_t page_size;

    pthread_mutex_t file_mutex;  // protects write access to the output file

    /**
     * Structure passed to each pthread.
     */
    struct ThreadArg {
        TestModule *module;
        int task_index; // 1-based index: 1..num_tasks
    };

    /** 
     * Pthreads entry point for each task.
     */
    static void *thread_func(void *arg);

   

    /**
     * Helper to format a single memory request line and append to the file.
     * @param task_index: task ID (1-based).
     * @param logical_addr: the generated logical address.
     * @param size_bytes: typically equal to page_size.
     */
    void write_request(int task_index, std::uint64_t logical_addr, std::uint64_t size_bytes);

    /**
     * Convert size in bytes to a string with KB or MB suffix.
     * E.g., 4096 -> "4KB", 1048576 -> "1MB".
     */
    static std::string size_to_string(std::uint64_t size_bytes);
};

#endif // TEST_MODULE_H
