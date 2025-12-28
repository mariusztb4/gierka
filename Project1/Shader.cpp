#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Shader.h"

Shader::Shader()
{

}

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    LoadFromFiles(vertexPath, fragmentPath);
}

static std::string LoadFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Failed to open shader file: " << path << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

Shader::~Shader()
{
    if (ID)
        glDeleteProgram(ID);
}

GLuint Shader::CompileShader(const std::string& source, GLenum type)
{
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char log[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, log);
        std::cerr << "Shader compile error:\n" << log << std::endl;
    }
    return shader;
}

void Shader::LoadFromFiles(const std::string& vertPath,
    const std::string& fragPath)
{
    std::cout << "=== LOADING SHADER ===" << std::endl;
    std::cout << "Vertex: " << vertPath << std::endl;
    std::cout << "Fragment: " << fragPath << std::endl;
    
    std::string vertSrc = LoadFile(vertPath);
    std::string fragSrc = LoadFile(fragPath);

    if (vertSrc.empty() || fragSrc.empty()) {
        std::cerr << "ERROR: Shader source is empty!" << std::endl;
        return;
    }
    
    std::cout << "Vertex shader length: " << vertSrc.length() << std::endl;
    std::cout << "Fragment shader length: " << fragSrc.length() << std::endl;

    if (ID)
        glDeleteProgram(ID);

    GLuint vs = CompileShader(vertSrc, GL_VERTEX_SHADER);
    GLuint fs = CompileShader(fragSrc, GL_FRAGMENT_SHADER);

    ID = glCreateProgram();
    glAttachShader(ID, vs);
    glAttachShader(ID, fs);
    glLinkProgram(ID);

    GLint success;
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        char log[1024];
        glGetProgramInfoLog(ID, 1024, nullptr, log);
        std::cerr << "Shader link error:\n" << log << std::endl;
    } else {
        std::cout << "✅ Shader linked successfully! Program ID: " << ID << std::endl;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
}

void Shader::Use() const
{
    glUseProgram(ID);
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::SetMat4(const std::string& name, const glm::mat4& mat)
{
    glUniformMatrix4fv(
        glGetUniformLocation(ID, name.c_str()),
        1,
        GL_FALSE,
        glm::value_ptr(mat)
    );
}

void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetVec2(const std::string& name, const glm::vec2& value) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), value.x, value.y);
}

void Shader::SetVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()),
        value.x, value.y, value.z, value.w);
}

void Shader::SetFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}