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

Texture ConvertDevILTexture(const TempIfStream& file) {
	const char* ext = strrchr(file.path.c_str(), '.');
	ILenum tex_type = ExtensionToDevILType_Convert(ext);
	if (tex_type == 0) return Texture();
	const PlainData& pd = file.GetData();
	Texture tex;

	ILuint id_img = ilGenImage();
	ilBindImage(id_img);
	bool success = ilLoadL(tex_type, pd.data, pd.size);
	//Texture* tex = nullptr;

	if (success && ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE)) {
		
		//tex = new Texture();
		//SetPlainData(ret, tex->bytes, sizeof(Texture));
		
		ILinfo info;
		iluGetImageInfo(&info);
		if (info.Origin != IL_ORIGIN_LOWER_LEFT)
			iluFlipImage();

		tex.format = GL_RGBA;
		tex.unit_size = 1;
		tex.unit_type = GL_UNSIGNED_BYTE;
		tex.size = info.SizeOfData;
		tex.bytes.resize(tex.size);

		tex.w = info.Width;
		tex.h = info.Height;
		memcpy(tex.bytes.data(), ilGetData(), tex.size);
	}

	ilDeleteImage(id_img);

	return tex;
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

void ConvertAssimpMaterial(const aiMaterial* aimat, const char* parent_path, Material& mat) {

	int num_texs = aimat->GetTextureCount(aiTextureType_DIFFUSE);
	for (int i = 0; i < num_texs; ++i) {
		aiString path;
		aimat->GetTexture(aiTextureType_DIFFUSE, i, &path);
		TempIfStream texstream(path.C_Str());
		if (texstream.GetData().size == 0) TryLoad_WithParentPath(path.C_Str(), parent_path, texstream);
		// TODO: Set the path to the relative path of the scene!
		Texture tex = ConvertDevILTexture(texstream);
		mat.textures_data.push_back(tex);
	}

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
}