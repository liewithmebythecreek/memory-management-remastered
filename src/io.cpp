#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <regex>
#include <cstdint>
#include "task.h" // Task class (from task.cpp)
#include "../include/config.h" // Configuration constants (from config.h)
#include "../include/memory_manager.h"

using namespace std;

// Parses size string like "16KB" or "4MB" into bytes
uint32_t parseSize(const string& sizeStr) {
    uint32_t multiplier = 1;
    if (sizeStr.find("KB") != string::npos) {
        multiplier = 1024;
    } else if (sizeStr.find("MB") != string::npos) {
        multiplier = 1024 * 1024;
    } else {
        cerr << "Unknown size format: " << sizeStr << endl;
        return 0;
    }

    uint32_t number = stoi(sizeStr);
    return number * multiplier;
}

// Process a single trace line
void processLine(const string& line, map<string, task*>& taskMap) {
    stringstream ss(line);
    string taskStr, addrStr, sizeStr;

    // Split line by ':' (format: T1: 0x4000:16KB)
    getline(ss, taskStr, ':');
    getline(ss, addrStr, ':');
    getline(ss, sizeStr);

    if (taskStr.empty() || addrStr.empty() || sizeStr.empty()) {
        cerr << "Malformed line: " << line << endl;
        return;
    }

    string task_id = taskStr;
    uint32_t logical_address = stoul(addrStr, nullptr, 16);
    uint32_t size_in_bytes = parseSize(sizeStr);

    if (taskMap.find(task_id) == taskMap.end()) {
        taskMap[task_id] = new task(task_id); // create new Task if it doesn't exist
    }

    // Simulate access for all pages covered by this memory region
    uint32_t page_size = MIN_PAGE_SIZE_KB * 1024;
    uint32_t num_pages = (size_in_bytes + page_size - 1) / page_size;

    for (uint32_t i = 0; i < num_pages; ++i) {
        uint32_t page_address = logical_address + i * page_size;
        taskMap[task_id]->accessMemory(page_address);
    }
}

// Reads the entire trace file and processes each line
void readTraceFile(const string& filename) {
    ifstream infile(filename);
    if (!infile) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    string line;
    map<string, task*> taskMap;

    while (getline(infile, line)) {
        if (line.empty()) continue;
        processLine(line, taskMap);
    }

    // Print stats for all tasks
    cout << "\n=== Memory Access Summary ===\n";
    for (auto it : taskMap) {
        it.second->printStats();
        delete it.second; // clean up
    }
}

int main(){
    readTraceFile("trace.txt");
}
