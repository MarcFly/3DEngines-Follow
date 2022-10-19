#include "../Importers.h"
#include <DevIL/include/IL/il.h>
#include <DevIL/include/IL/ilu.h>
#include <src/modules/Render/RendererTypes.h>
#include "../ModuleFS.h"

uint32_t ExtensionToDevILType(const char* ext) {
	uint32_t ret = 0;
	if (strcmp(ext, ".png") == 0 || strcmp(ext, ".PNG") == 0)
		ret = IL_PNG;
	else if (strcmp(ext, ".dds") == 0 || strcmp(ext, ".DDS") == 0)
		ret = IL_DDS;
	else if (strcmp(ext, ".tiff") == 0 || strcmp(ext, ".TIFF") == 0 || strcmp(ext, ".TIF") == 0 || strcmp(ext, ".tif") == 0)
		ret = IL_TIF;
	else if (strcmp(ext, ".tga") == 0 || strcmp(ext, ".TGA") == 0)
		ret = IL_TGA;

	return ret;
}

WatchedData TryImportTexture(const TempIfStream& file) {
	WatchedData ret;
	const char* ext = strrchr(file.path.c_str(), '.');
	
	ILenum tex_type = ExtensionToDevILType(ext);
	if ( tex_type != 0) {
		ret.pd = ImportDevILTexture(file.GetData(), tex_type);
		ret.uid = PCGRand();
		ret.str_len = file.path.length();
		ret.path = new char[ret.str_len];
		memcpy(ret.path, file.path.c_str(), ret.str_len);
	}
}

PlainData ImportDevILTexture(const PlainData& pd, uint32_t tex_type) {
	PlainData ret;
	
	ILuint id_img = ilGenImage();
	ilBindImage(id_img);
	bool success = ilLoadL(tex_type, pd.data, pd.size);
	
	if (success && ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE)) {
		
		Texture* tex = new Texture();
		SetPlainData(ret, tex, sizeof(Texture));
		
		ILinfo info;
		iluGetImageInfo(&info);
		//if (info.Origin != IL_ORIGIN_UPPER_LEFT) 
			iluFlipImage();
		tex->format = GL_RGBA;
		tex->unit_size = 1;
		tex->unit_type = GL_UNSIGNED_BYTE;
		tex->size = info.SizeOfData;
		tex->bytes = new char[tex->size];
		
		tex->w = info.Width;
		tex->h = info.Height;		

		tex->disksize = ilSaveL(IL_DDS, tex->bytes, tex->size);
	}

	ilDeleteImage(id_img);

	return ret;
}

std::vector<WatchedData> ImportAssimpMaterial(const aiMaterial* aimat, const char* parent_path) {
	std::vector<WatchedData> ret;
	Material* mat = new Material();

	int num_texs = aimat->GetTextureCount(aiTextureType_DIFFUSE);
	for (int i = 0; i < num_texs; ++i) {
		aiString path;
		aimat->GetTexture(aiTextureType_DIFFUSE, i, &path);
		// TODO: Set the path to the relative path of the scene!
		AppendVec(ret, TryLoadFromDisk(path.C_Str(), parent_path));
		ret.back().uid = PCGRand();
		ret.back().load_event_type = 0; // UNSET eventtype, so Material asks to load textures
		mat->textures.push_back(TexRelation());
		mat->textures.back().tex_uid = ret.back().uid;
		mat->textures.back().type = aiTextureType_DIFFUSE;
	}

	// Add Material at the back of WatchedData vec to make sure textures are
	// loaded before the material to GPU

	ret.push_back(WatchedData());
	WatchedData& mat_wd = ret.back();
	
	SetPlainData(mat_wd.pd, mat, sizeof(Material));
	mat_wd.event_type = LOAD_MAT_TO_GPU;

	aimat->Get(AI_MATKEY_NAME, mat->name);
	aimat->Get(AI_MATKEY_COLOR_DIFFUSE, mat->diffuse);
	aimat->Get(AI_MATKEY_COLOR_AMBIENT, mat->ambient);
	aimat->Get(AI_MATKEY_COLOR_EMISSIVE, mat->emissive);
	aimat->Get(AI_MATKEY_COLOR_TRANSPARENT, mat->transparent);
	aimat->Get(AI_MATKEY_OPACITY, mat->opacity);
	aimat->Get(AI_MATKEY_COLOR_REFLECTIVE, mat->reflective);
	aimat->Get(AI_MATKEY_REFLECTIVITY, mat->reflectivity);
	aimat->Get(AI_MATKEY_COLOR_SPECULAR, mat->specular);
	aimat->Get(AI_MATKEY_SHININESS, mat->shininess);
	aimat->Get(AI_MATKEY_SHININESS_STRENGTH, mat->shine_strength);
	aimat->Get(AI_MATKEY_REFRACTI, mat->refractiveness);

	return ret;
}