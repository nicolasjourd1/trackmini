#ifndef BUFFER_H_
#define BUFFER_H_

#include <glad/glad.h>
#include <span>
#include <utility>

namespace trackmini::renderer {

enum class BufferUsage : GLenum
{
    StaticDraw = GL_STATIC_DRAW,   // upload once, read often by the GPU
    DynamicDraw = GL_DYNAMIC_DRAW, // updated often, read often by the GPU
    StreamDraw = GL_STREAM_DRAW,   // updated each frame
};

// Generic buffer (VBO,EBO,UBO...)

template<typename T>
class GpuBuffer
{
  public:
    GpuBuffer() noexcept { glCreateBuffers(1, &m_handle); }

    // initial upload
    // flags 0 for immutable or GL_DYNAMIC_STORAGE_BIT
    void allocate(std::span<T const> data,
                  BufferUsage usage,
                  GLbitfield flags = 0) noexcept
    {
        m_count = data.size();
        glNamedBufferStorage(m_handle,
                             static_cast<GLsizeiptr>(data.size_bytes()),
                             data.data(),
                             flags);
        m_usage = usage;
    }

    // allocate now, upload later
    void allocate_empty(std::size_t count, BufferUsage usage) noexcept
    {
        m_count = count;
        m_usage = usage;
        glNamedBufferStorage(m_handle,
                             static_cast<GLsizeiptr>(count * sizeof(T)),
                             nullptr,
                             GL_DYNAMIC_STORAGE_BIT);
    }

    // requires GL_DYANMIC_STORAGE_BIT
    void upload(std::span<T const> data, GLintptr offset_elements = 0) noexcept
    {
        glNamedBufferSubData(m_handle,
                             offset_elements * static_cast<GLintptr>(sizeof(T)),
                             static_cast<GLsizeiptr>(data.size_bytes()),
                             data.data());
    }

    [[nodiscard]] GLuint handle() const noexcept { return m_handle; }
    [[nodiscard]] std::size_t count() const noexcept { return m_count; }

    GpuBuffer(GpuBuffer const&) = delete;
    GpuBuffer& operator=(GpuBuffer const&) = delete;

    GpuBuffer(GpuBuffer&& other) noexcept
      : m_handle{ std::exchange(other.m_handle, 0) }
      , m_count{ std::exchange(other.m_count, 0) }
      , m_usage{ other.m_usage }
    {
    }

    GpuBuffer& operator=(GpuBuffer&& other) noexcept
    {
        if (this != &other) {
            glDeleteBuffers(1, &m_handle);
            m_handle = std::exchange(other.m_handle, 0);
            m_count = std::exchange(other.m_count, 0);
            m_usage = other.m_usage;
        }
        return *this;
    }

    ~GpuBuffer() { glDeleteBuffers(1, &m_handle); }

  private:
    GLuint m_handle{ 0 };
    std::size_t m_count{ 0 };
    BufferUsage m_usage{ BufferUsage::StaticDraw };
};

using VertexBuffer = GpuBuffer<float>;
using IndexBuffer = GpuBuffer<uint32_t>;

} // namespace trackmini::renderer

#endif /* BUFFER_H_ */
