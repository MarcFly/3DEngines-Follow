#pragma once

#include "Globals.h"
#include "../FS.h"
#include "RenderAPI/Render.h"
#include "Renderer/Renderer.h"
namespace Engine {

	struct FileMesh : public Engine::FileVirtual {
		std::vector<float3> vtx;
		std::vector<float3> vtx_normals;

		struct uvs {
			std::string name;
			uint8_t num_uv_coords;
			float* data;
		};

		std::vector<uvs> uv_channels;

		std::vector<float3> tangents;
		std::vector<float3> bitangents;
		// TODO: Animation bones?

		uint8_t idx_size;
		uint32_t idx_var;
		std::vector<uint32_t> idx;

		PlainData Serialize();
		void Load(TempIfStream& disk_mem);
		void Unload();
	};

	struct MeshLoader : public FileTaker {
		uint32_t ShouldILoad(const char* extension);
		std::shared_ptr<FileVirtual> TryLoad(TempIfStream& raw_bytes, const uint32_t internaltype);
	};
};