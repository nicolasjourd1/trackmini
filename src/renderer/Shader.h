#ifndef SHADER_H_
#define SHADER_H_

#include <expected>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

#include <glad/glad.h>

namespace trackmini::renderer {

struct ShaderError
{
    std::string message;
};

class ShaderStage
{
  public:
    [[nodiscard]]
    static auto from_source(GLenum type, std::string_view source)
      -> std::expected<ShaderStage, ShaderError>;

    [[nodiscard]]
    static auto from_file(GLenum type, std::filesystem::path const& path)
      -> std::expected<ShaderStage, ShaderError>;

    ShaderStage(ShaderStage const&) = delete;
    ShaderStage& operator=(ShaderStage const&) = delete;
    ShaderStage(ShaderStage&&) noexcept;
    ShaderStage& operator=(ShaderStage&&) noexcept;
    ~ShaderStage();

    [[nodiscard]] GLuint handle() const noexcept { return m_handle; }

  private:
    explicit ShaderStage(GLuint handle) noexcept
      : m_handle{ handle }
    {
    }
    GLuint m_handle{ 0 };
};

class ShaderProgram
{
  public:
    [[nodiscard]]
    static auto from_stages(ShaderStage const& vert, ShaderStage const& frag)
      -> std::expected<ShaderProgram, ShaderError>;

    [[nodiscard]]
    static auto from_files(std::filesystem::path const& vert_path,
                           std::filesystem::path const& frag_path)
      -> std::expected<ShaderProgram, ShaderError>;

    ShaderProgram(ShaderProgram const&) = delete;
    ShaderProgram& operator=(ShaderProgram const&) = delete;
    ShaderProgram(ShaderProgram&&) noexcept;
    ShaderProgram& operator=(ShaderProgram&&) noexcept;
    ~ShaderProgram();

    void bind() const noexcept { glUseProgram(m_handle); }
    void unbind() const noexcept { glUseProgram(0); }

    [[nodiscard]] GLuint handle() const noexcept { return m_handle; }

    // Uniforms
    // temporary, will use UBO later
    void set_int(std::string_view name, int v) const noexcept;
    void set_float(std::string_view name, float v) const noexcept;
    void set_vec3(std::string_view name, float const* v) const noexcept;
    void set_mat4(std::string_view name, float const* v) const noexcept;

  private:
    explicit ShaderProgram(GLuint handle) noexcept
      : m_handle{ handle }
    {
    }
    [[nodiscard]] GLint uniform_location(std::string_view name) const noexcept;

    GLuint m_handle{ 0 };
};

} // namespace trackmini::renderer

#endif /* SHADER_H_ */
