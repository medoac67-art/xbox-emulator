#pragma once

#include "MemoryBus.hpp"
#include <cstdint>

namespace Emulator::Core::XboxOriginal {

struct CpuRegisters {
    uint32_t eax, ebx, ecx, edx;
    uint32_t esi, edi, ebp, esp;
    uint32_t eip;
    uint32_t eflags;
};

class X86Cpu {
public:
    X86Cpu(MemoryBus& memoryBus);
    ~X86Cpu() = default;

    void Reset();
    void SetEntryPoint(uint32_t address);
    
    // Execute one single x86 instruction
    void Step();

    // Stats/Inspection
    uint64_t GetInstructionCount() const { return m_instructionCount; }
    const CpuRegisters& GetRegisters() const { return m_regs; }

private:
    MemoryBus& m_memoryBus;
    CpuRegisters m_regs;
    uint64_t m_instructionCount = 0;

    // Helper functions
    uint8_t Fetch8();
    uint16_t Fetch16();
    uint32_t Fetch32();
};

} // namespace Emulator::Core::XboxOriginal