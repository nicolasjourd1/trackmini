#include "editor/TrackRenderer.h"
#include <glad/glad.h>
#include <print>

namespace trackmini::editor {

static constexpr auto
block_color(track::BlockId id) noexcept -> std::tuple<float, float, float>
{
    switch (id) {
        case track::BlockId::Road:
            return { 0.6f, 0.6f, 0.6f };
        case track::BlockId::Boost:
            return { 0.2f, 0.8f, 1.0f };
        case track::BlockId::Jump:
            return { 1.0f, 0.7f, 0.1f };
        case track::BlockId::Finish:
            return { 0.1f, 1.0f, 0.3f };
        default:
            return { 1.0f, 0.0f, 1.0f };
    }
}

static renderer::Mesh
make_block_mesh(track::BlockId id) noexcept
{
    auto [r, g, b] = block_color(id);
    auto data = renderer::MeshData::make_cube(
      0.48f * kCellSize); // slightly smaller than a cell
    for (auto& v : data.vertices) {
        v.color = { r, g, b };
    }
    return renderer::Mesh{ data };
}

// grid mesh on ground (lignes X and Z)
static renderer::Mesh
make_grid_mesh(int half_extent) noexcept
{
    renderer::MeshData data;
    float const e = static_cast<float>(half_extent) * kCellSize;
    float const y = -0.01f;
    float const t = 0.03f; // line stroke

    auto add_line_x = [&](float z) {
        uint32_t base = static_cast<uint32_t>(data.vertices.size());
        math::Vec3f n{ 0, 1, 0 };
        math::Vec3f c{ 0.3f, 0.3f, 0.35f };
        data.vertices.push_back({ { -e, y, z - t }, n, c });
        data.vertices.push_back({ { e, y, z - t }, n, c });
        data.vertices.push_back({ { e, y, z + t }, n, c });
        data.vertices.push_back({ { -e, y, z + t }, n, c });
        data.indices.insert(
          data.indices.end(),
          { base, base + 1, base + 2, base, base + 2, base + 3 });
    };

    auto add_line_z = [&](float x) {
        uint32_t base = static_cast<uint32_t>(data.vertices.size());
        math::Vec3f n{ 0, 1, 0 };
        math::Vec3f c{ 0.3f, 0.3f, 0.35f };
        data.vertices.push_back({ { x - t, y, -e }, n, c });
        data.vertices.push_back({ { x + t, y, -e }, n, c });
        data.vertices.push_back({ { x + t, y, e }, n, c });
        data.vertices.push_back({ { x - t, y, e }, n, c });
        data.indices.insert(
          data.indices.end(),
          { base, base + 1, base + 2, base, base + 2, base + 3 });
    };

    for (int i = -half_extent; i <= half_extent; ++i) {
        float pos = static_cast<float>(i) * kCellSize;
        add_line_x(pos);
        add_line_z(pos);
    }

    return renderer::Mesh{ data };
}

TrackRenderer::TrackRenderer(renderer::ShaderProgram& shader) noexcept
  : m_shader{ &shader }
  , m_cursor_mesh{ []() {
      auto data = renderer::MeshData::make_cube(0.50f * kCellSize);
      for (auto& v : data.vertices)
          v.color = { 1.f, 1.f, 1.f };
      return renderer::Mesh{ data };
  }() }
  , m_grid_mesh{ make_grid_mesh(16) }
{
    for (auto id : {
           track::BlockId::Road,
           track::BlockId::Boost,
           track::BlockId::Jump,
           track::BlockId::Finish,
         }) {
        m_block_meshes.emplace(id, make_block_mesh(id));
    }
}

void
TrackRenderer::render(EditorState const& state,
                      renderer::CameraMatrices const& cam,
                      math::Vec3f light_dir) noexcept
{
    (void)cam;

    float ld[3] = { light_dir.x, light_dir.y, light_dir.z };
    float white[3] = { 1.f, 1.f, 1.f };
    m_shader->set_vec3("u_light_dir", ld);
    m_shader->set_vec3("u_light_color", white);

    // grid
    auto identity = math::Mat4f::identity();
    m_shader->set_mat4("u_model", identity.data());
    m_grid_mesh.draw();

    // placed blocks
    state.track().grid().for_each(
      [&](track::GridPos pos, track::BlockInstance blk) {
          auto [r, g, b] = block_color(blk.id);
          draw_block(pos, blk, r, g, b);
      });

    // cursor
    if (auto cursor = state.cursor_pos()) {
        // selected block color but lighter
        auto [r, g, b] = block_color(state.selected_block());

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(2.f);
        draw_block(*cursor,
                   { state.selected_block(), track::Rotation::R0 },
                   r * 1.4f,
                   g * 1.4f,
                   b * 1.4f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glLineWidth(1.f);
    }
}

void
TrackRenderer::draw_block(track::GridPos pos,
                          track::BlockInstance blk,
                          float r,
                          float g,
                          float b,
                          float /*alpha_scale*/
                          ) noexcept
{
    math::Vec3f center = grid_to_world_center(pos);
    center.y = static_cast<float>(pos.y) * kCellSize + kCellSize * 0.5f;

    // rotation around Y
    auto model = math::Mat4f::translation(center) *
                 math::Mat4f::rotation(math::Vec3f::up(),
                                       track::rotation_radians(blk.rotation));

    float color[3] = { r, g, b };
    m_shader->set_mat4("u_model", model.data());
    m_shader->set_vec3("u_tint", color);

    auto it = m_block_meshes.find(blk.id);
    if (it != m_block_meshes.end())
        it->second.draw();
}

} // namespace trackmini::editor