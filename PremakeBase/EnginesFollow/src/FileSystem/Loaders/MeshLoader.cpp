#include "EnginePCH.h"

#include "MeshLoader.h"

using namespace Engine;

PlainData FileMesh::Serialize() {
	Engine::WriteStream out;
	int num_vtx = vtx.size();
	int num_idx = idx.size();
	bool hasnormals = vtx_normals.size() > 0;
	int num_uvchannels = uv_channels.size();
	bool hastanbitan = tangents.size() > 0;
	
	out.Add(&num_vtx);
	out.Add(&num_idx);
	out.Add(&hasnormals);
	out.Add(&num_uvchannels);
	out.Add(&hastanbitan);

	out.AddArr(vtx.data(), num_vtx);
	out.AddArr(idx.data(), num_idx);
	out.AddArr(vtx_normals.data(), vtx_normals.size());
	for (int i = 0; i < num_uvchannels; ++i) {
		int strleni = uv_channels[i].name.length();
		out.Add(&strleni);
		out.AddArr(uv_channels[i].name.c_str(), strleni);
		out.Add(&uv_channels[i].num_uv_coords);
		out.AddArr(uv_channels[i].data, num_vtx * uv_channels[i].num_uv_coords);
	}
	out.AddArr(tangents.data(), tangents.size());
	out.AddArr(bitangents.data(), bitangents.size());

	auto own = out.GetOwnership();
	PlainData ret;
	ret.Acquire(own.first, own.second);
	return ret;
}

void FileMesh::ParseBytes(TempIfStream& disk_mem) {
	ReadStream read;
	read.SetData(disk_mem.GetData().data, disk_mem.GetData().size);
	
	vtx.resize(read.GetV<int>());
	int num_vtx = vtx.size();
	idx.resize(read.GetV<int>());
	if (read.GetV<bool>()) vtx_normals.resize(num_vtx);
	uv_channels.resize(read.GetV<int>());
	if (read.GetV<bool>()) { tangents.resize(num_vtx); bitangents.resize(num_vtx); }
	
	read.GetArr(vtx.data(), num_vtx);
	read.GetArr(idx.data(), idx.size());
	read.GetArr(vtx_normals.data(), vtx_normals.size());
	for (int i = 0; i < uv_channels.size(); ++i) {
		uvs& uv = uv_channels[i];
		uv.name.resize(read.GetV<int>());
		read.GetArr(uv.name.c_str(), uv.name.size());
		uv.num_uv_coords = read.GetV<uint8_t>();
		uv.data = new float[num_vtx * uv.num_uv_coords];
		// new numero (valor)
		// new numero[cantitat]

		read.GetArr(uv.data, num_vtx * uv.num_uv_coords);
	}
	read.GetArr(tangents.data(), tangents.size());
	read.GetArr(bitangents.data(), bitangents.size());
}

void FileMesh::Unload_RAM() {
	for (int i = 0; i < uv_channels.size(); ++i)
		if (uv_channels[i].data != nullptr) {
			delete[] uv_channels[i].data, uv_channels[i].num_uv_coords * vtx.size() * sizeof(float);
			uv_channels[i].data = nullptr;
		}
}

uint32_t MeshLoader::ShouldILoad(const char* ext) {
	uint32_t ret = 0;
	if (!strcmp(ext, ".mesh"))
		ret = 1;

	return ret;
}

// Turn this into a default macro define
FileTaker_DefaultTryLoad(MeshLoader, FileMesh);