#include "io_module.h"
#include <algorithm>
#include <cctype>

IO_Module::IO_Module(const std::string &filename,
                     PageTableType pt_type_,
                     std::uint64_t page_size_,
                     MemoryManager &mem_mgr)
    : trace_filename(filename),
      pt_type(pt_type_),
      page_size(page_size_),
      memory_manager(mem_mgr)
{
}

void IO_Module::run() {
    std::ifstream infile(trace_filename);
    if (!infile.is_open()) {
        throw std::runtime_error("Unable to open trace file: " + trace_filename);
    }

    std::string line;
    while (std::getline(infile, line)) {
        // Remove leading/trailing whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        if (line.empty() || line[0] == '#') continue;

        std::string task_id;
        std::uint64_t logical;
        std::uint64_t size_bytes;
        if (!parse_line(line, task_id, logical, size_bytes)) {
            // skip malformed lines
            continue;
        }

        // If this task ID is new, create a Task instance
        if (tasks.find(task_id) == tasks.end()) {
            auto task_ptr = std::make_shared<Task>(task_id, pt_type, page_size, memory_manager);
            tasks[task_id] = task_ptr;
        }

        // Dispatch request to the Task instance
        tasks[task_id]->process_request(logical, size_bytes);
    }
}

const std::unordered_map<std::string, std::shared_ptr<Task>>& IO_Module::get_tasks() const {
    return tasks;
}

bool IO_Module::parse_line(const std::string &line,
                           std::string &out_task_id,
                           std::uint64_t &out_logical,
                           std::uint64_t &out_size) {
    // Expected format: T<ID>: 0x<hex>:<number><KB or MB>
    // Example: "T1: 0x4000:16KB"
    std::istringstream iss(line);
    std::string token;

    // 1) Read up to colon: "T1:"
    if (!std::getline(iss, token, ':')) return false;
    // Trim whitespace
    token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());
    out_task_id = token; // e.g. "T1"

    // 2) Read up to next colon: " 0x4000"
    if (!std::getline(iss, token, ':')) return false;
    // Trim whitespace
    token.erase(0, token.find_first_not_of(" \t\r\n"));
    token.erase(token.find_last_not_of(" \t\r\n") + 1);
    // Now token contains something like "0x4000"
    try {
        out_logical = std::stoull(token, nullptr, 16);
    } catch (...) {
        return false;
    }

    // 3) Remaining: "16KB" or "4MB" etc.
    if (!std::getline(iss, token)) return false;
    token.erase(0, token.find_first_not_of(" \t\r\n"));
    token.erase(token.find_last_not_of(" \t\r\n") + 1);
    // Determine unit suffix
    std::uint64_t multiplier = 1;
    if (token.size() >= 2) {
        std::string suffix = token.substr(token.size() - 2);
        std::string number_part = token.substr(0, token.size() - 2);
        if (suffix == "KB" || suffix == "kb" || suffix == "Kb" || suffix == "kB") {
            multiplier = 1024ULL;
        } else if (suffix == "MB" || suffix == "mb" || suffix == "Mb" || suffix == "mB") {
            multiplier = 1024ULL * 1024ULL;
        } else {
            return false;
        }
        try {
            std::uint64_t val = std::stoull(number_part);
            out_size = val * multiplier;
        } catch (...) {
            return false;
        }
    } else {
        return false;
    }

    return true;
}
