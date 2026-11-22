#include "X86Cpu.hpp"
#include "Logger.hpp"
#include <cstring>

namespace Emulator::Core::XboxOriginal {

X86Cpu::X86Cpu(MemoryBus& memoryBus) : m_memoryBus(memoryBus) {
    Reset();
}

void X86Cpu::Reset() {
    std::memset(&m_regs, 0, sizeof(m_regs));
    m_regs.esp = 0x0007FFFC; // Common stack start
    m_regs.eflags = 0x00000002;
    m_instructionCount = 0;
}

void X86Cpu::SetEntryPoint(uint32_t address) {
    m_regs.eip = address;
}

uint8_t X86Cpu::Fetch8() {
    uint8_t val = m_memoryBus.Read8(m_regs.eip);
    m_regs.eip += 1;
    return val;
}

uint16_t X86Cpu::Fetch16() {
    uint16_t val = m_memoryBus.Read16(m_regs.eip);
    m_regs.eip += 2;
    return val;
}

uint32_t X86Cpu::Fetch32() {
    uint32_t val = m_memoryBus.Read32(m_regs.eip);
    m_regs.eip += 4;
    return val;
}

void X86Cpu::Step() {
    uint8_t opcode = Fetch8();
    m_instructionCount++;

    switch (opcode) {
        // MOV EAX, imm32
        case 0xB8: {
            m_regs.eax = Fetch32();
            break;
        }
        // ADD EAX, imm32
        case 0x05: {
            m_regs.eax += Fetch32();
            break;
        }
        // INC EBX
        case 0x43: {
            m_regs.ebx++;
            break;
        }
        // JMP rel8 (relative short jump)
        case 0xEB: {
            int8_t rel = static_cast<int8_t>(Fetch8());
            m_regs.eip += rel;
            break;
        }
        default:
            // Log unsupported instruction and advance EIP safely for PoC stability
            XLOG_ERROR("Unsupported opcode: " + std::to_string(static_cast<int>(opcode)) + " at instruction #" + std::to_string(m_instructionCount));
            break;
    }
}

} // namespace Emulator::Core::XboxOriginal