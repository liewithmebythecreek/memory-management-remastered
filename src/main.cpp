#include <iostream>
#include <string>
#include <filesystem>
#include "memory_manager.h"
#include "task.h"
#include "test_module.h"
#include "performance.h"
#include "io_module.h"
#include "config.h"

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    if (argc < 5) {
        std::cerr << "Usage: " << argv[0] << " <num_tasks> <num_requests> <page_size> <trace_file_path>\n";
        return 1;
    }

    int numTasks = std::stoi(argv[1]);
    int numRequests = std::stoi(argv[2]);
    int pageSize = std::stoi(argv[3]);
    std::string traceFilePath = argv[4];

    // Step 1: Create trace file directory if it doesn't exist
    fs::path tracePath(traceFilePath);
    fs::create_directories(tracePath.parent_path());

    // Step 2: Generate Trace
    std::cout << "Generating test trace...\n";
    TestModule testModule(numTasks, numRequests, traceFilePath, pageSize);
    for (int task_id = 0; task_id < numTasks; ++task_id) {
        testModule.generate_for_task(task_id);
    }
    std::cout << "Trace generated at: " << traceFilePath << "\n";

    // Step 3: Read and parse trace
    std::cout << "Reading trace and parsing tasks...\n";
    MemoryManager memoryManager(pageSize);
    IO_Module ioModule(traceFilePath, PageTableType::MAP, pageSize, memoryManager);

    // Step 4: Performance test
    std::cout << "\nRunning performance analysis with different page table implementations...\n";
    Performance perf(traceFilePath, pageSize);

    std::cout << "\n--- Using MAP Page Table ---\n";
    perf.run_single(PageTableType::MAP);

    std::cout << "\n--- Using SINGLE LEVEL Page Table ---\n";
    perf.run_single(PageTableType::SINGLE);

    std::cout << "\n--- Using TWO LEVEL Page Table ---\n";
    perf.run_single(PageTableType::TWO_LEVEL);

    std::cout << "\nAll tests complete.\n";

    return 0;
}
