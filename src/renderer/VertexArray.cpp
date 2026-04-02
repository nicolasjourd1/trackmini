#include "renderer/VertexArray.h"
#include <utility>

namespace trackmini::renderer {

VertexArray::VertexArray() noexcept
{
    glCreateVertexArrays(1, &m_handle);
}

void
VertexArray::add_vertex_buffer(GLuint vbo_handle,
                               std::span<VertexAttrib const> attribs,
                               uint32_t stride) noexcept
{
    uint32_t const binding = m_binding_index++;

    // associates the VBO to this binding in the VAO
    glVertexArrayVertexBuffer(
      m_handle, binding, vbo_handle, 0, static_cast<GLsizei>(stride));

    for (auto const& attr : attribs) {
        glEnableVertexArrayAttrib(m_handle, attr.location);

        glVertexArrayAttribFormat(m_handle,
                                  attr.location,
                                  attr.components,
                                  attr.type,
                                  attr.normalized ? GL_TRUE : GL_FALSE,
                                  attr.offset);

        glVertexArrayAttribBinding(m_handle, attr.location, binding);
    }
}

void
VertexArray::set_index_buffer(GLuint ebo_handle) noexcept
{
    glVertexArrayElementBuffer(m_handle, ebo_handle);
}

VertexArray::VertexArray(VertexArray&& other) noexcept
  : m_handle{ std::exchange(other.m_handle, 0) }
  , m_binding_index{ other.m_binding_index }
{
}

VertexArray&
VertexArray::operator=(VertexArray&& other) noexcept
{
    if (this != &other) {
        glDeleteVertexArrays(1, &m_handle);
        m_handle = std::exchange(other.m_handle, 0);
        m_binding_index = other.m_binding_index;
    }
    return *this;
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &m_handle);
}

} // namespace trackmini::renderer