#ifndef MESH_H_
#define MESH_H_

#include "math/Vec3.h"
#include "renderer/Buffer.h"
#include "renderer/VertexArray.h"
#include <cstdint>
#include <vector>

namespace trackmini::renderer {

struct MeshVertex
{
    math::Vec3f position;
    math::Vec3f normal;
    math::Vec3f color;
};

struct MeshData
{
    std::vector<MeshVertex> vertices;
    std::vector<uint32_t> indices;

    [[nodiscard]] static MeshData make_cube(float half_size = 0.5f) noexcept;
    [[nodiscard]] static MeshData make_plane(float size = 1.f,
                                             int subdivs = 1) noexcept;
};

class Mesh
{
  public:
    explicit Mesh(MeshData const& data) noexcept;

    void draw() const noexcept;

    [[nodiscard]] std::size_t index_count() const noexcept
    {
        return m_index_count;
    }

    Mesh(Mesh const&) = delete;
    Mesh& operator=(Mesh const&) = delete;
    Mesh(Mesh&&) = default;
    Mesh& operator=(Mesh&&) = default;

  private:
    GpuBuffer<float> m_vbo;
    GpuBuffer<uint32_t> m_ebo;
    VertexArray m_vao;
    std::size_t m_index_count{ 0 };
};

} // namespace trackmini::renderer

#endif /* MESH_H_ */
