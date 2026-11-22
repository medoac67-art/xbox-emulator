#pragma once
#include <cstdint>
#include <vector>

namespace XboxOriginal {

    class SystemDevices {
    public:
        SystemDevices();
        ~SystemDevices() = default;

        void Initialize();
        void Reset();

        // I/O Port operations
        uint32_t ReadPort(uint16_t port, uint8_t size);
        void WritePort(uint16_t port, uint32_t value, uint8_t size);

        // Controller input mapping
        void SetControllerState(uint32_t buttons, float leftStickX, float leftStickY);

    private:
        // PIC / PIT state
        uint8_t m_picMask;
        uint32_t m_pitCounter;

        // SMBus / EEPROM state
        std::vector<uint8_t> m_eepromData;
        uint8_t m_smbusStatus;

        // Controller state
        uint32_t m_controllerButtons;
        float m_leftStickX;
        float m_leftStickY;
    };

} // namespace XboxOriginal
