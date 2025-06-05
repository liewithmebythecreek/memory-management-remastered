#include "test_module.h"
#include <fstream>
#include <random>
#include <sstream>
#include <iomanip>
#include <vector>
#include <stdexcept>

// Maximum number of pages per section (arbitrary). Each section will be in [base, base + MAX_PAGES_IN_SECTION * page_size).
static constexpr int MAX_PAGES_IN_SECTION = 1024;

TestModule::TestModule(int num_tasks_, int requests_per_task_,
                       const std::string &output_filename_,
                       std::uint64_t page_size_bytes_)
    : num_tasks(num_tasks_),
      requests_per_task(requests_per_task_),
      output_filename(output_filename_),
      page_size(page_size_bytes_)
{
    if (num_tasks <= 0) {
        throw std::invalid_argument("Number of tasks must be positive");
    }
    if (requests_per_task <= 0) {
        throw std::invalid_argument("Requests per task must be positive");
    }
    if (page_size == 0 || (Config::PHYSICAL_MEMORY_SIZE % page_size) != 0) {
        throw std::invalid_argument("Invalid page size in TestModule");
    }
    pthread_mutex_init(&file_mutex, nullptr);
}

TestModule::~TestModule() {
    pthread_mutex_destroy(&file_mutex);
}

void TestModule::run() {
    // Clear (truncate) output file
    {
        std::ofstream ofs(output_filename, std::ios::trunc);
        if (!ofs.is_open()) {
            throw std::runtime_error("Unable to create output file: " + output_filename);
        }
    }

    // Spawn threads
    std::vector<pthread_t> threads(num_tasks);
    std::vector<ThreadArg> thread_args(num_tasks);
    for (int i = 0; i < num_tasks; ++i) {
        thread_args[i].module = this;
        thread_args[i].task_index = i + 1; // 1-based
        int rc = pthread_create(&threads[i], nullptr, TestModule::thread_func, &thread_args[i]);
        if (rc != 0) {
            throw std::runtime_error("Failed to create pthread for task " + std::to_string(i+1));
        }
    }

    // Join threads
    for (int i = 0; i < num_tasks; ++i) {
        pthread_join(threads[i], nullptr);
    }
}

void *TestModule::thread_func(void *arg) {
    ThreadArg *targ = static_cast<ThreadArg *>(arg);
    TestModule *module = targ->module;
    int task_idx = targ->task_index;
    module->generate_for_task(task_idx);
    return nullptr;
}

void TestModule::generate_for_task(int task_index) {
    // Initialize random engine with seed based on task_index and random_device
    std::random_device rd;
    std::mt19937 engine(rd() ^ (static_cast<unsigned long>(task_index) << 1));
    std::uniform_int_distribution<int> section_dist(0, 4); // 5 sections: 0..4
    std::uniform_int_distribution<int> offset_dist(0, MAX_PAGES_IN_SECTION - 1);

    // Section base addresses from config
    const std::uint64_t bases[5] = {
        Config::TEXT_BASE_ADDR,
        Config::DATA_BASE_ADDR,
        Config::HEAP_BASE_ADDR,
        Config::STACK_BASE_ADDR,
        Config::SHARED_LIB_BASE_ADDR
    };

    for (int i = 0; i < requests_per_task; ++i) {
        int sec = section_dist(engine);
        int page_offset = offset_dist(engine);
        std::uint64_t logical_addr = bases[sec] + static_cast<std::uint64_t>(page_offset) * page_size;
        std::uint64_t size_bytes = page_size;
        write_request(task_index, logical_addr, size_bytes);
    }
}

void TestModule::write_request(int task_index, std::uint64_t logical_addr, std::uint64_t size_bytes) {
    // Format size string (e.g., "4KB" or "1MB")
    std::string size_str = size_to_string(size_bytes);

    // Format logical address as hex (without leading zeros): e.g., 0x1A2B3C
    std::ostringstream oss;
    oss << "T" << task_index << ": 0x"
        << std::hex << std::uppercase << logical_addr << std::dec << ":"
        << size_str;
    std::string line = oss.str();

    // Write to file under mutex
    pthread_mutex_lock(&file_mutex);
    std::ofstream ofs(output_filename, std::ios::app);
    if (ofs.is_open()) {
        ofs << line << "\n";
    }
    ofs.close();
    pthread_mutex_unlock(&file_mutex);
}

std::string TestModule::size_to_string(std::uint64_t size_bytes) {
    if (size_bytes % (1024ULL * 1024ULL) == 0) {
        std::uint64_t mb = size_bytes / (1024ULL * 1024ULL);
        return std::to_string(mb) + "MB";
    } else if (size_bytes % 1024ULL == 0) {
        std::uint64_t kb = size_bytes / 1024ULL;
        return std::to_string(kb) + "KB";
    } else {
        return std::to_string(size_bytes) + "B";
    }
}
