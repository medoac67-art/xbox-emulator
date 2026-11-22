#include "../include/SystemDevices.hpp"
#include "Logger.hpp"

namespace XboxOriginal {

    SystemDevices::SystemDevices()
        : m_picMask(0), m_pitCounter(0), m_smbusStatus(0),
          m_controllerButtons(0), m_leftStickX(0.0f), m_leftStickY(0.0f) {
        m_eepromData.resize(256, 0x00);
    }

    void SystemDevices::Initialize() {
        Reset();
        LOG_INFO("SystemDevices initialized (PIC, PIT, SMBus, EEPROM, Controller stub).");
    }

    void SystemDevices::Reset() {
        m_picMask = 0xFF;
        m_pitCounter = 0;
        m_smbusStatus = 0x80; // Ready flag
    }

    uint32_t SystemDevices::ReadPort(uint16_t port, uint8_t size) {
        // Basic stub for I/O ports
        if (port >= 0xC000 && port < 0xC010) {
            return m_smbusStatus;
        }
        return 0;
    }

    void SystemDevices::WritePort(uint16_t port, uint32_t value, uint8_t size) {
        if (port >= 0xC000 && port < 0xC010) {
            m_smbusStatus = (uint8_t)value;
        }
    }

    void SystemDevices::SetControllerState(uint32_t buttons, float leftStickX, float leftStickY) {
        m_controllerButtons = buttons;
        m_leftStickX = leftStickX;
        m_leftStickY = leftStickY;
    }

} // namespace XboxOriginal
