#include "MDLFile.h"
#include "fstream"
#include "vector"

size_t FindSwooblesDelimeter(const char* buffer, size_t bufferLength)
{
	// reverse search since it's at the end of the file
	for (size_t i = bufferLength; i > 0; i--)
	{
		// -1 to account for null character
		if (buffer[i] == SwooblesURL[strlen(SwooblesURL) - 1])
		{
			if (!strcmp(buffer + i - strlen(SwooblesURL) + 1, SwooblesURL))
				return (size_t)(i - strlen(SwooblesURL) + 1);
		};
	}
	return 0;
};

MDLFile::MDLFile()
{
	this->isDirty = false;

	this->FullFileData = nullptr;
	this->FullFileDataLength = 0;

	this->BaseHeaderData = nullptr;
}

bool MDLFile::ReadFromFile(const char* fileName)
{
	std::ifstream fileStream = std::ifstream(fileName, std::ios::binary);

	if (fileStream.good())
	{
		std::streampos fsize = fileStream.tellg();
		fileStream.seekg(0, std::ifstream::end);
		fsize = fileStream.tellg() - fsize;

		std::vector<char> DataBuffer;

		//DataBuffer.reserve(fsize);
		DataBuffer.resize(fsize, 0x00);

		fileStream.seekg(0, std::ifstream::beg);
		fileStream.read(&DataBuffer.front(), fsize);

		fileStream.close();

		return ReadFromBuffer((const char*)&DataBuffer.front(), fsize);
	}
	else
		return false;
};

bool MDLFile::ReadFromBuffer(const char* buffer, int length)
{
	if (FullFileData)
	{
		free(FullFileData);
	}
	if(FullFileDataLength)
	{
		FullFileDataLength = 0;
	}

	if (length < sizeof(MDLFileBaseHeader))
		return false;

	void* dataPtr = malloc(length);
	if (!dataPtr)
		return false;
	FullFileData = reinterpret_cast<char*>(dataPtr);
	FullFileDataLength = length;

	memcpy(dataPtr, buffer, length);

	this->BaseHeaderData = reinterpret_cast<MDLFileBaseHeader*>(FullFileData);

	return true;
}

bool MDLFile::SetFileName(const char* newName)
{
	if (strlen(newName) > 64) return false;

	memcpy(&this->BaseHeaderData->FullName, newName, strlen(newName) + 1);

	return true;
}