#ifndef IO_MODULE_H
#define IO_MODULE_H

#include <string>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>

#include "task.h"

/**
 * IO_Module:
 *   - Reads a trace file line by line.
 *   - Parses lines of format: "T<ID>: <LogicalAddressHex>: <Size>".
 *   - Looks up the Task instance by ID and invokes process_request on it.
 *   - Creates Task instances on first encounter (with a chosen PageTableType and page size).
 */
class IO_Module {
public:
    /**
     * Constructor.
     * @param filename: path to the trace file.
     * @param pt_type: page table implementation for all tasks.
     * @param page_size: page size (bytes).
     * @param mem_mgr: reference to shared MemoryManager.
     */
    IO_Module(const std::string &filename,
              PageTableType pt_type,
              std::uint64_t page_size,
              MemoryManager &mem_mgr);

    /**
     * Reads the entire trace file and dispatches requests to Tasks.
     */
    void run();

    /**
     * After processing, return reference to the internal task map.
     * Key: task ID (e.g. "T1"), Value: shared_ptr<Task>
     */
    const std::unordered_map<std::string, std::shared_ptr<Task>>& get_tasks() const;

private:
    std::string trace_filename;
    PageTableType pt_type;
    std::uint64_t page_size;
    MemoryManager &memory_manager;

    // Map from task ID ("T1") to Task instance
    std::unordered_map<std::string, std::shared_ptr<Task>> tasks;

    /**
     * Parse a single line from the trace file.
     * Example line: "T1: 0x4000:16KB"
     * @param line: input line
     * @param out_task_id: parsed task ID (e.g. "T1")
     * @param out_logical: parsed logical address (uint64_t)
     * @param out_size: parsed size (in bytes)
     * @return true if parse succeeded, false otherwise.
     */
    bool parse_line(const std::string &line,
                    std::string &out_task_id,
                    std::uint64_t &out_logical,
                    std::uint64_t &out_size);
};

#endif // IO_MODULE_H
