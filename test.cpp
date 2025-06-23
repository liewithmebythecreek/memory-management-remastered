#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iomanip>
#include <sstream>
#include <windows.h>
#include "common/config.h"
#include <cstdint>
#define SECTIONS 5 // text, data, heap, stack, shared_lib

// Thread-safe random number generation
CRITICAL_SECTION rand_cs;
int thread_safe_rand() {
    EnterCriticalSection(&rand_cs);
    int result = rand();
    LeaveCriticalSection(&rand_cs);
    return result;
}

// Random section names
enum SectionType { TEXT, DATA, HEAP, STACK, SHARED_LIB };

std::string sectionName(SectionType s) {
    switch (s) {
        case TEXT: return "TEXT";
        case DATA: return "DATA";
        case HEAP: return "HEAP";
        case STACK: return "STACK";
        case SHARED_LIB: return "SHARED_LIB";
    }
    return "UNKNOWN";
}

// Get base address of section from config
uint32_t sectionBase(SectionType s) {
    switch (s) {
        case TEXT: return TEXT_BASE_ADDR;
        case DATA: return DATA_BASE_ADDR;
        case HEAP: return HEAP_BASE_ADDR;
        case STACK: return STACK_BASE_ADDR;
        case SHARED_LIB: return SHARED_LIB_ADDR;
    }
    return 0;
}

// Align address to page boundary
uint32_t getAlignedAddress(uint32_t base) {
    uint32_t page_size = MIN_PAGE_SIZE_KB * 1024;
    return base + (rand() % 1024) * page_size;
}

// Generate a size string (randomly 4KB to 16KB)
std::string randomSizeStr() {
    int sizes[] = {4, 8, 16, 32}; // in KB
    int s = sizes[rand() % 3];
    return std::to_string(s) + "KB";
}

// Thread-safe file writing
CRITICAL_SECTION file_cs;

// Thread arguments structure
struct ThreadArgs {
    int task_id;
    int num_lines;  // Changed from max_lines to num_lines
    std::ofstream* file;
};

// Thread function to generate trace lines for a single task
DWORD WINAPI generateTaskTrace(LPVOID arg) {
    ThreadArgs* args = static_cast<ThreadArgs*>(arg);
    std::string task_id = "T" + std::to_string(args->task_id);

    for (int i = 0; i < args->num_lines; ++i) {
        SectionType section = static_cast<SectionType>(thread_safe_rand() % SECTIONS);
        uint32_t addr = getAlignedAddress(sectionBase(section));
        std::stringstream line;
        line << task_id << ": 0x" << std::hex << addr << ": " << randomSizeStr();
        
        // Thread-safe file writing
        EnterCriticalSection(&file_cs);
        *args->file << line.str() << "\n";
        LeaveCriticalSection(&file_cs);
    }
    return 0;
}

// Distribute total lines randomly among tasks
std::vector<int> distributeLines(int total_lines, int num_tasks) {
    std::vector<int> distribution(num_tasks, 0);
    
    // First give at least 1 line to each task
    for (int i = 0; i < num_tasks; i++) {
        distribution[i] = 1;
    }
    
    // Distribute remaining lines randomly
    int remaining_lines = total_lines - num_tasks;
    while (remaining_lines > 0) {
        int task_idx = thread_safe_rand() % num_tasks;
        distribution[task_idx]++;
        remaining_lines--;
    }
    
    return distribution;
}

void generateTraceFile(int num_tasks, int total_lines, const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Could not open file for writing.\n";
        return;
    }

    // Initialize critical sections
    InitializeCriticalSection(&rand_cs);
    InitializeCriticalSection(&file_cs);

    srand(static_cast<unsigned int>(time(nullptr)));

    // Distribute lines among tasks
    std::vector<int> line_distribution = distributeLines(total_lines, num_tasks);

    // Create thread array
    HANDLE* threads = new HANDLE[num_tasks];
    ThreadArgs* thread_args = new ThreadArgs[num_tasks];

    // Create threads
    for (int t = 0; t < num_tasks; ++t) {
        thread_args[t].task_id = t + 1;
        thread_args[t].num_lines = line_distribution[t];
        thread_args[t].file = &file;
        
        threads[t] = CreateThread(
            NULL,                   // default security attributes
            0,                      // use default stack size
            generateTaskTrace,      // thread function name
            &thread_args[t],        // argument to thread function
            0,                      // use default creation flags
            NULL                    // returns the thread identifier
        );

        if (threads[t] == NULL) {
            std::cerr << "Failed to create thread " << t << std::endl;
            continue;
        }
    }

    // Wait for all threads to complete
    WaitForMultipleObjects(num_tasks, threads, TRUE, INFINITE);

    // Cleanup
    for (int t = 0; t < num_tasks; ++t) {
        CloseHandle(threads[t]);
    }
    delete[] threads;
    delete[] thread_args;

    // Cleanup critical sections
    DeleteCriticalSection(&rand_cs);
    DeleteCriticalSection(&file_cs);

    file.close();
    std::cout << "Trace file generated: " << filename << "\n";
    std::cout << "Total lines: " << total_lines << "\n";
    std::cout << "Number of tasks: " << num_tasks << "\n";
    std::cout << "Line distribution per task:\n";
    for (int t = 0; t < num_tasks; ++t) {
        std::cout << "Task " << (t + 1) << ": " << line_distribution[t] << " lines\n";
    }
}

int main(int argc, char* argv[]) {
    int total_lines = 1000000;  // Default: 1 million lines
    int num_tasks = 10;         // Default: 10 tasks

    if (argc == 3) {
        total_lines = std::stoi(argv[1]);
        num_tasks = std::stoi(argv[2]);
    } else if (argc != 1) {
        std::cerr << "Usage: " << argv[0] << " [total_lines] [num_tasks]\n";
        std::cerr << "Using default values: " << total_lines << " lines, " << num_tasks << " tasks\n";
        return 1;
    }

    if (total_lines < num_tasks) {
        std::cerr << "Error: Total lines must be greater than or equal to number of tasks\n";
        return 1;
    }

    std::cout << "Generating trace file with " << total_lines << " lines across " << num_tasks << " tasks...\n";
    generateTraceFile(num_tasks, total_lines, "trace.txt");
    return 0;
}

