#include "renderer/Mesh.h"
#include <array>
#include <cstring>
#include <glad/glad.h>

namespace trackmini::renderer {

MeshData
MeshData::make_cube(float h) noexcept
{
    MeshData d;
    d.vertices.reserve(24);
    d.indices.reserve(36);

    auto face = [&](math::Vec3f p0,
                    math::Vec3f p1,
                    math::Vec3f p2,
                    math::Vec3f p3,
                    math::Vec3f n,
                    math::Vec3f color) {
        uint32_t base = static_cast<uint32_t>(d.vertices.size());
        d.vertices.push_back({ p0, n, color });
        d.vertices.push_back({ p1, n, color });
        d.vertices.push_back({ p2, n, color });
        d.vertices.push_back({ p3, n, color });
        d.indices.insert(
          d.indices.end(),
          { base + 0, base + 1, base + 2, base + 0, base + 2, base + 3 });
    };

    // +X (red)
    face({ h, -h, -h },
         { h, h, -h },
         { h, h, h },
         { h, -h, h },
         { 1, 0, 0 },
         { 0.9f, 0.3f, 0.3f });
    // -X (cyan)
    face({ -h, -h, h },
         { -h, h, h },
         { -h, h, -h },
         { -h, -h, -h },
         { -1, 0, 0 },
         { 0.3f, 0.9f, 0.9f });
    // +Y (green)
    face({ -h, h, -h },
         { -h, h, h },
         { h, h, h },
         { h, h, -h },
         { 0, 1, 0 },
         { 0.3f, 0.9f, 0.3f });
    // -Y (magenta)
    face({ -h, -h, h },
         { -h, -h, -h },
         { h, -h, -h },
         { h, -h, h },
         { 0, -1, 0 },
         { 0.9f, 0.3f, 0.9f });
    // +Z (blue)
    face({ -h, -h, h },
         { h, -h, h },
         { h, h, h },
         { -h, h, h },
         { 0, 0, 1 },
         { 0.3f, 0.3f, 0.9f });
    // -Z (yellow)
    face({ h, -h, -h },
         { -h, -h, -h },
         { -h, h, -h },
         { h, h, -h },
         { 0, 0, -1 },
         { 0.9f, 0.9f, 0.3f });

    return d;
}

MeshData
MeshData::make_plane(float size, int subdivs) noexcept
{
    MeshData d;
    float const step = size / static_cast<float>(subdivs);
    float const half = size / 2.f;

    for (int j = 0; j <= subdivs; ++j) {
        for (int i = 0; i <= subdivs; ++i) {
            float x = -half + static_cast<float>(i) * step;
            float z = -half + static_cast<float>(j) * step;
            d.vertices.push_back(
              { { x, 0.f, z }, { 0.f, 1.f, 0.f }, { 0.6f, 0.6f, 0.6f } });
        }
    }

    int const w = subdivs + 1;
    for (int j = 0; j < subdivs; ++j) {
        for (int i = 0; i < subdivs; ++i) {
            uint32_t tl = static_cast<uint32_t>(j * w + i);
            uint32_t tr = tl + 1;
            uint32_t bl = static_cast<uint32_t>((j + 1) * w + i);
            uint32_t br = bl + 1;
            d.indices.insert(d.indices.end(), { tl, bl, tr, tr, bl, br });
        }
    }
    return d;
}

Mesh::Mesh(MeshData const& data) noexcept
  : m_index_count{ data.indices.size() }
{
    std::vector<float> flat;
    flat.reserve(data.vertices.size() * 9);
    for (auto const& v : data.vertices) {
        flat.insert(flat.end(),
                    {
                      v.position.x,
                      v.position.y,
                      v.position.z,
                      v.normal.x,
                      v.normal.y,
                      v.normal.z,
                      v.color.x,
                      v.color.y,
                      v.color.z,
                    });
    }

    m_vbo.allocate(flat, BufferUsage::StaticDraw);
    m_ebo.allocate(data.indices, BufferUsage::StaticDraw);

    constexpr uint32_t kStride = 9 * sizeof(float);
    static constexpr std::array<VertexAttrib, 3> kAttribs{ {
      { 0, 3, GL_FLOAT, false, 0 * 3 * sizeof(float) }, // position
      { 1, 3, GL_FLOAT, false, 1 * 3 * sizeof(float) }, // normal
      { 2, 3, GL_FLOAT, false, 2 * 3 * sizeof(float) }, // color
    } };

    m_vao.add_vertex_buffer(m_vbo.handle(), kAttribs, kStride);
    m_vao.set_index_buffer(m_ebo.handle());
}

void
Mesh::draw() const noexcept
{
    m_vao.bind();
    glDrawElements(GL_TRIANGLES,
                   static_cast<GLsizei>(m_index_count),
                   GL_UNSIGNED_INT,
                   nullptr);
}

} // namespace trackmini::renderer