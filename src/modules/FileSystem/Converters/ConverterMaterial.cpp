#include "../Converters.h"
#include <DevIL/include/IL/il.h>
#include <DevIL/include/IL/ilu.h>
#include <src/modules/Render/RendererTypes.h>
#include "../ModuleFS.h"

uint32_t ExtensionToDevILType_Convert(const char* ext) {
	uint32_t ret = 0;
	if (strcmp(ext, ".png") == 0 || strcmp(ext, ".PNG") == 0)
		ret = IL_PNG;
	else if (strcmp(ext, ".tiff") == 0 || strcmp(ext, ".TIFF") == 0 || strcmp(ext, ".TIF") == 0 || strcmp(ext, ".tif") == 0)
		ret = IL_TIF;
	else if (strcmp(ext, ".tga") == 0 || strcmp(ext, ".TGA") == 0)
		ret = IL_TGA;

	return ret;
}

uint32_t ExtensionToDevILType_Import(const char* ext) {
	uint32_t ret = 0;
	if (strcmp(ext, ".dds") == 0 || strcmp(ext, ".DDS") == 0)
		ret = IL_DDS;

	return ret;
}

WatchedData TryConvertTexture(const TempIfStream& file) {
	WatchedData ret;
	const char* ext = strrchr(file.path.c_str(), '.');
	
	const char* filename = FileName(file.path.c_str());
	size_t filename_len = strlen(filename);

	ILenum tex_type = ExtensionToDevILType_Convert(ext);
	if ( tex_type != 0) {
		ret.pd = ConvertDevILTexture(file.GetData(), tex_type);
		ret.uid = PCGRand();

		ret.str_len = FitString(ret.path, "Assets/Textures/%s.dds", std::string(filename).substr(0, filename_len - 4).c_str());
		ret.loaded = true;
	}

	return ret;
}

WatchedData TryImportTexture(const TempIfStream& file) {
	WatchedData ret;
	const char* ext = strrchr(file.path.c_str(), '.');

	const char* filename = FileName(file.path.c_str());
	size_t filename_len = strlen(filename);

	ILenum tex_type = ExtensionToDevILType_Import(ext);
	if (tex_type != 0) {
		ret.pd = ImportDevILTexture(file.GetData(), tex_type);
		ret.uid = PCGRand();

		ret.str_len = FitString(ret.path, "%s", file.path.c_str());
		ret.loaded = true;
	}

	return ret;
}

PlainData ConvertDevILTexture(const PlainData& pd, uint32_t tex_type) {
	PlainData ret;
	
	ILuint id_img = ilGenImage();
	ilBindImage(id_img);
	bool success = ilLoadL(tex_type, pd.data, pd.size);
	//Texture* tex = nullptr;

	if (success && ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE)) {
		
		//tex = new Texture();
		//SetPlainData(ret, tex->bytes, sizeof(Texture));
		
		ILinfo info;
		iluGetImageInfo(&info);
		// Only use info for size, other is when loading it!
		//if (info.Origin != IL_ORIGIN_UPPER_LEFT) 
			//iluFlipImage();
		//tex->format = GL_RGBA;
		//tex->unit_size = 1;
		//tex->unit_type = GL_UNSIGNED_BYTE;
		//tex->size = info.SizeOfData;
		//tex->bytes = new char[tex->size];
		
		//tex->w = info.Width;
		//tex->h = info.Height;		
		ret.data = new char[info.SizeOfData]; // Way bigger than needed, raw texture...
		ret.size = ilSaveL(IL_DDS, ret.data, info.SizeOfData);
	}

	ilDeleteImage(id_img);

	return ret;
}

PlainData ImportDevILTexture(const PlainData& pd, uint32_t tex_type) {
	PlainData ret;

	ILuint id_img = ilGenImage();
	ilBindImage(id_img);
	bool success = ilLoadL(tex_type, pd.data, pd.size);
	Texture* tex = nullptr;

	if (success && ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE)) {

		tex = new Texture();
		SetPlainData(ret, tex, sizeof(Texture));

		ILinfo info;
		iluGetImageInfo(&info);
		// Only use info for size, other is when loading it!
		if (info.Origin != IL_ORIGIN_UPPER_LEFT) 
			iluFlipImage();
		tex->format = GL_RGBA;
		tex->unit_size = 1;
		tex->unit_type = GL_UNSIGNED_BYTE;
		tex->size = info.SizeOfData;
		tex->bytes.resize(tex->size);

		tex->w = info.Width;
		tex->h = info.Height;
		memcpy(tex->bytes.data(), ilGetData(), tex->size);
	}

	ilDeleteImage(id_img);

	return ret;
}

// =====================================================================================================

PlainData ImportMaterial(const TempIfStream& file) {
	PlainData ret;
	Material* mat = new Material();
	SetPlainData(ret, mat, sizeof(Material));
	PlainData read_pd = file.GetData();
	ReadStream read;
	read.SetData(read_pd.data, read_pd.size);

	//uint32_t namelen;
	//read.Get(&namelen);
	//mat->name.resize(namelen);
	//read.GetArr(&mat->name.at(0), namelen);

	read.Get(&mat->diffuse);
	read.Get(&mat->specular);
	read.Get(&mat->shininess);
	read.Get(&mat->shine_strength);

	read.Get(&mat->ambient);
	read.Get(&mat->emissive);
	read.Get(&mat->transparent);
	read.Get(&mat->opacity);

	read.Get(&mat->reflective);
	read.Get(&mat->reflectivity);

	read.Get(&mat->refractiveness);

	uint32_t num_texs;
	read.Get(&num_texs);
	mat->textures.resize(num_texs);
	read.GetArr(mat->textures.data(), num_texs);

	return ret;
}

PlainData SerializeMaterial(const Material& mat) {
	PlainData ret;
	
	WriteStream out;
	
	//uint32_t namelen = mat.name.length();
	//out.Add(&namelen);
	//out.AddArr(mat.name.c_str(), namelen);

	out.Add(&mat.diffuse);
	out.Add(&mat.specular);
	out.Add(&mat.shininess);
	out.Add(&mat.shine_strength);
	
	out.Add(&mat.ambient);
	out.Add(&mat.emissive);
	out.Add(&mat.transparent);
	out.Add(&mat.opacity);

	out.Add(&mat.reflective);
	out.Add(&mat.reflectivity);

	out.Add(&mat.refractiveness);

	uint32_t num_texs = mat.textures.size();
	out.Add(&num_texs);
	out.AddArr(mat.textures.data(), num_texs);

	datapair out_data = out.GetOwnership();
	ret.data = out_data.first;
	ret.size = out_data.second;

	return ret;
}

std::vector<WatchedData> ConvertAssimpMaterial(const aiMaterial* aimat, const char* parent_path) {
	std::vector<WatchedData> ret;
	Material mat;

	int num_texs = aimat->GetTextureCount(aiTextureType_DIFFUSE);
	for (int i = 0; i < num_texs; ++i) {
		aiString path;
		aimat->GetTexture(aiTextureType_DIFFUSE, i, &path);
		// TODO: Set the path to the relative path of the scene!
		AppendVec(ret, TryLoadFromDisk(path.C_Str(), parent_path));
		ret.back().uid = PCGRand();
		ret.back().load_event_type = 0; // UNSET eventtype, so Material asks to load textures
		mat.textures.push_back(TexRelation());
		mat.textures.back().tex_uid = ret.back().uid;
		mat.textures.back().type = aiTextureType_DIFFUSE;

	}

	// Add Material at the back of WatchedData vec to make sure textures are
	// loaded before the material to GPU

	ret.push_back(WatchedData());
	WatchedData& mat_wd = ret.back();
	
	mat_wd.load_event_type = LOAD_MAT_TO_GPU;

	aimat->Get(AI_MATKEY_NAME, mat.name);
	aimat->Get(AI_MATKEY_COLOR_DIFFUSE, mat.diffuse);
	aimat->Get(AI_MATKEY_COLOR_AMBIENT, mat.ambient);
	aimat->Get(AI_MATKEY_COLOR_EMISSIVE, mat.emissive);
	aimat->Get(AI_MATKEY_COLOR_TRANSPARENT, mat.transparent);
	aimat->Get(AI_MATKEY_OPACITY, mat.opacity);
	aimat->Get(AI_MATKEY_COLOR_REFLECTIVE, mat.reflective);
	aimat->Get(AI_MATKEY_REFLECTIVITY, mat.reflectivity);
	aimat->Get(AI_MATKEY_COLOR_SPECULAR, mat.specular);
	aimat->Get(AI_MATKEY_SHININESS, mat.shininess);
	aimat->Get(AI_MATKEY_SHININESS_STRENGTH, mat.shine_strength);
	aimat->Get(AI_MATKEY_REFRACTI, mat.refractiveness);

	std::string temp_path = "Assets/Materials/" + std::string(aimat->GetName().C_Str()) + std::string(".material");
	mat_wd.str_len = FitString(mat_wd.path, "Assets/Materials/%s.material", aimat->GetName().C_Str());
	mat_wd.loaded = true;
	

	mat_wd.pd = SerializeMaterial(mat);

	return ret;
}