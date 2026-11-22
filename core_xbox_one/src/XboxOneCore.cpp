#include "XboxOneCore.hpp"
#include <cstring>
#include "../../core/include/Logger.hpp"

namespace Emulator::Core {

bool XboxOneCore::Initialize() {
    XLOG_INFO("Xbox One Core: Initializing Jaguar x86-64 Architecture...");
    m_memorySize = 8 * 1024 * 1024; // PoC: 8MB lightweight pool
    m_memoryBuffer.resize(m_memorySize);
    return true;
}

bool XboxOneCore::LoadTitle(const std::string& filePath) {
    XLOG_INFO("Xbox One Core: Attempting to load .appx: " + filePath);
    // Synthetic PoC payload
    m_memoryBuffer[0] = 0xC3; // RET
    return true;
}

void XboxOneCore::RunFrame() {
    m_stats.frames++;
    m_cpu.ExecuteCycle();
    m_renderer.RenderFrame();
}

void XboxOneCore::Pause() {}
void XboxOneCore::Step() { m_cpu.ExecuteCycle(); }

bool XboxOneCore::ReadMemory(uint64_t address, void* out, size_t size) {
    if (address + size > m_memorySize) return false;
    std::memcpy(out, &m_memoryBuffer[address], size);
    return true;
}

bool XboxOneCore::WriteMemory(uint64_t address, const void* src, size_t size) {
    if (address + size > m_memorySize) return false;
    std::memcpy(&m_memoryBuffer[address], src, size);
    return true;
}

CoreStats XboxOneCore::GetStats() const { return m_stats; }
std::vector<MemoryRegion> XboxOneCore::GetMemoryMap() const {
    return {{0x00000000, m_memorySize, "One RAM (PoC)", true}};
}
const char* XboxOneCore::GetCoreName() const noexcept { return "Xbox One (Jaguar) Core v0.1"; }

} // namespace Emulator::Core