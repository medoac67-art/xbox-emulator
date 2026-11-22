#include "XboxSeriesCore.hpp"
#include <cstring>
#include "../../core/include/Logger.hpp"

namespace Emulator::Core {

bool XboxSeriesCore::Initialize() {
    XLOG_INFO("Xbox Series Core: Initializing Zen 2 & RDNA 2...");
    m_isSeriesX = false;
    m_memorySize = 16 * 1024 * 1024; // PoC: 16MB lightweight pool
    m_memoryBuffer.resize(m_memorySize);
    return true;
}

bool XboxSeriesCore::LoadTitle(const std::string& filePath) {
    XLOG_INFO("Xbox Series Core: Attempting to load .appx: " + filePath);
    m_memoryBuffer[0] = 0xC3; // RET
    return true;
}

void XboxSeriesCore::RunFrame() {
    m_stats.frames++;
    m_cpu.ExecuteCycle();
    m_renderer.RenderFrame();
}

void XboxSeriesCore::Pause() {}
void XboxSeriesCore::Step() { m_cpu.ExecuteCycle(); }

bool XboxSeriesCore::ReadMemory(uint64_t address, void* out, size_t size) {
    if (address + size > m_memorySize) return false;
    std::memcpy(out, &m_memoryBuffer[address], size);
    return true;
}

bool XboxSeriesCore::WriteMemory(uint64_t address, const void* src, size_t size) {
    if (address + size > m_memorySize) return false;
    std::memcpy(&m_memoryBuffer[address], src, size);
    return true;
}

CoreStats XboxSeriesCore::GetStats() const { 
    CoreStats s = m_stats;
    s.coreName = m_isSeriesX ? "Xbox Series X" : "Xbox Series S";
    return s;
}

std::vector<MemoryRegion> XboxSeriesCore::GetMemoryMap() const {
    return {{0x00000000, m_memorySize, "Series RAM (PoC)", true}};
}
const char* XboxSeriesCore::GetCoreName() const noexcept { return "Xbox Series X|S (Zen2) Core v0.1"; }

} // namespace Emulator::Core