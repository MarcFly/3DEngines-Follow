#include "..\..\Importers.h"
#include <DevIL/include/IL/il.h>
#include <DevIL/include/IL/ilu.h>
#include <src/modules/Render/RendererTypes.h>

uint32_t ExtensionToDevILType(const char* ext) {
	uint32_t ret = 0;
	if(strcmp(ext, ".png") == 0 || strcmp(ext, ".PNG")==0)
		ret = IL_PNG;
	else if (strcmp(ext, ".dds") == 0 || strcmp(ext, ".DDS") == 0)
		ret = IL_DDS;
	else if (strcmp(ext, ".tiff") == 0 || strcmp(ext, ".TIFF") == 0 || strcmp(ext, ".TIF") == 0 || strcmp(ext, ".tif") == 0)
		ret = IL_TIF;

	return ret;
}

PlainData ImportDevILTexture(const PlainData& pd, uint32_t tex_type) {
	PlainData ret;

	Texture* tex = new Texture();
	SetPlainData(ret, tex, sizeof(Texture));

	ILuint id_img = ilGenImage();
	ilBindImage(id_img);
	bool success = ilLoadL(tex_type, pd.data, pd.size);
	if (success && ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE)) {
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
		memcpy(tex->bytes, ilGetData(), tex->size);
	}

	ilDeleteImage(id_img);

	return ret;
}