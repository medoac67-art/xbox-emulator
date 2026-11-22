#pragma once
#include <cstdint>
#include <memory>
#include <string>

namespace Emulator::Host {

/**
 * @brief Abstract interface for GPU backends (D3D11/D3D12).
 */
class IGraphicsBackend {
public:
    virtual ~IGraphicsBackend() = default;
    virtual bool Initialize(void* windowHandle) = 0;
    virtual void Shutdown() = 0;
    virtual void PresentFrame() = 0;
    virtual bool UpdateFrameBuffer(const uint32_t* buffer, uint32_t width, uint32_t height) = 0;
    virtual const char* GetName() const = 0;
};

} // namespace Emulator::Host