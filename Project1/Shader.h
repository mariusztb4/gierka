#pragma once
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

class Shader
{
public:
	string LoadShaderSource(const string& filePath);

	Shader();
	Shader(const char* vertexPath, const char* fragmentPath);
	~Shader();

	void Use() const;
	unsigned int GetProgram() const;

	unsigned int CompileShader(const std::string& source, unsigned int shaderType);
	unsigned int ID;

	void LoadFromFiles(const std::string& vertPath,
		const std::string& fragPath);

	void SetVec3(const std::string& name, const glm::vec3& value) const;
	void SetVec2(const std::string& name, const glm::vec2& value) const;
	void SetVec4(const std::string& name, const glm::vec4& value) const;
	void SetMat4(const std::string& name, const glm::mat4& mat);
	void SetFloat(const std::string& name, float value) const;
	void setInt(const std::string& name, int value) const;
	void CheckCompileErrors(unsigned int shader, std::string type);

private:
	unsigned int program;
};