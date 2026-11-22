#pragma once

#include "IEmulationCore.hpp"
#include "MemoryBus.hpp"
#include "XbeLoader.hpp"
#include "X86Cpu.hpp"
#include <memory>
#include <string>

namespace Emulator::Core::XboxOriginal {

class XboxOriginalCore : public Emulator::Core::IEmulationCore {
public:
    XboxOriginalCore();
    ~XboxOriginalCore() override = default;

    // Core Lifecycle
    bool Initialize() override;
    void Shutdown() override;

    // Rom/Executable Loading
    bool LoadTitle(const std::string& filePath) override;
    
    // Execution Control
    void RunFrame() override;
    void Pause() override;
    void Step() override;
    void Reset() override;

    // Memory Inspection for Debugging
    std::vector<MemoryRegion> GetMemoryMap() const override;
    bool ReadMemory(uint64_t address, void* dest, size_t size) override;
    bool WriteMemory(uint64_t address, const void* src, size_t size) override;

    // Performance and Telemetry
    CoreStats GetStats() const override;
    const char* GetCoreName() const noexcept override { return "Xbox Original Core (PoC)"; }

private:
    std::unique_ptr<MemoryBus> m_memoryBus;
    std::unique_ptr<XbeLoader> m_xbeLoader;
    std::unique_ptr<X86Cpu> m_cpu;

    bool m_isInitialized = false;
    bool m_isRunning = false;
    uint32_t m_entryPoint = 0;
};

} // namespace Emulator::Core::XboxOriginal