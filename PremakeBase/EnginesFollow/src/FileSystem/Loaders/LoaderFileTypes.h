#pragma once

#include "Globals.h"
#include "FileSystem/DefaultFileTypes.h"
#include "RenderAPI/Render.h"
#include "FileSystem/FS.h"

namespace Engine {
	struct FileTexture : public FileVirtual {
		Texture gputex;
		size_t w, h;
		uint32_t deviltype;

		void Load(TempIfStream& disk_mem);
		void Unload();
		// PlainData Serialize(); // No savign back to disk textures
	};

	struct FileMaterial : public FileVirtual {
		std::vector<WDHandle<FileTexture>> textures;
		uint64_t texuserid = PCGRand64(); // ?
		void Load(TempIfStream& disk_mem);
		PlainData Serialize(); // No serialization for now
	};

	struct RAMMesh {
		// VTX design decision
		// Float3 pos,  float3 normal, float3 tangent float3 bitangent, float x uvs
		// All determined with descriptors
		std::shared_ptr<byte[]> vtx;
		VTX_Descriptor vtx_desc;

		uint8_t idx_size;
		uint32_t idx_var;
		uint32_t num_idx;
		std::shared_ptr<byte[]> idx;
	};

	struct FileMesh : public Engine::FileVirtual {
		Mesh gpumesh;
		std::shared_ptr<RAMMesh> mesh;

		PlainData Serialize();
		void Load(TempIfStream& disk_mem);
		void Unload();
	};
}