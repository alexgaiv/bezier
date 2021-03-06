#ifndef _TEXTURE2D_H_
#define _TEXTURE2D_H_

#include "common.h"

#pragma pack(push, 1)
struct TGAHEADER
{
	BYTE   idLength;
	BYTE   colorMapType;
	BYTE   imageType;
	USHORT colorMapOffset;
	USHORT colorMapLength;
	BYTE   colorMapEntrySize;
	USHORT xOrigin;
	USHORT yOrigin;
	USHORT width;
	USHORT height;
	BYTE   depth;
	BYTE   descriptor;
};
#pragma pack(pop)

class Texture2D
{
public:
	UINT id;
	TGAHEADER tgaHeader;
	BYTE *imageData;

	Texture2D();
	Texture2D(LPCWSTR filename, UINT id = 0);
	~Texture2D();

	static void SetEnvMode();
	static void SetMinFilter();
	static void SetMagFilter();

	bool IsLoaded() { return _loaded; }
	bool LoadFromTGA(LPCWSTR filename);

	void SetAsTexImage() {
		if (!_loaded) return;
		this->Bind();

		glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, tgaHeader.width,
			tgaHeader.height, 0, _format, GL_UNSIGNED_BYTE, imageData);
	}

	void BuildMipmaps() {
		if (!_loaded) return;
		this->Bind();

		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tgaHeader.width, tgaHeader.height,
			_format, GL_UNSIGNED_BYTE, imageData);
	}

	void Bind() {
		glBindTexture(GL_TEXTURE_2D, id);
	}
private:
	bool _loaded;
	int _internalFormat, _format;

	bool _read(HANDLE hFile, LPVOID lpBuffer, DWORD nNumBytes);
};

#endif // _TEXTURE2D_H_