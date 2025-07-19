#include "MDLFile.h"
#include <MDLFile_V48.h>

#include "string"
#include "iostream"
#include "fstream"

bool MDLFile_V48::ReadFromBuffer(const char* buffer, int length)
{
	if (!this->MDLFile::ReadFromBuffer(buffer, length))
		return false;

	if (BaseHeaderData->Version != 48)
	{
		std::cout << "File version: " + std::to_string(BaseHeaderData->Version) + " is not compatible for version 48" << std::endl;;
		return false;
	}


	// These won't save us..
	if (length < sizeof(MDLFileBaseHeader) + sizeof(MDLFileHeader_V48))
		return false;

	V48HeaderData = reinterpret_cast<MDLFileHeader_V48*>(FullFileData + sizeof(MDLFileBaseHeader));

	if (length < V48HeaderData->StudioHeader2Offset + sizeof(MDLFileHeader02_V48))
		return false;

	V48HeaderData2 = reinterpret_cast<MDLFileHeader02_V48*>(FullFileData + V48HeaderData->StudioHeader2Offset);
	
	//
	// SECTION: TEXTURE NAMES
	//

	Textures.resize(V48HeaderData->TexturesInfo.Count);
	for (int i = 0; i < V48HeaderData->TexturesInfo.Count; i++)
	{
		const char* BaseTextureDataOffset = reinterpret_cast<const char*>(FullFileData + V48HeaderData->TexturesInfo.Offset + (i * sizeof(TextureData)));

		memcpy(&(Textures[i].Data), BaseTextureDataOffset, sizeof(TextureData));
		Textures[i].FileName.assign(BaseTextureDataOffset + Textures[i].Data.NameOffset);
	}

	//
	// SECTION: TEXTURE PATHS
	//

	TexturePaths.resize(V48HeaderData->TexturePathsInfo.Count);
	for (int i = 0; i < V48HeaderData->TexturePathsInfo.Count; i++)
	{
		memcpy(&(TexturePaths[i].Address), reinterpret_cast<const char*>(FullFileData + V48HeaderData->TexturePathsInfo.Offset + sizeof(int32_t) * i), sizeof(int32_t));
		TexturePaths[i].FolderPath.assign(reinterpret_cast<const char*>(FullFileData + TexturePaths[i].Address));
	}
	
	//
	// SECTION: SKIN FAMILIES
	//

	SkinFamilies.resize(V48HeaderData->SkinFamiliesInfo.Count);

	for (int i = 0; i < V48HeaderData->SkinFamiliesInfo.Count; i++)
	{
		std::vector<int16_t>* skinRefs = &SkinFamilies[i];
		skinRefs->resize(V48HeaderData->SkinReferenceCount);

		memcpy(&skinRefs->front(), FullFileData + V48HeaderData->SkinFamiliesInfo.Offset + (sizeof(int16_t) * V48HeaderData->SkinReferenceCount * i), (sizeof(int16_t) * V48HeaderData->SkinReferenceCount));
	}

	//
	// SECTION: SURFACE PROP
	//
	SurfaceProp.assign(FullFileData + V48HeaderData->SurfacePropOffset);

	//
	// SECTION: KEYVALUES
	//
	if(V48HeaderData->KeyValuesSize > 0)
	{
		KeyValues.assign(FullFileData + V48HeaderData->KeyValuesOffset);
	}

	return true;
}

bool MDLFile_V48::SaveToFile(std::string filePath)
{
	size_t Swoobles = FindSwooblesDelimeter(FullFileData, FullFileDataLength);
	if (!Swoobles)
		Swoobles = FullFileDataLength;

	// Figure out all the new data offsets and such

	size_t lengthOfTextureData = Textures.size() * sizeof(TextureData);
	size_t lengthOfTexturePathAddresses = TexturePaths.size() * sizeof(int32_t);
	size_t lengthOfSkinFamilyRefs = SkinFamilies.size() * SkinFamilies[0].size() * sizeof(int16_t);

	V48HeaderData->TexturesInfo.Count = Textures.size();
	V48HeaderData->TexturesInfo.Offset = Swoobles + strlen(SwooblesURL) + 1;
	size_t lengthOfTextures = 0;
	for (int i = 0; i < Textures.size(); i++)
	{
		Textures[i].Data.NameOffset = lengthOfTextureData + lengthOfTexturePathAddresses + lengthOfSkinFamilyRefs + lengthOfTextures
			- (sizeof(TextureData) * i);

		//Textures[i].Data.NameOffset = (sizeof(TextureData) * (Textures.size() - i)) + lengthOfTexturePathAddresses + lengthOfSkinFamilyRefs + lengthOfTextures;
		
		// Extra char for null termination
		lengthOfTextures += Textures[i].FileName.length() + 1;
	}

	V48HeaderData->TexturePathsInfo.Count = TexturePaths.size();
	V48HeaderData->TexturePathsInfo.Offset = Swoobles + strlen(SwooblesURL) + 1 + lengthOfTextureData;
	size_t lengthOfTexturePaths = 0;
	for (int i = 0; i < TexturePaths.size(); i++)
	{
		TexturePaths[i].Address = Swoobles + strlen(SwooblesURL) + 1 + lengthOfTextureData + lengthOfTexturePathAddresses + lengthOfSkinFamilyRefs + lengthOfTextures + lengthOfTexturePaths;
		
		// Extra char for null termination
		lengthOfTexturePaths += TexturePaths[i].FolderPath.length() + 1;
	}

	V48HeaderData->SkinFamiliesInfo.Count = SkinFamilies.size();
	V48HeaderData->SkinFamiliesInfo.Offset = V48HeaderData->TexturePathsInfo.Offset + lengthOfTexturePathAddresses;


	// NameCopyOffset is based on the start address of MDLFileHeader02
	V48HeaderData2->NameCopyOffset =	Swoobles + strlen(SwooblesURL) + 1 + lengthOfTextureData + lengthOfTexturePathAddresses + lengthOfSkinFamilyRefs + 
										lengthOfTextures + lengthOfTexturePaths - sizeof(MDLFileBaseHeader) - sizeof(MDLFileHeader_V48);

	std::string nulledName = std::string(BaseHeaderData->FullName);
	size_t lengthOfNameWithNull = nulledName.length() + 1;


	V48HeaderData->SurfacePropOffset = V48HeaderData2->NameCopyOffset + lengthOfNameWithNull + sizeof(MDLFileBaseHeader) + sizeof(MDLFileHeader_V48);
	size_t lengthOfSurfacePropWithNull = SurfaceProp.length() + 1;

	V48HeaderData->KeyValuesOffset = V48HeaderData->SurfacePropOffset + lengthOfSurfacePropWithNull;
	size_t lengthOfKeyvaluesWithNull = V48HeaderData->KeyValuesSize = KeyValues.length();
	
	BaseHeaderData->FileSize = V48HeaderData->KeyValuesOffset + lengthOfKeyvaluesWithNull + 1;

	// Actually write the data

	std::ofstream outFileStream;
	outFileStream.open(filePath, std::ios::out | std::ios::trunc | std::ios::binary);
	if (!outFileStream)
		return false;

	outFileStream.write(reinterpret_cast<const char*>(BaseHeaderData), sizeof(MDLFileBaseHeader));
	outFileStream.write(reinterpret_cast<const char*>(V48HeaderData), sizeof(MDLFileHeader_V48));
	outFileStream.write(reinterpret_cast<const char*>(V48HeaderData2), sizeof(MDLFileHeader02_V48));
	
	outFileStream.write(FullFileData + sizeof(MDLFileBaseHeader) + sizeof(MDLFileHeader_V48) + sizeof(MDLFileHeader02_V48), Swoobles - sizeof(MDLFileBaseHeader) - sizeof(MDLFileHeader_V48) - sizeof(MDLFileHeader02_V48));

	outFileStream.write(SwooblesURL, strlen(SwooblesURL) + 1);

	
	for (int i = 0; i < Textures.size(); i++)
	{
		outFileStream.write(reinterpret_cast<const char*>(&Textures[i].Data), sizeof(TextureData));
	}
	for (int i = 0; i < TexturePaths.size(); i++)
	{
		outFileStream.write(reinterpret_cast<const char*>(&TexturePaths[i].Address), sizeof(int32_t));
	}
	for (int y = 0; y < SkinFamilies.size(); y++)
	{
		for (int x = 0; x < SkinFamilies[0].size(); x++)
		{
			outFileStream.write(reinterpret_cast<const char*>(&(SkinFamilies[y][x])), sizeof(int16_t));
		}
	}
	for (int i = 0; i < Textures.size(); i++)
	{
		outFileStream.write(Textures[i].FileName.c_str(), Textures[i].FileName.length() + 1);
	}
	for (int i = 0; i < TexturePaths.size(); i++)
	{
		outFileStream.write(TexturePaths[i].FolderPath.c_str(), TexturePaths[i].FolderPath.length() + 1);
	}
	outFileStream.write(nulledName.c_str(), lengthOfNameWithNull);
	outFileStream.write(SurfaceProp.c_str(), lengthOfSurfacePropWithNull);
	outFileStream.write(KeyValues.c_str(), lengthOfKeyvaluesWithNull);
	
	outFileStream.write("\0", 1);

	outFileStream.flush();
	outFileStream.close();

	return true;
}