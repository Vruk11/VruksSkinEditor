#pragma once
#include <cstdint>
#include <string>
#include <nfd.h>
#include <SDL.h>
#include <vector>

struct Vec3
{
	int32_t x, y, z;
};

struct DataLocation
{
	int32_t Count;
	int32_t Offset;
};

struct MDLFileBaseHeader
{
	int8_t ID[4];
	int32_t Version;
	int32_t Checksum;
	char FullName[64];
	int32_t FileSize;
};

struct TextureData
{
	int32_t NameOffset;
	int32_t Flags;
	int32_t Used;
	int32_t Unused1;
	int32_t MaterialP;
	int32_t ClientMaterialP;
	int32_t unused[10];
	//std::string<char> FilePathName @ addressof(this) + NameOffset;
};

struct Texture
{
	TextureData Data;
	std::string FileName;
};

struct TexturePath
{
	int32_t Address;
	std::string FolderPath;
};

// To maintain compatability with the existing skin editor, find the swoobles delimeter
// if the file has already been edited
constexpr const char* SwooblesURL = "http://swoobles.com";

size_t FindSwooblesDelimeter(const char* buffer, size_t bufferLength);

class MDLFile
{
public:
	MDLFile();
	~MDLFile()
	{
		free(FullFileData);
	}

	virtual bool ReadFromFile(const char* fileName);

	virtual bool ReadFromBuffer(const char* buffer, int length);

	virtual bool SaveToFile(std::string filepath) { return false; }

	const MDLFileBaseHeader* GetBaseHeaderData() { return BaseHeaderData; }

	bool SetFileName(const char* newName);

	virtual bool HasTextures() { return false; }
	virtual bool HasTexturePaths() { return false; }
	virtual std::vector<Texture>* GetTextures() { return nullptr; }
	virtual std::vector<TexturePath>* GetTexturePaths() { return nullptr; }
	virtual std::vector<std::vector<int16_t>>* GetSkinFamilies() { return nullptr; }
	virtual int GetSkinReferenceCount() { return -1; }
	virtual std::string* GetKeyValues() { return nullptr; }

	bool isDirty;
protected:
	char* FullFileData;
	size_t FullFileDataLength;

	MDLFileBaseHeader* BaseHeaderData;

	friend class MDLFileFactory;

};