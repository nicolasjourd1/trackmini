#ifndef VERTEXARRAY_H_
#define VERTEXARRAY_H_

#include <cstdint>
#include <glad/glad.h>
#include <span>

namespace trackmini::renderer {

struct VertexAttrib
{
    uint32_t location;
    int components;
    GLenum type;
    bool normalized;
    uint32_t offset;
};

class VertexArray
{
  public:
    VertexArray() noexcept;

    // associates a VBO to this VAO and defines attributes
    // stride : size of a vertex in bytes
    void add_vertex_buffer(GLuint vbo_handle,
                           std::span<VertexAttrib const> attribs,
                           uint32_t stride) noexcept;

    // associates EBO (index buffer) with this VAO
    void set_index_buffer(GLuint ebo_handle) noexcept;

    void bind() const noexcept { glBindVertexArray(m_handle); }
    void unbind() const noexcept { glBindVertexArray(0); }

    [[nodiscard]] GLuint handle() const noexcept { return m_handle; }

    VertexArray(VertexArray const&) = delete;
    VertexArray& operator=(VertexArray const&) = delete;
    VertexArray(VertexArray&&) noexcept;
    VertexArray& operator=(VertexArray&&) noexcept;
    ~VertexArray();

  private:
    GLuint m_handle{ 0 };
    uint32_t m_binding_index{ 0 }; // next possible binding spot
};

} // namespace trackmini::renderer

#endif /* VERTEXARRAY_H_ */
