#include <MDLFileFactory.h>
#include <MDLFile_V48.h>

MDLFile* MDLFileFactory::LoadFromFile(const char* filePath)
{
    MDLFile temp_data;
    bool successfulRead = temp_data.ReadFromFile(filePath);
    if (!successfulRead)
        return nullptr;

    MDLFile* resultMDLFilePTR = GetCorrectVersion(temp_data.BaseHeaderData->Version);
    if (!resultMDLFilePTR)
        return nullptr;

    if (resultMDLFilePTR->ReadFromBuffer(temp_data.FullFileData, temp_data.FullFileDataLength))
        return resultMDLFilePTR;

    return nullptr;
}

MDLFile* MDLFileFactory::LoadFromBuffer(const char* buffer, size_t length)
{
    MDLFile temp_data;
    bool successfulRead = temp_data.ReadFromBuffer(buffer, length);
    if (!successfulRead)
        return nullptr;

    MDLFile* resultMDLFilePTR = GetCorrectVersion(temp_data.BaseHeaderData->Version);
    if (!resultMDLFilePTR)
        return nullptr;

    if (resultMDLFilePTR->ReadFromBuffer(temp_data.FullFileData, temp_data.FullFileDataLength))
        return resultMDLFilePTR;

    return nullptr;
}

MDLFile* MDLFileFactory::GetCorrectVersion(int32_t version)
{
    MDLFile* result;
    switch (version)
    {
    case 48:
        result = new MDLFile_V48;
        break;
    default:
        return nullptr;
    }

    return result;
}