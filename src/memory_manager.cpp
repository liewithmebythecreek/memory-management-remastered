#include "memory_manager.h"
#include <stdexcept>

MemoryManager::MemoryManager(std::uint64_t page_size_bytes)
    : page_size(page_size_bytes)
{
    if (page_size == 0) {
        throw std::invalid_argument("Page size must be non-zero");
    }
    num_frames = Config::PHYSICAL_MEMORY_SIZE / page_size;
    if (num_frames == 0) {
        throw std::invalid_argument("Page size larger than physical memory");
    }

    // Initially, all frames are free
    frame_bitmap.assign(num_frames, true);
}

MemoryManager::~MemoryManager() {
    // nothing to free explicitly
}

int64_t MemoryManager::allocate_frame() {
    for (std::uint64_t idx = 0; idx < num_frames; ++idx) {
        if (frame_bitmap[idx]) {
            frame_bitmap[idx] = false;
            return static_cast<int64_t>(idx);
        }
    }
    // no free frame found
    return -1;
}

void MemoryManager::free_frame(std::uint64_t frame_number) {
    if (frame_number >= num_frames) {
        throw std::out_of_range("Invalid frame number to free");
    }
    frame_bitmap[frame_number] = true;
}

std::uint64_t MemoryManager::total_frames() const {
    return num_frames;
}

std::uint64_t MemoryManager::free_frames() const {
    std::uint64_t count = 0;
    for (bool free_flag : frame_bitmap) {
        if (free_flag) ++count;
    }
    return count;
}

std::uint64_t MemoryManager::total_memory_size() const {
    return num_frames * page_size;
}

std::uint64_t MemoryManager::free_memory_size() const {
    return free_frames() * page_size;
}
