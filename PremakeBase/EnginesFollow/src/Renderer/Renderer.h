#pragma once

#include "../Globals.h"
#include "Core/Layer.h"
#include "FileSystem/FS.h"
#include "../RenderAPI/Render.h"
#include "../RenderAPI/RenderMacros.h"


namespace Engine {

	struct RenderState {
		RenderState(); // Utility constructor/destructure, scoped state saving...
		~RenderState();

		int32_t blend_equation_rgb, blend_equation_alpha;

		bool blend_func_separate = false;
		int32_t blend_func_src_rgb, blend_func_dst_rgb;
		int32_t blend_func_src_alpha, blend_func_dst_alpha;

		int32_t polygon_mode[2];
		bool blend, cull_faces, depth_test, scissor_test;

		int32_t draw_framebuffer, read_framebuffer, framebuffer;

		int32_t vp[4];

		void SetUp();
		void BackUp();

	private:
		bool backed_up = false;
	};
	
	// TODO: Swap unordered maps for slotmaps
	struct RenderMesh {
		std::vector<float4x4> transforms;
		uint64_t mesh_id;
		// Uniforms?
	};
	
	struct RenderMaterial {
		std::unordered_map<uint64_t, RenderMesh> meshes;
		uint64_t material; // 1 Texture for now
	};

	struct RenderProgram {
		uint64_t program;
		uint64_t state;
		
		std::vector<uint64_t> framebuffers;
		std::unordered_map<uint64_t, RenderMaterial> meshes;
	};

	enum TextureTypes {
		NOTEX = 0,
		DIFFUSE, SPECULAR, AMBIENT, EMISSIVE, HEIGHT, NORMALS,
		SHININESS, OPACITY, DISPLACEMENT, LIGHTMAP, REFLECTION,
		BASE_COLOR, NORMAL_CAM, EMISSION_COLOR, METALNESS,
		ROUGHNESS, OCCLUSION, TEXUNKNONW, SHEEN, CLEARCOAT, TRANSMISSION
	};

	struct Material {
		// Should be built from a Shaderprogram
		// understanding the variable it uses
		// for now, only textures
		std::vector<uint64_t> textures;
	};

	struct Renderer : public Layer, public FileTaker {
		void OnAttach(); // Register to submissions and load requests
		void OnDetach();

		void PreUpdate();
		void Update();
		void PostUpdate();

		// Reacts to submissions
		// Submission { RenderStage, ShaderProgram, Material, Texture, Mesh, Transform}
		// Each Submission helps build the rendergraph

		std::unordered_map<uint64_t, RenderProgram> stages;

		std::unordered_map<uint64_t, Framebuffer> framebuffers;
		std::unordered_map<uint64_t, ShaderProgram> shaderprograms;
		std::unordered_map<uint64_t, Mesh> meshes;
		//std::unordered_map<uint64_t, Material> materials;
		std::unordered_map<uint64_t, Texture> textures;
		std::unordered_map<uint64_t, VTX_Buf> vtx_meshes;
		
		int32_t default_fb;

		DECL_DYN_ENGINE_EV_FUNS(RequestFramebuffers_EventFun);
	};


}