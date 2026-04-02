#include "renderer/Shader.h"

#include <fstream>
#include <print>
#include <sstream>
#include <utility>

namespace trackmini::renderer {

static auto
read_file(std::filesystem::path const& path)
  -> std::expected<std::string, ShaderError>
{
    std::ifstream file{ path };
    if (!file) {
        return std::unexpected(
          ShaderError{ "Cannot open shader file: " + path.string() });
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

auto
ShaderStage::from_source(GLenum type, std::string_view source)
  -> std::expected<ShaderStage, ShaderError>
{
    GLuint handle = glCreateShader(type);

    char const* src_ptr = source.data();
    auto const src_len = static_cast<GLint>(source.size());
    glShaderSource(handle, 1, &src_ptr, &src_len);
    glCompileShader(handle);

    GLint success{ 0 };
    glGetShaderiv(handle, GL_COMPILE_STATUS, &success);

    if (!success) {
        GLint log_len{ 0 };
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &log_len);
        std::string log(static_cast<std::size_t>(log_len), '\0');
        glGetShaderInfoLog(handle, log_len, nullptr, log.data());
        glDeleteShader(handle);
        return std::unexpected(
          ShaderError{ "Shader compilation failed:\n" + log });
    }

    return ShaderStage{ handle };
}

auto
ShaderStage::from_file(GLenum type, std::filesystem::path const& path)
  -> std::expected<ShaderStage, ShaderError>
{
    return read_file(path).and_then(
      [type](std::string source) { return from_source(type, source); });
}

ShaderStage::ShaderStage(ShaderStage&& other) noexcept
  : m_handle{ std::exchange(other.m_handle, 0) }
{
}

ShaderStage&
ShaderStage::operator=(ShaderStage&& other) noexcept
{
    if (this != &other) {
        glDeleteShader(m_handle);
        m_handle = std::exchange(other.m_handle, 0);
    }
    return *this;
}

ShaderStage::~ShaderStage()
{
    glDeleteShader(m_handle);
}

auto
ShaderProgram::from_stages(ShaderStage const& vert, ShaderStage const& frag)
  -> std::expected<ShaderProgram, ShaderError>
{
    GLuint handle = glCreateProgram();
    glAttachShader(handle, vert.handle());
    glAttachShader(handle, frag.handle());
    glLinkProgram(handle);

    GLint success{ 0 };
    glGetProgramiv(handle, GL_LINK_STATUS, &success);

    if (!success) {
        GLint log_len{ 0 };
        glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &log_len);
        std::string log(static_cast<std::size_t>(log_len), '\0');
        glGetProgramInfoLog(handle, log_len, nullptr, log.data());
        glDeleteProgram(handle);
        return std::unexpected(ShaderError{ "Shader link failed:\n" + log });
    }

    glDetachShader(handle, vert.handle());
    glDetachShader(handle, frag.handle());

    return ShaderProgram{ handle };
}

auto
ShaderProgram::from_files(std::filesystem::path const& vert_path,
                          std::filesystem::path const& frag_path)
  -> std::expected<ShaderProgram, ShaderError>
{
    auto vert = ShaderStage::from_file(GL_VERTEX_SHADER, vert_path);
    if (!vert)
        return std::unexpected(vert.error());

    auto frag = ShaderStage::from_file(GL_FRAGMENT_SHADER, frag_path);
    if (!frag)
        return std::unexpected(frag.error());

    return from_stages(*vert, *frag);
}

ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept
  : m_handle{ std::exchange(other.m_handle, 0) }
{
}

ShaderProgram&
ShaderProgram::operator=(ShaderProgram&& other) noexcept
{
    if (this != &other) {
        glDeleteProgram(m_handle);
        m_handle = std::exchange(other.m_handle, 0);
    }
    return *this;
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(m_handle);
}

GLint
ShaderProgram::uniform_location(std::string_view name) const noexcept
{
    return glGetUniformLocation(m_handle, std::string{ name }.c_str());
}

void
ShaderProgram::set_int(std::string_view name, int v) const noexcept
{
    glProgramUniform1i(m_handle, uniform_location(name), v);
}

void
ShaderProgram::set_float(std::string_view name, float v) const noexcept
{
    glProgramUniform1f(m_handle, uniform_location(name), v);
}

void
ShaderProgram::set_vec3(std::string_view name, float const* v) const noexcept
{
    glProgramUniform3fv(m_handle, uniform_location(name), 1, v);
}

void
ShaderProgram::set_mat4(std::string_view name, float const* v) const noexcept
{
    glProgramUniformMatrix4fv(m_handle, uniform_location(name), 1, GL_FALSE, v);
}

} // namespace trackmini::renderer