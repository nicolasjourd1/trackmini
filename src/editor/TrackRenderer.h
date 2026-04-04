#ifndef TRACKRENDERER_H_
#define TRACKRENDERER_H_

#include "editor/EditorState.h"
#include "math/Math.h"
#include "renderer/Camera.h"
#include "renderer/Mesh.h"
#include "renderer/Shader.h"
#include "renderer/UniformBuffer.h"
#include <memory>
#include <unordered_map>

namespace trackmini::editor {

class TrackRenderer
{
  public:
    explicit TrackRenderer(renderer::ShaderProgram& shader) noexcept;

    void render(EditorState const& state,
                renderer::CameraMatrices const& cam,
                math::Vec3f light_dir = { 0.6f, 1.f, 0.4f }) noexcept;

  private:
    void draw_block(track::GridPos pos,
                    track::BlockInstance blk,
                    float r,
                    float g,
                    float b,
                    float alpha_scale = 1.f) noexcept;

    renderer::ShaderProgram* m_shader;

    // one mesh cube per bloc type (different color)
    std::unordered_map<track::BlockId,
                       renderer::Mesh,
                       decltype([](track::BlockId b) {
                           return std::hash<uint16_t>{}(
                             static_cast<uint16_t>(b));
                       })>
      m_block_meshes;

    renderer::Mesh m_cursor_mesh; // semi-transparent cube for cursor
    renderer::Mesh m_grid_mesh;   // ground grid
};

} // namespace trackmini::editor

#endif /* TRACKRENDERER_H_ */
