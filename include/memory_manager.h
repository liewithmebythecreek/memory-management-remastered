#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <cstdint>
#include <vector>
#include <mutex>          // use std::mutex for thread safety

#include "config.h"

/**
 * MemoryManager:
 *   - Keeps track of free/allocated physical pages.
 *   - On request, returns a free physical frame number (or -1 if none available).
 *   - Allows freeing a previously allocated frame.
 *   - Reports total and free physical memory.
 *   - Thread-safe for concurrent allocations/frees.
 */
class MemoryManager {
public:
    /**
     * Constructor: initialize with a given page size (in bytes). Computes total frames.
     */
    MemoryManager(std::uint64_t page_size_bytes);
    ~MemoryManager();                    // destroy the pthread mutex
    /**
     * Allocate one physical frame. Returns the allocated frame number (0-based),
     * or -1 if no free frames are available.
     */
    int64_t allocate_frame();

    /**
     * Free a previously allocated frame (frame_number). Marks it as free.
     * Caller must ensure frame_number is valid.
     */
    void free_frame(std::uint64_t frame_number);

    /**
     * Get total number of physical frames managed.
     */
    std::uint64_t total_frames() const;

    /**
     * Get number of free frames currently available.
     */
    std::uint64_t free_frames() const;

    /**
     * Get total physical memory size (in bytes).
     */
    std::uint64_t total_memory_size() const;

    /**
     * Get free physical memory size (in bytes).
     */
    std::uint64_t free_memory_size() const;

private:
    std::uint64_t page_size;        // page size in bytes
    std::uint64_t num_frames;       // total number of frames
    std::vector<bool> frame_bitmap; // true == free, false == allocated

    mutable std::mutex mtx;         // C++ standard mutex
};

#endif // MEMORY_MANAGER_H
