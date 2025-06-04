#ifndef CONFIG_H
#define CONFIG_H

#include <cstdint>
#include <cstddef>

/*
 * ===========================
 *  Configuration Parameters
 * ===========================
 *
 * 1. Page sizes: support multiple page sizes (in bytes).
 * 2. Virtual memory size (2^N).
 * 3. Physical memory size (2^N).
 * 4. Two‐level page table parameters.
 * 5. Fixed starting addresses for text, data, stack, shared library, heap
 *    (used by Test module).
 */

namespace Config {

// --------------------------------------------------------------------------------
// 1) Page size(s):
//    We allow up to three selectable page sizes. In practice, tasks will pick one.
//    Values are in bytes. Example: 1 KB = 1024, 4 KB = 4096, etc.
// --------------------------------------------------------------------------------
static constexpr std::uint64_t PAGE_SIZE_1 = 1024ULL;   // 1 KB
static constexpr std::uint64_t PAGE_SIZE_2 = 4096ULL;   // 4 KB
static constexpr std::uint64_t PAGE_SIZE_3 = 16384ULL;  // 16 KB

// --------------------------------------------------------------------------------
// 2) Virtual memory size (2^N):
//    We let the user choose N at compile‐time. For demonstration, default N=32
//    (i.e., 4 GB of virtual address space). Adjust as needed.
// --------------------------------------------------------------------------------
static constexpr std::uint8_t VIRT_MEM_BITS = 32;      // 2^32 = 4 GB
static constexpr std::uint64_t VIRTUAL_MEMORY_SIZE = (1ULL << VIRT_MEM_BITS);

// --------------------------------------------------------------------------------
// 3) Physical memory size (2^M):
//    Default M=30 (i.e., 1 GB of physical memory). Adjust if needed.
// --------------------------------------------------------------------------------
static constexpr std::uint8_t PHYS_MEM_BITS = 30;      // 2^30 = 1 GB
static constexpr std::uint64_t PHYSICAL_MEMORY_SIZE = (1ULL << PHYS_MEM_BITS);

// --------------------------------------------------------------------------------
// 4) Two‐level page table parameters:
//    We divide the virtual page number into two indices: top‐level index bits
//    and second‐level index bits. For example, if page size = 4 KB (12 bits offset),
//    and we want 10 bits for first level and 10 bits for second level (for a 32‐bit
//    address space), then:
//
//      VIRTUAL_BITS = 32
//      OFFSET_BITS = log2(PAGE_SIZE)   (e.g., 12 for 4 KB)
//      VPN_BITS    = VIRTUAL_BITS - OFFSET_BITS   (e.g., 32 - 12 = 20)
//      LEVEL1_BITS = 10
//      LEVEL2_BITS = VPN_BITS - LEVEL1_BITS (e.g., 10)
//
//    Here we choose defaults; user can override by editing these macros.
// --------------------------------------------------------------------------------
static constexpr std::uint8_t LEVEL1_BITS = 10;   // top‐level index bits
static constexpr std::uint8_t LEVEL2_BITS = 10;   // second‐level index bits

// --------------------------------------------------------------------------------
// 5) Fixed base addresses for “sections” (used by Test module to generate addresses).
//    These addresses must be multiples of the minimum page size (1 KB).
//    You can change them according to your design.
// --------------------------------------------------------------------------------
static constexpr std::uint64_t TEXT_BASE_ADDR       = 0x0040'0000ULL; // 4 MB
static constexpr std::uint64_t DATA_BASE_ADDR       = 0x1000'0000ULL; // 256 MB
static constexpr std::uint64_t HEAP_BASE_ADDR       = 0x2000'0000ULL; // 512 MB
static constexpr std::uint64_t STACK_BASE_ADDR      = 0x7FFF'0000ULL; // just under 2 GB
static constexpr std::uint64_t SHARED_LIB_BASE_ADDR = 0x5000'0000ULL; // 1.25 GB

// --------------------------------------------------------------------------------
// 6) Miscellaneous settings:
//    - MAX_TASKS      : upper bound on number of concurrent tasks (for pthread pool).
//    - TRACE_LINE_BUFFER: max chars per line when reading traces.
// --------------------------------------------------------------------------------
static constexpr int    MAX_TASKS = 256;
static constexpr size_t TRACE_LINE_BUFFER = 1024;

} // namespace Config

#endif // CONFIG_H
