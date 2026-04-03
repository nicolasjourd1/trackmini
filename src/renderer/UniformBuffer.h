#ifndef UNIFORMBUFFER_H_
#define UNIFORMBUFFER_H_

#include <cstring>
#include <glad/glad.h>
#include <utility>

namespace trackmini::renderer {

template<typename T>
class UniformBuffer
{
  public:
    UniformBuffer() noexcept
    {
        glCreateBuffers(1, &m_handle);
        glNamedBufferStorage(
          m_handle, sizeof(T), nullptr, GL_DYNAMIC_STORAGE_BIT);
    }

    void upload(T const& data) noexcept
    {
        glNamedBufferSubData(m_handle, 0, sizeof(T), &data);
    }

    void bind_to_slot(GLuint binding_point) const noexcept
    {
        glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, m_handle);
    }

    [[nodiscard]] GLuint handle() const noexcept { return m_handle; }

    UniformBuffer(UniformBuffer const&) = delete;
    UniformBuffer& operator=(UniformBuffer const&) = delete;

    UniformBuffer(UniformBuffer&& o) noexcept
      : m_handle{ std::exchange(o.m_handle, 0) }
    {
    }

    UniformBuffer& operator=(UniformBuffer&& o) noexcept
    {
        if (this != &o) {
            glDeleteBuffers(1, &m_handle);
            m_handle = std::exchange(o.m_handle, 0);
        }
        return *this;
    }

    ~UniformBuffer() { glDeleteBuffers(1, &m_handle); }

  private:
    GLuint m_handle{ 0 };
};

} // namespace trackmini::renderer

#endif /* UNIFORMBUFFER_H_ */
