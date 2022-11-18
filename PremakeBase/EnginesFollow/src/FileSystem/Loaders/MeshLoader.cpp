#include "EnginePCH.h"

#include "MeshLoader.h"
#include "LoaderFileTypes.h"

using namespace Engine;

PlainData FileMesh::Serialize() {
	Engine::WriteStream out;

	int num_vtx = mesh->vtx_desc.vtx_num;
	int normalsize = mesh->vtx_desc.GetAttributeSize("normal");
	int tansize = mesh->vtx_desc.GetAttributeSize("tangent");
	
	int uv_start = INT32_MAX;
	for(int i = 0; i < mesh->vtx_desc.attributes.size(); ++i)
		if (!strcmp(mesh->vtx_desc.attributes[i].name.str.get(), "uv0")) {
			uv_start = i;
			break;
		}
	int num_uvchannels = 0;
	if (uv_start != INT32_MAX) {
		num_uvchannels = mesh->vtx_desc.attributes.size() - uv_start;
	}
	
	// Index info
	out.Add(&mesh->num_idx); 
	out.Add(&mesh->idx_var);
	out.Add(&mesh->idx_size); 
	
	// Vertex info
	out.Add(&num_vtx);
	out.Add(&normalsize);
	out.Add(&tansize); // If it has tangents, it has bitangents
	out.Add(&num_uvchannels);
	for (int i = 0; i < num_uvchannels; ++i) {
		out.Add(&mesh->vtx_desc.attributes[uv_start].num_components);
	}

	// Index data
	out.AddArr(mesh->idx.get(), mesh->num_idx * mesh->idx_size);
	// Vertex Data
	out.AddArr(mesh->vtx.get(), num_vtx * mesh->vtx_desc.VertSize());

	auto own = out.GetOwnership();
	return PlainData(own.first, own.second);
}

void FileMesh::Load(TempIfStream& disk_mem) {
	ReadStream read;
	read.SetData(disk_mem.bytes.get(), disk_mem.size);
	mesh.reset();
	mesh = std::make_shared<RAMMesh>();
	mesh->num_idx = read.GetV<uint32_t>(); // num indices
	mesh->idx_var = read.GetV<uint32_t>(); // idx_var;
	mesh->idx_size = read.GetV<uint8_t>(); // idx_varsize
	//mesh->idx = std::shared_ptr<byte[]>(new byte[mesh->num_idx * mesh->idx_size]);
	
	VTX_Buf& v = gpumesh.vtx;
	v.draw_mode = EF_STATIC_DRAW;
	v.draw_config = EF_TRIANGLES;

	size_t vtx_bufsize = 0;
	v.vtx_num = read.GetV<int>();
	v.vtx_desc.attributes.push_back(VertAttrib("position"));
	vtx_bufsize += v.vtx_num * sizeof(float3); // num vertices	
	
	if (read.GetV<int>() > 0) { // has normals {
		vtx_bufsize += v.vtx_num * sizeof(float3);
		v.vtx_desc.attributes.push_back(VertAttrib("normal"));
	}
	
	if (read.GetV<int>() > 0) { // has tangent and bitangents
		vtx_bufsize += v.vtx_num * 2 * sizeof(float3);
		v.vtx_desc.attributes.push_back(VertAttrib("tangents"));
		v.vtx_desc.attributes.push_back(VertAttrib("bitangents"));
	}

	// uv channel information
	int num_uv_channels = read.GetV<int>(); // number of uvchannels
	std::vector<uint16_t> comps_per_channel;
	for (int i = 0; i < num_uv_channels; ++i) {
		static char uvchname[8];
		snprintf(uvchname, sizeof(uvchname), "uv%d", i);
		v.vtx_desc.attributes.push_back(VertAttrib(uvchname));

		comps_per_channel.push_back(read.GetV<uint16_t>()); // number of components per uvchannel
		vtx_bufsize += v.vtx_num * comps_per_channel.back() * sizeof(float);

	}

	byte* idx_pointer = read.pointer;
	byte* vtx_pointer = read.pointer + (mesh->idx_size * mesh->num_idx);

	// After reading the header and indices, now only data about vertices are left...
	v.vtx_desc.SetBlock();

	gpumesh.idx_num = mesh->num_idx;
	gpumesh.idx_var_size = mesh->idx_size;
	gpumesh.idx_var = mesh->idx_var;

	gpumesh.Create();
	gpumesh.Bind();
	gpumesh.SendToGPU(idx_pointer, vtx_pointer);
	// TODO: set as interleaved...
}

void FileMesh::Unload() {
	gpumesh.FreeFromGPU();
}

uint32_t MeshLoader::ShouldILoad(const char* ext) {
	uint32_t ret = 0;
	if (!strcmp(ext, ".mesh"))
		ret = 1;

	return ret;
}

// Turn this into a default macro define
FileTaker_DefaultTryLoad(MeshLoader, FileMesh);