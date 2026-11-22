#pragma once

#include <vector>
#include <cstdint>
#include <memory>
#include <string>

namespace Emulator::Core::XboxOriginal {

// RAM configuration for the original Xbox
constexpr uint32_t XBOX_RAM_SIZE_64MB = 64 * 1024 * 1024;
constexpr uint32_t XBOX_RAM_SIZE_128MB = 128 * 1024 * 1024;

class MemoryBus {
public:
    MemoryBus();
    ~MemoryBus() = default;

    bool Initialize(uint32_t ramSize = XBOX_RAM_SIZE_64MB);
    void Reset();

    // Fast inline physical memory accessors
    inline uint8_t* GetPhysicalPointer(uint32_t physicalAddress);
    
    // Virtual to Physical address translation
    // In Xbox Original, kernel addresses are typically mapped using Page Tables,
    // but the system has high/low virtual regions mapped to physical 0 (identity or direct mapping).
    uint32_t TranslateVirtualAddress(uint32_t virtualAddress) const;

    // Read and Write methods supporting virtual addresses
    bool Write8(uint32_t virtualAddr, uint8_t value);
    bool Write16(uint32_t virtualAddr, uint16_t value);
    bool Write32(uint32_t virtualAddr, uint32_t value);
    bool WriteBytes(uint32_t virtualAddr, const void* data, size_t size);

    uint8_t Read8(uint32_t virtualAddr);
    uint16_t Read16(uint32_t virtualAddr);
    uint32_t Read32(uint32_t virtualAddr);
    bool ReadBytes(uint32_t virtualAddr, void* dest, size_t size);

    uint32_t GetRamSize() const { return m_ramSize; }
    uint8_t* GetRamBase() { return m_ram.data(); }

private:
    std::vector<uint8_t> m_ram;
    uint32_t m_ramSize = 0;
};

inline uint8_t* MemoryBus::GetPhysicalPointer(uint32_t physicalAddress) {
    if (physicalAddress < m_ramSize) {
        return &m_ram[physicalAddress];
    }
    return nullptr;
}

} // namespace Emulator::Core::XboxOriginal