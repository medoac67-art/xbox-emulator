#pragma once
#include <cstdint>
#include <vector>

namespace XboxOriginal {

    class Nv2aRenderer {
    public:
        Nv2aRenderer();
        ~Nv2aRenderer() = default;

        void Initialize(uint32_t width, uint32_t height);
        void Reset();

        // Render a dynamic test pattern to the virtual framebuffer
        void RenderTestFrame(float timeInSeconds);

        // Accessors for the generated texture/framebuffer
        const uint8_t* GetFrameBuffer() const { return m_frameBuffer.data(); }
        uint32_t GetWidth() const { return m_width; }
        uint32_t GetHeight() const { return m_height; }

    private:
        uint32_t m_width;
        uint32_t m_height;
        std::vector<uint8_t> m_frameBuffer; // RGBA8
    };

} // namespace XboxOriginal
