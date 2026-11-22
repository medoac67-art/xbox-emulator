#include "MemoryBus.hpp"
#include "Logger.hpp"
#include <cstring>

namespace Emulator::Core::XboxOriginal {

MemoryBus::MemoryBus() : m_ramSize(0) {}

bool MemoryBus::Initialize(uint32_t ramSize) {
    m_ramSize = ramSize;
    try {
        m_ram.resize(m_ramSize, 0);
        XLOG_INFO("MemoryBus initialized with " + std::to_string(m_ramSize / (1024 * 1024)) + " MB of RAM.");
        return true;
    } catch (const std::exception& e) {
        XLOG_ERROR("Failed to allocate RAM for Xbox core: " + std::string(e.what()));
        return false;
    }
}

void MemoryBus::Reset() {
    std::fill(m_ram.begin(), m_ram.end(), 0);
}

uint32_t MemoryBus::TranslateVirtualAddress(uint32_t virtualAddress) const {
    // Basic direct mapping simulation for the Kernel and ROM regions.
    // Xbox uses a x86 Page Table, but most executable sections load into specific addresses (usually around 0x00010000 to 0x000F0000 upwards).
    // Let's implement a simple translator.
    // E.g., strip high bits if it's the kernel identity-mapped space, or directly map 0x80000000 region.
    if (virtualAddress >= 0x80000000) {
        return virtualAddress & (m_ramSize - 1);
    }
    // If it's a standard low address, map it directly or relative to base
    return virtualAddress & (m_ramSize - 1);
}

bool MemoryBus::Write8(uint32_t virtualAddr, uint8_t value) {
    uint32_t phys = TranslateVirtualAddress(virtualAddr);
    if (phys < m_ramSize) {
        m_ram[phys] = value;
        return true;
    }
    return false;
}

bool MemoryBus::Write16(uint32_t virtualAddr, uint16_t value) {
    uint32_t phys = TranslateVirtualAddress(virtualAddr);
    if (phys + 1 < m_ramSize) {
        std::memcpy(&m_ram[phys], &value, sizeof(value));
        return true;
    }
    return false;
}

bool MemoryBus::Write32(uint32_t virtualAddr, uint32_t value) {
    uint32_t phys = TranslateVirtualAddress(virtualAddr);
    if (phys + 3 < m_ramSize) {
        std::memcpy(&m_ram[phys], &value, sizeof(value));
        return true;
    }
    return false;
}

bool MemoryBus::WriteBytes(uint32_t virtualAddr, const void* data, size_t size) {
    uint32_t phys = TranslateVirtualAddress(virtualAddr);
    if (phys + size <= m_ramSize) {
        std::memcpy(&m_ram[phys], data, size);
        return true;
    }
    return false;
}

uint8_t MemoryBus::Read8(uint32_t virtualAddr) {
    uint32_t phys = TranslateVirtualAddress(virtualAddr);
    if (phys < m_ramSize) {
        return m_ram[phys];
    }
    return 0;
}

uint16_t MemoryBus::Read16(uint32_t virtualAddr) {
    uint32_t phys = TranslateVirtualAddress(virtualAddr);
    if (phys + 1 < m_ramSize) {
        uint16_t val;
        std::memcpy(&val, &m_ram[phys], sizeof(val));
        return val;
    }
    return 0;
}

uint32_t MemoryBus::Read32(uint32_t virtualAddr) {
    uint32_t phys = TranslateVirtualAddress(virtualAddr);
    if (phys + 3 < m_ramSize) {
        uint32_t val;
        std::memcpy(&val, &m_ram[phys], sizeof(val));
        return val;
    }
    return 0;
}

bool MemoryBus::ReadBytes(uint32_t virtualAddr, void* dest, size_t size) {
    uint32_t phys = TranslateVirtualAddress(virtualAddr);
    if (phys + size <= m_ramSize) {
        std::memcpy(dest, &m_ram[phys], size);
        return true;
    }
    return false;
}

} // namespace Emulator::Core::XboxOriginal