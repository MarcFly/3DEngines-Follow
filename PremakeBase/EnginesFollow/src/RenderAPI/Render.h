#pragma once

#include "../Globals.h"
#include <MathGeoLib.h>
#include "RenderMacros.h"

namespace Engine {
	struct EF_API VertAttrib {
		VertAttrib(const char* _name) {
			name = new char[128];
			size_t namelen = strlen(_name);
			memcpy(name, _name, 128);
			(namelen > 127) ? name[127] = '\0' : name[namelen] = '\0';
		}
		~VertAttrib() { delete[] name; }

		int id = -1;
		int var = EF_FLOAT;
		uint16_t num_components = 3;
		bool normalize = false;
		uint16_t var_size = 4;
		uint16_t offset = 0;

		char* name;
	};

	struct EF_API VTX_Buf
	{
		int draw_config;
		int draw_mode;
		uint32_t vao; // Vertex Attribute Object
		uint32_t vtx_id;
		uint32_t vtx_num;

		std::vector<VertAttrib> attributes;
		uint16_t vtx_size;
		
		uint16_t VertSize(); // Calculates it
		void SetInterleave();
		void SetBlock();
		uint16_t GetAttributeSize(const char* name);
		uint16_t GetAttributeSize(int id);
		void BindLocations();

		void Create();
		void SendToGPU(void* vtx_data);
		void FreeFromGPU();

		inline void Bind();
		inline void Draw();
	};

	struct EF_API Mesh {
		VTX_Buf vtx;

		uint32_t idx_id;
		uint32_t idx_num;

		uint32_t idx_var;
		uint8_t idx_var_size;

		void Create();
		void SendToGPU(void* vtx_data, void* idx_data);
		void FreeFromGPU();

		inline void Bind();
		inline void Draw();
	};

	struct EF_API TexAttrib {
		TexAttrib(const char* name, int32_t id, float* value, int num_values = 1);
		TexAttrib(const char* name, int32_t id, int* value, int num_values = 1);
		TexAttrib(const TexAttrib& ta) : isfloat(ta.isfloat), id(ta.id), fvalues(ta.fvalues),
			num_values(ta.num_values), name(32, ta.name.str) {}
		~TexAttrib();
		bool isfloat;
		uint32_t id;
		union {
			float* fvalues;
			int* ivalues;
		};
		int num_values;

		offload_str name;
	};

	struct EF_API Texture {
		uint32_t tex_id;
		int dimension_format;
		int use_format;
		int data_format;
		int var;
		uint16_t var_size;

		int w, h;
		int num_channels;

		std::vector<TexAttrib> attributes;

		void AddAttributeF(const char* name, int32_t id, float value);
		void AddAttributeI(const char* name, int32_t id, int value);

		void Create();
		void ApplyAttributes();
		void SendToGPU(void* pixels, const int mipmap_level = 0);
		void GenMipmaps();

		void FreeFromGPU();

		void Bind();
	};

	struct EF_API Uniform {
		Uniform(const char* _name) : name(64, _name) { hashid = simplehash(_name); }
		Uniform(const Uniform& u) : name(64, u.name.str), hashid(u.hashid), type(u.type),
			var_size(u.var_size), id(u.id) {}
		int id = -1;
		uint64_t hashid = UINT64_MAX;
		uint32_t type = 0;
		int var_size = 0;
		offload_str name;
	};

	struct EF_API Shader {
		int id;
		int type;

		void Create();
		void SetSource(const char* source);
		void Compile();
		void FreeFromGPU();
	};

	struct EF_API ShaderProgram {
		uint32_t id;
		uint32_t shaders[2]; // TODO: Geomtery, Tesselation, Compute,...
		std::vector<Uniform> uniforms;

		void Create();
		void AttachShader(Shader& shader);
		void Link();

		void CacheUniforms();

		void Bind();

		int GetUniform(const char* name);
		void SetBool(const int id, const bool value);
		void SetInt(const int id, const int value);
		void SetFloat(const int id, const float value);
		void SetFloat2(const int id, const float2 value);
		void SetFloat3(const int id, const float3& value);
		void SetFloat4(const int id, const float4& value);
		void SetMatrix4(const int id, const float* matrices, const bool transpose = false, const uint16_t num_matrices = 1);
	};

	struct EF_API Framebuffer {
		uint32_t id;
		int fb_type; // Read Write Both
		
		uint32_t renderbuffer_id = 0;
		int rb_type;

		uint32_t w, h;
		uint32_t depth_buffer_id = 0;
		int depth_type;

		uint32_t stencil_buffer_id = 0;
		int stencil_type;

		std::vector<Texture> attachments;

		void Create();
		void AddRenderbuffer(int use_format, int type, uint32_t w, uint32_t h);
		void AddTexAttachment(const uint32_t location, const uint32_t texture_id, const int mipmap_lvl = 0);
		void SetDrawBuffers();

		void Bind();
		void FreeFromGPU();
	};


	struct EF_API RenderAPI {
		// Requires the user to make a window contex / app context the base one
		static void InitForContext();

		static void GenBuffers(const uint32_t num, uint32_t* id_arr);
		static void GenVAOs(const uint32_t num, uint32_t* vao_arr);
		static void GenTextureIDs(const uint32_t num, uint32_t id_arr);
		static void SetActiveUnit(const uint8_t num); // 0 - 15

		static void Shader_CheckCompileLog(const int shader_id);
		static void ShaderProgram_CheckLinkLog(const int program_id);

		static void Framebuffer_CheckStatus();
		// Should have a framebuffer that is provided for it to render to specifically

		static void ClearFB(float4 rgba);
	};
};