#include "MDLFileTab.h"
#include <imgui.h>
#include <SkinTable.h>
#include <KeyValuesText.h>
#include <MDLFileFactory.h>
#include <TexturesTable.h>

namespace VrukSections {

    bool MDLFileTab::LoadFile(const char* filePath)
    {
        MDLFile* tempFile = MDLFileFactory::LoadFromFile(filePath);
        if (!tempFile)
            return false;
        this->File = tempFile;

        this->filePath.assign(filePath);

        const MDLFileBaseHeader* tempHeader = this->File->GetBaseHeaderData();
        if (!tempHeader)
            return false;

        this->fileName.assign(tempHeader->FullName);

        return true;
    }

    bool MDLFileTab::SaveFile(const char* filePath)
    {
        int indexOfName = FindFileNameIndexInPath(filePath);
        if (strcmp(filePath + indexOfName, this->fileName.c_str()))
        {
            if (!this->File->SetFileName(filePath + indexOfName))
                return false;
        }
        bool result = this->File->SaveToFile(filePath);
        if (result)
        {
            this->isDirty = false;
            this->filePath.assign(filePath);
            this->fileName.assign(this->File->GetBaseHeaderData()->FullName);
            this->File->isDirty = false;
        }
        return result;
    }

	int MDLFileTab::DrawTab(int tabIndex)
	{
        int flags = ImGuiTabItemFlags_NoAssumedClosure;
        if (this->isDirty) flags |= ImGuiTabItemFlags_UnsavedDocument;

        ImGui::PushID(tabIndex);
        bool selected = ImGui::BeginTabItem(fileName.c_str(), &this->open, flags);
        if (selected)
        {
            if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_Reorderable))
            {
                if (ImGui::BeginTabItem("Skins"))
                {
                    if (!this->File->HasTextures())
                    {
                        ImGui::Text("File has no textures");
                    }
                    else if (this->File->GetSkinReferenceCount() <= 0)
                    {
                        ImGui::Text("File has no materials");
                    }
                    else
                    {
                        VrukSections::SkinTable::DrawSkinsTable(this->File);

                        VrukSections::TexturesTable::DrawTexturesTable(this->File);
                        ImGui::SameLine(0, 10);
                    }
                    
                    if (this->File->HasTexturePaths())
                        VrukSections::TexturesTable::DrawTexturePathsTable(this->File);
                    else
                    {
                        ImGui::Text("File has no texture paths");
                        ImGui::NewLine();
                    }
                        
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Keyvalues"))
                {
                    VrukSections::KeyValuesText::DrawKeyValuesTextBox(this->File);
                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }

            ImGui::EndTabItem();
        }
        ImGui::PopID();

        isDirty = File->isDirty;

        int returnFlags = TabResult_None;

        if(!this->open)
        {
            if(isDirty)
                this->open = true;

            returnFlags |= TabResult_Closed;
        }

        if (selected) returnFlags |= TabResult_Selected;

        return returnFlags;
	}
}