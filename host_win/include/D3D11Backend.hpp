#pragma once
#include "IGraphicsBackend.hpp"
#include <d3d11.h>
#include <dxgi.h>

namespace Emulator::Host {

class D3D11Backend : public IGraphicsBackend {
public:
    bool Initialize(void* windowHandle) override;
    void Shutdown() override;
    void PresentFrame() override;
    bool UpdateFrameBuffer(const uint32_t* buffer, uint32_t width, uint32_t height) override;
    const char* GetName() const override { return "Direct3D 11"; }
private:
    std::unique_ptr<ID3D11Device> m_device;
    std::unique_ptr<ID3D11DeviceContext> m_context;
    std::unique_ptr<ID3D11RenderTargetView> m_rtv;
    std::unique_ptr<IDXGISwapChain> m_swapChain;
    std::unique_ptr<ID3D11Texture2D> m_frameTexture;
    std::unique_ptr<ID3D11ShaderResourceView> m_srv;
    uint32_t m_width = 1280, m_height = 720;
};

} // namespace Emulator::Host