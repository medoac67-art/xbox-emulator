#include "Xbox360Core.hpp"
#include <cstring>
#include <fstream>
#include <algorithm>
#include "../../core/include/Logger.hpp"

namespace Emulator::Core {

bool Xbox360Core::Initialize() {
    XLOG_INFO("Xbox 360 Core: Initializing Xenon architecture...");
    m_memorySize = 256 * 1024 * 1024; // 256MB for 360
    m_memoryBuffer.resize(m_memorySize);
    return true;
}

bool Xbox360Core::LoadTitle(const std::string& filePath) {
    XLOG_INFO("Xbox 360 Core: Loading .xex title: " + filePath);
    // Simplified XEX loader for PoC
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file) {
        XLOG_WARN("Could not open file for 360 PoC, loading synthetic test payload.");
        // Synthetic test: Write a simple 0x3880 0001 (LI R0, 1) to RAM
        m_memoryBuffer[0x00000000] = 0x38;
        m_memoryBuffer[0x00000001] = 0x80;
        return true;
    }
    // Real loading would happen here
    return true;
}

void Xbox360Core::RunFrame() {
    m_stats.frames++;
    // Execute a step in Xenon
    m_cpu.ExecuteCycle();
    m_renderer.RenderFrame();
}

void Xbox360Core::Pause() { /* m_isPaused = true; */ }
void Xbox360Core::Step() { m_cpu.ExecuteCycle(); }

bool Xbox360Core::ReadMemory(uint64_t address, void* out, size_t size) {
    if (address >= m_memorySize || address + size > m_memorySize) return false;
    std::memcpy(out, &m_memoryBuffer[address], size);
    return true;
}

bool Xbox360Core::WriteMemory(uint64_t address, const void* src, size_t size) {
    if (address >= m_memorySize || address + size > m_memorySize) return false;
    std::memcpy(&m_memoryBuffer[address], src, size);
    return true;
}

Emulator::Core::CoreStats Xbox360Core::GetStats() const {
    return m_stats;
}

std::vector<Emulator::Core::MemoryRegion> Xbox360Core::GetMemoryMap() const {
    return {
        {0x00000000, m_memorySize, "Main RAM (LIMBO)", true}
    };
}

const char* Xbox360Core::GetCoreName() const noexcept {
    return "Xbox 360 (Xenon) Core v0.1";
}

} // namespace Emulator::Core