#include <TexturesTable.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <cmath>
#include <algorithm>

namespace VrukSections
{
	namespace TexturesTable
	{
		bool DrawTexturesTable(MDLFile* fileData)
		{
			std::vector<Texture>* Textures = fileData->GetTextures();
			
			if (!Textures || Textures->empty())
				return false;
				

			ImGuiViewport* viewport = ImGui::GetMainViewport();

			int flags = ImGuiTableFlags_RowBg;

			ImGuiStyle style = ImGui::GetStyle();
			float fontSize = ImGui::GetFontSize();


			// This isn't perfect, but it's close enough?
			int skinTableHeight = ImGui::GetCursorPos().y + fontSize + (style.FramePadding.y * 2.0f) + style.CellPadding.y;

			if (viewport->WorkSize.y - skinTableHeight <= 0)
				return false;

			int RemoveAtIndex = -1;
			int AddAtIndex = -1;

			int singleLineHeight = fontSize + style.CellPadding.y + style.FramePadding.y * 2.0f;

			int NumberOfColumns = std::ceil((singleLineHeight * Textures->size()) / (viewport->WorkSize.y - skinTableHeight));
			int NumberOfItemsPerColumn = Textures->size() / NumberOfColumns;


			if (ImGui::BeginTable("Textures_Table", NumberOfColumns, flags, ImVec2(viewport->WorkSize.x / 2, 0)))
			{
				ImGui::TableSetupColumn("Textures", ImGuiTableColumnFlags_NoSort);
				for (int i = 1; i < NumberOfColumns; i++)
				{
					char label[128];
					sprintf(label, "Textures_Extra_Column_%d", i);

					ImGui::PushID(label);
					ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoSort);
					ImGui::PopID();
						
				}
				ImGui::TableHeadersRow();

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);

				for (int i = 0; i < Textures->size(); i++)
				{
					char label[128];
					sprintf(label, "Texture_%d", i);

					std::string* fileName = &(Textures->at(i).FileName);

					ImGui::PushID(label);
					ImGui::PushItemWidth(viewport->WorkSize.x / 2);
					ImGui::InputText("", &(Textures->at(i).FileName), ImGuiInputTextFlags_ElideLeft);
					ImGui::PopID();

					if (ImGui::BeginPopupContextItem())
					{
						char label[128];
						sprintf(label, "Texture Path: \"%s\"", fileName->c_str());
						ImGui::Text(label);
						ImGui::Spacing();
						ImGui::Separator();

						if (ImGui::Selectable("Delete texture path"))
						{
							// Validate that no skins use this texture
							std::vector<std::vector<int16_t>>* skinFamilies = fileData->GetSkinFamilies();
							int refSlotCount = skinFamilies->at(0).size();

							bool textureInUse = false;

							for (int iSkin = 0; iSkin < skinFamilies->size(); iSkin++)
							{
								for (int iRefSlot = 0; iRefSlot < refSlotCount; iRefSlot++)
								{
									int16_t value = skinFamilies->at(iSkin).at(iRefSlot);
									if (value == i)
									{
										textureInUse = true;
										break;
									}
								}
								if (textureInUse)
									break;
							}
							if (!textureInUse)
							{
								RemoveAtIndex = i;
							}
							else
							{
								// Notify that a skin uses this texture still
								StartFailedToDeleteTexturePopup(fileName->c_str());
							}
						}

						ImGui::Spacing();

						if (ImGui::Selectable("Add new texture below"))
							AddAtIndex = i + 1;

						if (ImGui::Selectable("Add new texture above"))
							AddAtIndex = i;

						ImGui::EndPopup();
					}

					ImGui::TableNextColumn();
				}

				if (Textures->size() > 1 && RemoveAtIndex <= Textures->size())
				{
					Textures->erase(Textures->begin() + RemoveAtIndex);

					std::vector<std::vector<int16_t>>* skinFamilies = fileData->GetSkinFamilies();
					int refSlotCount = skinFamilies->at(0).size();

					for (int iSkin = 0; iSkin < skinFamilies->size(); iSkin++)
					{
						for (int iRefSlot = 0; iRefSlot < refSlotCount; iRefSlot++)
						{
							int16_t* value = &skinFamilies->at(iSkin).at(iRefSlot);
							if (*value >= RemoveAtIndex)
							{
								(*value)--;
							}
						}
					}

					fileData->isDirty = true;
					// Not really needed since we are at the end of the function
					RemoveAtIndex = -1;
				}

				if (AddAtIndex >= 0)
				{
					Textures->resize(Textures->size() + 1);
					std::rotate(Textures->begin() + AddAtIndex, Textures->end() - 1, Textures->end());
					Textures->at(AddAtIndex).FileName.assign("\\");

					std::vector<std::vector<int16_t>>* skinFamilies = fileData->GetSkinFamilies();
					int refSlotCount = skinFamilies->at(0).size();

					for (int iSkin = 0; iSkin < skinFamilies->size(); iSkin++)
					{
						for (int iRefSlot = 0; iRefSlot < refSlotCount; iRefSlot++)
						{
							int16_t* value = &skinFamilies->at(iSkin).at(iRefSlot);
							if (*value >= AddAtIndex)
							{
								(*value)++;
							}
						}
					}

					fileData->isDirty = true;
					// Not really needed since we are at the end of the function
					AddAtIndex = -1;
				}

				ImGui::EndTable();

				DrawFailedToDeleteTexturePopup();

			}
			return true;
		}

		bool DrawTexturePathsTable(MDLFile* fileData)
		{
			std::vector<TexturePath>* TexturePaths = fileData->GetTexturePaths();

			if (!TexturePaths || TexturePaths->empty())
				return false;

			int RemoveAtIndex = -1;
			int AddAtIndex = -1;

			ImGuiViewport* viewport = ImGui::GetMainViewport();

			int flags = ImGuiTableFlags_RowBg;

			ImGuiStyle style = ImGui::GetStyle();
			float fontSize = ImGui::GetFontSize();

			// This isn't perfect, but it's close enough?
			int skinTableHeight = ImGui::GetCursorPos().y + fontSize + (style.FramePadding.y * 2.0f) + style.CellPadding.y;

			if (viewport->WorkSize.y - skinTableHeight <= 0)
				return false;

			int singleLineHeight = fontSize + style.CellPadding.y + style.FramePadding.y * 2.0f;

			int NumberOfColumns = ceil((singleLineHeight * TexturePaths->size()) / (viewport->WorkSize.y - skinTableHeight));
			int NumberOfItemsPerColumn = TexturePaths->size() / NumberOfColumns;


			if (ImGui::BeginTable("TexturePaths_Table", NumberOfColumns, flags, ImVec2(viewport->WorkSize.x / 2, 0)))
			{
				ImGui::TableSetupColumn("Texture Paths", ImGuiTableColumnFlags_NoSort);
				for (int i = 1; i < NumberOfColumns; i++)
				{
					char label[128];
					sprintf(label, "TexturePaths_Extra_Column_%d", i);

					ImGui::PushID(label);
					ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoSort);
					ImGui::PopID();

				}
				ImGui::TableHeadersRow();

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);

				for (int i = 0; i < TexturePaths->size(); i++)
				{
					char label[128];
					sprintf(label, "Texture_%d", i);

					ImGui::PushID(label);
					ImGui::PushItemWidth((viewport->WorkSize.x / 2 / NumberOfColumns) - 50);

					std::string* folderPath = &(TexturePaths->at(i).FolderPath);

					ImGui::InputText("", folderPath, ImGuiInputTextFlags_ElideLeft);
					ImGui::PopID();

					if (ImGui::BeginPopupContextItem())
					{
						char label[128];
						sprintf(label, "Texture Path: \"%s\"", folderPath->c_str());
						ImGui::Text(label);
						ImGui::Spacing();
						ImGui::Separator();

						if (ImGui::Selectable("Delete texture path"))
							RemoveAtIndex = i;

						ImGui::Spacing();

						if (ImGui::Selectable("Add new texture path below"))
							AddAtIndex = i + 1;

						if (ImGui::Selectable("Add new texture path above"))
							AddAtIndex = i;

						ImGui::EndPopup();
					}

					ImGui::TableNextColumn();
				}
				ImGui::EndTable();

				// never delete last element
				if (TexturePaths->size() > 1 && RemoveAtIndex <= TexturePaths->size())
				{
					TexturePaths->erase(TexturePaths->begin() + RemoveAtIndex);

					fileData->isDirty = true;
					// Not really needed since we are at the end of the function
					RemoveAtIndex = -1;
				}

				if (AddAtIndex >= 0)
				{
					TexturePaths->resize(TexturePaths->size() + 1);
					std::rotate(TexturePaths->begin() + AddAtIndex, TexturePaths->end() - 1, TexturePaths->end());
					TexturePaths->at(AddAtIndex).FolderPath.assign("\\");

					fileData->isDirty = true;
					// Not really needed since we are at the end of the function
					AddAtIndex = -1;
				}
			}
			return true;
		}
	
		// Functions related to failing to delete a texture from it still being in use

		static char* FailedToDeleteTextureFileName = nullptr;
		// time in seconds
		time_t lastFailedTime = std::numeric_limits<time_t>::lowest();
		constexpr time_t errorPopupDuration = 5;

		void StartFailedToDeleteTexturePopup(const char* fileName)
		{
			if (FailedToDeleteTextureFileName)
			{
				free(FailedToDeleteTextureFileName);
				FailedToDeleteTextureFileName = nullptr;
			}


			FailedToDeleteTextureFileName = reinterpret_cast<char*>(malloc(strlen(fileName) + 1));
			
			if (FailedToDeleteTextureFileName)
			{
				memcpy(FailedToDeleteTextureFileName, fileName, strlen(fileName) + 1);
				SDL_Time tempTime;
				SDL_GetCurrentTime(&tempTime);
				lastFailedTime = SDL_NS_TO_SECONDS(tempTime);
			}
		}

		bool DrawFailedToDeleteTexturePopup()
		{
			if (lastFailedTime != std::numeric_limits<time_t>::lowest())
			{
				SDL_Time curNSTime;
				SDL_GetCurrentTime(&curNSTime);
				time_t curTime = SDL_NS_TO_SECONDS(curNSTime);
				if (lastFailedTime + errorPopupDuration < curTime)
				{
					if (FailedToDeleteTextureFileName)
					{
						free(FailedToDeleteTextureFileName);
						FailedToDeleteTextureFileName = nullptr;
					}
					lastFailedTime = std::numeric_limits<time_t>::lowest();
				}
			}
			else
				return false;

			if (!FailedToDeleteTextureFileName)
				return false;

			// We specify a default position/size in case there's no data in the .ini file.
			// We only do it to make the demo applications a little more welcoming, but typically this isn't required.
			const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
			//ImGui::SetNextWindowFocus();
			char* errorText;
			size_t errorTextLength = SDL_asprintf(&errorText, "Texture \"%s\" is still in use", FailedToDeleteTextureFileName);

			ImVec2 errorTextSize = ImGui::CalcTextSize(errorText);

			constexpr char errorText2[] = "Remove all references to delete";
			ImVec2 errorTextSize2 = ImGui::CalcTextSize(errorText2);

			ImGuiStyle style = ImGui::GetStyle();

			ImVec2 popupSize(errorTextSize.x, errorTextSize.y + errorTextSize2.y + (style.ItemSpacing.y * 2) + style.FramePadding.y + style.WindowPadding.y);
			ImVec2 popupPos(main_viewport->WorkSize.x - popupSize.x - (style.ItemSpacing.x * 2), main_viewport->WorkSize.y - popupSize.y);
			ImGui::SetNextWindowPos(popupPos, ImGuiCond_Always);
			// this does weird things for some reason
			//ImGui::SetNextWindowSize(errorTextSize);

			//ImGui::OpenPopup("DeleteTextureErrorPopup");
			if (!ImGui::BeginTooltip())//"DeleteTextureErrorPopup", ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs))
			{
				// Early out if the window is collapsed, as an optimization.
				ImGui::EndTooltip();
				return false;
			}

			ImGui::TextUnformatted(errorText);
			ImGui::TextUnformatted(errorText2);

			ImGui::EndTooltip();

			return true;
		}
	}
}