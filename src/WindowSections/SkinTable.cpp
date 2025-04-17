#include <imgui.h>
#include <stdio.h>
#include <SkinTable.h>
#include <imgui_stdlib.h>
#include <MDLFile_V48.h>
#include <algorithm>

namespace VrukSections
{
    namespace SkinTable
    {
        void DrawSkinsTable(MDLFile* fileData)
        {
            const MDLFileBaseHeader* HeaderData = fileData->GetBaseHeaderData();

            if (!fileData->HasTextures())
                return;

            int tableHorizontalEntries = fileData->GetSkinReferenceCount();
            if (tableHorizontalEntries <= 0)
                return;

            std::vector<Texture>* Textures = fileData->GetTextures();
            std::vector<TexturePath>* TexturePaths = fileData->GetTexturePaths();
            std::vector<std::vector<int16_t>>* SkinFamilies = fileData->GetSkinFamilies();


            // Most "big" widgets share a common width settings by default. See 'Demo->Layout->Widgets Width' for details.
            //ImGui::PushItemWidth(ImGui::GetFontSize() * -12);           // e.g. Leave a fixed amount of width for labels (by passing a negative value), the rest goes to widgets.
            //ImGui::PushItemWidth(-ImGui::GetWindowWidth() * 0.35f);   // e.g. Use 2/3 of the space for widgets and 1/3 for labels (right align)

            // FIXME: there is temporary (usually single-frame) ID Conflict during reordering as a same item may be submitting twice.
            // This code was always slightly faulty but in a way which was not easily noticeable.
            // Until we fix this, enable ImGuiItemFlags_AllowDuplicateId to disable detecting the issue.
            ImGui::PushItemFlag(ImGuiItemFlags_AllowDuplicateId, true);

            int RemoveSkinAtIndex = -1;
            int AddSkinAtIndex = -1;

            //ImGui::GetStyle().FramePadding.x = 0;

            if (ImGui::BeginTable("Skins_Table", tableHorizontalEntries + 1, ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_HighlightHoveredColumn))
            {
                ImGui::TableSetupColumn("Skin Family", ImGuiTableColumnFlags_NoSort);
                for (int i = 0; i < tableHorizontalEntries; i++)
                {
                    char label[128];
                    sprintf(label, "Ref %d", i);
                    ImGui::TableSetupColumn(label);
                }
                ImGui::TableHeadersRow();

                int dragDropOption = 0;
                if (ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift))
                    dragDropOption = 1;
                else if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl))
                    dragDropOption = 2;

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                for (int n = 0; n < SkinFamilies->size(); n++)
                {
                    if (n == 0)
                        ImGui::Spacing();
                    char label[128];
                    sprintf(label, "Skin %d", n);
                    ImGui::Selectable(label);

                    if (ImGui::BeginDragDropSource())
                    {
                        ImGui::SetDragDropPayload("SKIN_DRAG", &n, sizeof(int));
                        int option = 0;
                        switch (dragDropOption)
                        {
                        case 0:
                            ImGui::Text("Drop to swap skins");
                            ImGui::Text("Shift to place before");
                            ImGui::Text("Ctrl to place after");
                            break;

                        case 1:
                            ImGui::Text("Drop to place before");
                            break;

                        case 2:
                            ImGui::Text("Drop to place after");
                        }
                        
                        ImGui::Separator();
                        ImGui::Text("Skin %d:", n);
                        for (int skinRefSlot = 0; skinRefSlot < SkinFamilies->at(n).size(); skinRefSlot++)
                        {
                            ImGui::Text("Ref %d: %s", skinRefSlot, Textures->at(SkinFamilies->at(n).at(skinRefSlot)).FileName.c_str());
                        }
                        ImGui::EndDragDropSource();
                    }
                    if (ImGui::BeginDragDropTarget())
                    {
                        const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SKIN_DRAG");
                        if (payload)
                        {
                            IM_ASSERT(payload->DataSize == sizeof(int));
                            int payload_n = *(const int*)payload->Data;

                            if (payload_n != n)
                            {
                                int placeAtIndex = -1;

                                switch (dragDropOption)
                                {
                                case 0:
                                    std::swap(SkinFamilies->at(n), SkinFamilies->at(payload_n));
                                    fileData->isDirty = true;
                                    break;
                                case 1:
                                    placeAtIndex = n;
                                    break;
                                case 2:
                                    placeAtIndex = n + 1;
                                    break;
                                }

                                if (placeAtIndex >= 0)
                                {
                                    // payload_n being the item to move, placeAtIndex being the new location

                                    if (placeAtIndex > payload_n)
                                        std::rotate(SkinFamilies->begin() + payload_n, SkinFamilies->begin() + payload_n + 1, SkinFamilies->begin() + placeAtIndex);
                                    else if (placeAtIndex < payload_n)
                                        std::rotate(SkinFamilies->begin() + placeAtIndex, SkinFamilies->begin() + payload_n, SkinFamilies->begin() + payload_n + 1);

                                    // if placeAtIndex == payload_n, we don't need to do anything :)
                                    fileData->isDirty = true;
                                }
                            }
                        }

                        ImGui::EndDragDropTarget();
                    }

                    if(ImGui::BeginPopupContextItem())
                    {
                        char label[128];
                        sprintf(label, "Delete: Skin %d", n);
                        if(ImGui::Selectable(label))
                            RemoveSkinAtIndex = n;

                        ImGui::Spacing();

                        sprintf(label, "Add new skin after: Skin %d", n);
                        if (ImGui::Selectable(label))
                            AddSkinAtIndex = n + 1;

                        sprintf(label, "Add new skin before: Skin %d", n);
                        if(ImGui::Selectable(label))
                            AddSkinAtIndex = n;

                        ImGui::EndPopup();
                    }


                    ImGui::TableNextColumn();
                    for (int skinRefSlot = 0; skinRefSlot < tableHorizontalEntries; skinRefSlot++)
                    {
                        if (n == 0)
                            ImGui::Spacing();
                        char label[128];
                        sprintf(label, "skinfam_%d_ref_%d_combo", n, skinRefSlot);
                        ImGui::PushID(label);
                        if (ImGui::BeginCombo("", Textures->at(SkinFamilies->at(n).at(skinRefSlot)).FileName.c_str(), ImGuiComboFlags_WidthFitPreview))
                        {
                            for (int textureIndex = 0; textureIndex < Textures->size(); textureIndex++)
                            {
                                const bool is_selected = (SkinFamilies->at(n).at(skinRefSlot) == textureIndex);

                                char label[128];
                                sprintf(label, "skinfam_%d_ref_%d_combo_tex_%d", n, skinRefSlot, textureIndex);
                                ImGui::PushID(label);

                                if (ImGui::Selectable(Textures->at(textureIndex).FileName.c_str(), is_selected))
                                {
                                    SkinFamilies->at(n).at(skinRefSlot) = textureIndex;
                                    fileData->isDirty = true;
                                }

                                ImGui::PopID();
                                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                                if (is_selected)
                                    ImGui::SetItemDefaultFocus();
                            }
                            ImGui::EndCombo();
                        }
                        ImGui::PopID();
                        ImGui::TableNextColumn();
                    }
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                }

                ImGui::EndTable();
            }
            ImGui::PopItemFlag();

            if (RemoveSkinAtIndex >= 1 && RemoveSkinAtIndex <= SkinFamilies->size())
            {
                SkinFamilies->erase(SkinFamilies->begin() + RemoveSkinAtIndex);

                fileData->isDirty = true;
                // Not really needed since we are at the end of the function
                RemoveSkinAtIndex = -1;
            }

            if (AddSkinAtIndex >= 0)
            {
                SkinFamilies->resize(SkinFamilies->size() + 1);
                std::rotate(SkinFamilies->begin() + AddSkinAtIndex, SkinFamilies->end() - 1, SkinFamilies->end());
                SkinFamilies->at(AddSkinAtIndex).resize(tableHorizontalEntries);

                fileData->isDirty = true;
                // Not really needed since we are at the end of the function
                AddSkinAtIndex = -1;
            }


        }
    }
}