#include "EnginePCH.h"
#include "Core/Logger.h"
#include "glad/glad.h"
#include "Render.h"
#include "RenderMacros.h"

using namespace Engine;

//==================================================================
// SHADERS
//==================================================================

void Shader::Create() {
	id = glCreateShader(type);
}

void Shader::SetSource(const char* source) {
	// TODO: shader headers based on API (GLES...)
	glShaderSource(id, 1, &source, NULL);
}

void Shader::Compile() {
	glCompileShader(id);

	//Renderer::Shader_CheckCompileLog(id);
}

void RenderAPI::Shader_CheckCompileLog(const int id) {
	int success;
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (success) {
		Engine_INFO("Shader {} compiled successfully", id);
	}
	else {
		char infolog[512];
		glGetShaderInfoLog(id, 512, NULL, infolog);
		Engine_WARN("Shader {} failed to compile: {}", id, infolog);
	}
}

void Shader::FreeFromGPU() {
	glDeleteShader(id); 
}

//==================================================================
// SHADER PROGRAMS
//==================================================================

void ShaderProgram::Create() {
	id = glCreateProgram();
}

void ShaderProgram::AttachShader(Shader& shader) {
	int pos = -1 + (shader.type == EF_VERTEX_SHADER) * 1 + (shader.type == EF_FRAGMENT_SHADER) * 2;
	glAttachShader(id, shader.id);

	shaders[pos] = shader.id;
}

void ShaderProgram::Link() {
	glLinkProgram(id);
}

void RenderAPI::ShaderProgram_CheckLinkLog(const int program_id) {
	int success;
	glGetProgramiv(program_id, GL_LINK_STATUS, &success);
	if (success) {
		Engine_INFO("ShaderProgram {} linked successfuly.", program_id);
	}
	else {
		char infolog[512];
		glGetProgramInfoLog(program_id, 512, NULL , infolog);
		Engine_WARN("ShaderProgram {} failed to link: {}", program_id, infolog);
	}
}

void ShaderProgram::CacheUniforms() {
	int count;

	glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &count);
	uniforms.clear();
	static int l; // Length???
	for (int i = 0; i < count; ++i) {
		uniforms.push_back(Uniform("noname"));
		Uniform& u = uniforms.back();
		u.id = i;
		glGetActiveUniform(id, i, u.name.size, &l, &u.var_size, &u.type, u.name.str.get());
		u.hashid = simplehash(u.name.str.get());
	}
}

void ShaderProgram::Bind() {
	glUseProgram(id);
}


int ShaderProgram::GetUniform(const char* name) {
	uint64_t hashid = simplehash(name);
	for (const Uniform& u : uniforms)
		if (hashid == u.hashid)
			return u.id;

	return -1;
}

void ShaderProgram::SetBool(const int id, const bool value) {
	glUniform1i(id, value);
}
void ShaderProgram::SetInt(const int id, const int value) {
	glUniform1i(id, value);
}
void ShaderProgram::SetFloat(const int id, const float value) {
	glUniform1f(id, value);
}
void ShaderProgram::SetFloat2(const int id, const float2 value) {
	glUniform2f(id, value.x, value.y);
}
void ShaderProgram::SetFloat3(const int id, const float3& v) {
	glUniform3f(id, v.x, v.y, v.z);
}
void ShaderProgram::SetFloat4(const int id, const float4& v) {
	glUniform4f(id, v.x, v.y, v.z, v.w);
}
void ShaderProgram::SetMatrix4(const int id, const float* matrices, const bool transpose, const uint16_t num_matrices) {
	glUniformMatrix4fv(id, num_matrices, transpose, matrices);
}
