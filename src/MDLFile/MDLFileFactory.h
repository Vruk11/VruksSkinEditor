#pragma once
#include "MDLFile.h"

class MDLFileFactory
{
public:
	static MDLFile* LoadFromFile(const char* filePath);
	static MDLFile* LoadFromBuffer(const char* buffer, size_t length);
private:
	static MDLFile* GetCorrectVersion(int32_t version);
};