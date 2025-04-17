#pragma once
#include "MDLFile.h"
#include "vector"

struct MDLFileHeader_V48
{
    Vec3 EyePosition;

    Vec3 IlluminationPosition;

    Vec3 HullMinPosition;
    Vec3 HullMaxPosition;

    Vec3 ViewBoundingBoxMinPosition;
    Vec3 ViewBoundingBoxMaxPosition;

    int32_t Flags;

    DataLocation Bones;
    DataLocation BoneControllers;

    DataLocation HitboxSets;

    DataLocation LocalAnimations;

    DataLocation LocalSequences;

    int32_t ActivityListVersion;
    int32_t EventsIndexed;

    DataLocation TexturesInfo;
    DataLocation TexturePathsInfo;

    int32_t SkinReferenceCount;
    DataLocation SkinFamiliesInfo;

    DataLocation BodyParts;

    DataLocation LocalAttachments;

    DataLocation LocalNodes;
    int32_t LocalNodeNameOffset;

    DataLocation FlexDescriptions;
    DataLocation FlexControllers;
    DataLocation FlexRules;

    DataLocation IKChains;

    DataLocation Mouths;

    DataLocation LocalPoseParameters;

    int32_t SurfacePropOffset;

    int32_t KeyValuesOffset;
    int32_t KeyValuesSize;

    DataLocation LocalIKAutoPlayLocks;

    float Mass;
    int32_t Contents;

    DataLocation IncludeModels;

    int32_t VirtualModelP;

    int32_t AnimBlockNameOffset;
    DataLocation AnimBlocks;
    int32_t AnimBlockModelP;

    int32_t BoneTableNameOffset;

    int32_t VertexBaseP;
    int32_t IndexBaseP;

    int8_t DirectionalLightDot;

    int8_t RootLOD;

    int8_t AllowedRootLODCount_VERSION48;

    int8_t Unused1;
    int32_t ZeroFrameCacheIndex_VERSION44to47;

    DataLocation FlexControllerUIs;

    int32_t Unused2;
    int32_t Unused3;

    int32_t StudioHeader2Offset;

    int32_t Unused4;
};

struct MDLFileHeader02_V48
{
    DataLocation SourceBoneTransforms;

    int32_t IllumPositionAttachmentNumber;

    float maxEyeDeflection;

    int32_t LinearBoneOffset;

    //Not used?
    int32_t NameCopyOffset;

    int32_t reserved[57];
};

class MDLFile_V48 : public MDLFile
{
public:
    MDLFile_V48() {};
	~MDLFile_V48() {};

    virtual bool ReadFromBuffer(const char* buffer, int length) override;
    virtual bool SaveToFile(std::string filePath) override;

    virtual bool HasTextures() override { return !Textures.empty(); };
    virtual bool HasTexturePaths() override { return !TexturePaths.empty(); }
    virtual std::vector<Texture>* GetTextures() override { return &Textures; }
    virtual std::vector<TexturePath>* GetTexturePaths() override { return &TexturePaths; }
    virtual std::vector<std::vector<int16_t>>* GetSkinFamilies() override { return &SkinFamilies; }
    virtual int GetSkinReferenceCount() { return V48HeaderData->SkinReferenceCount; }
    virtual std::string* GetKeyValues() { return &KeyValues; }

protected:
    MDLFileHeader_V48* V48HeaderData;
    MDLFileHeader02_V48* V48HeaderData2;

    std::vector<Texture> Textures;
    std::vector<TexturePath> TexturePaths;
    std::vector<std::vector<int16_t>> SkinFamilies;

    std::string SurfaceProp;

    std::string KeyValues;
};