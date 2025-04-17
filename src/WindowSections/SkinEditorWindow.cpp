#include "SkinEditorWindow.h"
#include <imgui.h>
#include <queue>

//#define MDLFileTabs VrukSections::SkinEditorWindow::MDLFileTabs
//extern std::vector<VrukSections::MDLFileTab*> VrukSections::SkinEditorWindow::MDLFileTabs;

//extern int VrukSections::SkinEditorWindow::CurrentlySelectedFile;

//#define MDLFileTabs VrukSections::SkinEditorWindow::MDLFileTabs
//#define CurrentlySelectedFile VrukSections::SkinEditorWindow::CurrentlySelectedFile


namespace VrukSections
{
	namespace SkinEditorWindow
	{
        std::vector<VrukSections::MDLFileTab*> MDLFileTabs;
        int CurrentlySelectedFile = -1;

        std::queue<int> closeQueue;

        static bool firstOpenDialog = false;

        bool ShowSkinEditorWindow(bool& TryingToCloseWindow)
        {
            bool windowResult = true;

            const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y));
            ImGui::SetNextWindowSize(main_viewport->WorkSize);

            if (!ImGui::Begin("VruksSkinEditor", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar))
            {
                // Early out if the window is collapsed, as an optimization.
                ImGui::End();
                return false;
            }

            if (!firstOpenDialog)
            {
                if (OpenFileDialog())
                {
                    CurrentlySelectedFile = MDLFileTabs.size() - 1;
                }
                firstOpenDialog = true;
            }

            VrukSections::MenuBar::DrawMenuBar();

            if (MDLFileTabs.size() > 0)
            {
                int IndexToRemove = -1;
                if (ImGui::BeginTabBar("##filetabs", ImGuiTabBarFlags_Reorderable))
                {
                    for (int i = 0; i < MDLFileTabs.size(); i++)
                    {
                        VrukSections::MDLFileTab* fileTab = MDLFileTabs[i];
                        int tabResult = fileTab->DrawTab(i);

                        if (tabResult & VrukSections::MDLFileTab::TabResult_Selected)
                            CurrentlySelectedFile = i;
                        if (tabResult & VrukSections::MDLFileTab::TabResult_Closed)
                            closeQueue.push(i);
                    }

                    ImGui::EndTabBar();
                }
            }
            else
            {
                //float fontSize = ImGui::GetFontSize();

                //ImVec2 cursorPos = ImGui::GetCursorPos();

                //ImVec2 position{ cursorPos.x + ((main_viewport->WorkSize.x / 2) - (fontSize * 2)), cursorPos.y + (main_viewport->WorkSize.y / 2 - fontSize) };
                //ImGui::SetCursorPos(position);
                //ImGui::Text(":3");
            }

            //ImGui::ShowDemoWindow();

            if (TryingToCloseWindow && closeQueue.empty())
            {
                for (int i = MDLFileTabs.size() - 1; i > -1; i--)
                {
                    closeQueue.push(i);
                }
            }

            if (!closeQueue.empty())
            {
                MDLFileTab* tab = SkinEditorWindow::MDLFileTabs[closeQueue.front()];

                if (tab->isDirty)
                {
                    switch (ShowCloseFilePopup())
                    {
                        case CloseFileResult_CloseWithoutSaving:
                            delete tab;
                            SkinEditorWindow::MDLFileTabs.erase(SkinEditorWindow::MDLFileTabs.begin() + closeQueue.front());

                            closeQueue.pop();
                            break;

                        case CloseFileResult_SaveAs:
                        {
                            if (SaveFileDialog(closeQueue.front()))
                            {
                                delete tab;
                                SkinEditorWindow::MDLFileTabs.erase(SkinEditorWindow::MDLFileTabs.begin() + closeQueue.front());

                                closeQueue.pop();
                            }
                            break;

                        case CloseFileResult_Cancel:
                        {
                            while (!closeQueue.empty())
                                closeQueue.pop();
                            TryingToCloseWindow = false;
                            break;
                        }
                        default:
                            break;
                        }
                    }
                }
                else
                {
                    delete tab;
                    SkinEditorWindow::MDLFileTabs.erase(SkinEditorWindow::MDLFileTabs.begin() + closeQueue.front());

                    closeQueue.pop();
                }
            }
            // return false, so we close the app
            else if (TryingToCloseWindow)
            {
                windowResult = false;
            }

            ImGui::End(); // ImGui::Begin("VruksSkinEditor")


            return windowResult;
        }


        // the way this popup is layed out is really bad because of trying to center the items
        // probably redo this there's a better way
        CloseFileResult_ ShowCloseFilePopup()
        {
            if (closeQueue.empty())
                return CloseFileResult_None;

            ImGuiStyle style = ImGui::GetStyle();
            float heightOfTitleSection = style.ItemSpacing.y;

            // We specify a default position/size in case there's no data in the .ini file.
            // We only do it to make the demo applications a little more welcoming, but typically this isn't required.
            const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
            //ImGui::SetNextWindowFocus();
            ImVec2 popupSize(std::min(500.0f, main_viewport->WorkSize.x / 3), std::min(200.0f, main_viewport->WorkSize.y / 4));
            ImVec2 popupPos(main_viewport->WorkSize.x / 2 - (popupSize.x / 2), main_viewport->WorkSize.y / 4 - (popupSize.y / 2));
            ImGui::SetNextWindowPos(popupPos, ImGuiCond_Always);
            ImGui::SetNextWindowSize(popupSize);

            ImGui::OpenPopup("EnsureCloseFilePopup");
            if (!ImGui::BeginPopup("EnsureCloseFilePopup", ImGuiWindowFlags_NoScrollbar))
            {
                // Early out if the window is collapsed, as an optimization.
                ImGui::EndPopup();
                return CloseFileResult_None;
            }
            char* warningTitle;
            size_t warningTitleLength = SDL_asprintf(&warningTitle, "File %s isn't saved!", SkinEditorWindow::MDLFileTabs[closeQueue.front()]->GetFileName().c_str());

            ImVec2 warningTitleSize = ImGui::CalcTextSize(warningTitle);

            ImGui::SetCursorPosX((popupSize.x / 2) - (warningTitleSize.x / 2));
            ImGui::TextWrapped(warningTitle);
            heightOfTitleSection += ImGui::GetItemRectSize().y + style.ItemSpacing.y;

            constexpr char warningText[] = "Are you sure you want to close it without saving?";
            ImVec2 warningTextSize = ImGui::CalcTextSize(warningText, 0, false, 0.0f);

            ImGui::SetCursorPosX((popupSize.x / 2) - (warningTextSize.x / 2));
            ImGui::TextWrapped(warningText);
            heightOfTitleSection += ImGui::GetItemRectSize().y + style.ItemSpacing.y;

            ImGui::Separator();
            heightOfTitleSection += ImGui::GetItemRectSize().y + style.ItemSpacing.y;

            constexpr const char* optionsText[3] = { "Yes", "Save as..", "Cancel" };

            constexpr float optionsPadding = 50;
            // I wish this could be calculated at compile time, but CalcTextSize uses the font size
            // to calculate the size of the elements
            float optionsWidth = optionsPadding * 2.0f;
            for (int i = 0; i < 3; i++)
                optionsWidth += ImGui::CalcTextSize(optionsText[i]).x;

            ImGui::SetCursorPos(ImVec2((popupSize.x / 2) - (optionsWidth / 2.0f), heightOfTitleSection + (popupSize.y - heightOfTitleSection) / 2 - ImGui::GetFontSize()));

            CloseFileResult_ selection = CloseFileResult_None;
            if (ImGui::Button(optionsText[0]))
            {
                selection = CloseFileResult_CloseWithoutSaving;
            }
            ImGui::SameLine(0, optionsPadding);
            if (ImGui::Button(optionsText[1]))
            {
                selection = CloseFileResult_SaveAs;
            }
            ImGui::SameLine(0, optionsPadding);
            if (ImGui::Button(optionsText[2]))
            {
                selection = CloseFileResult_Cancel;
            }

            ImGui::EndPopup();

            return selection;
        }

        bool OpenFileDialog()
        {
            //int fileDialogResult = OpenMDLFileDialog(filePath, SDL_GL_GetCurrentWindow());
            bool result = false;


            NFD_Init();

            nfdu8char_t* filePath = nullptr;
            nfdu8filteritem_t filters[1] = { { "MDL", "mdl" } };
            nfdopendialogu8args_t args = { 0 };
            args.filterList = filters;
            args.filterCount = 1;
            args.parentWindow = { NFD_WINDOW_HANDLE_TYPE_UNSET, SDL_GL_GetCurrentWindow() };
            nfdresult_t dialogResult = NFD_OpenDialogU8_With(&filePath, &args);
            switch (dialogResult)
            {
            case NFD_OKAY:
            {
                if (!filePath || strcmp(&filePath[strlen(filePath) - 4], ".mdl"))
                    break;

                // "empty name .mdl"
                if (strlen(filePath) - 4 <= 0)
                    break;

                MDLFileTab* newTab = new MDLFileTab();
                if (!newTab->LoadFile(filePath))
                    break;

                puts("Success!");
                puts(filePath);

                newTab->open = true;
                SkinEditorWindow::MDLFileTabs.push_back(newTab);
                VrukSections::SkinEditorWindow::CurrentlySelectedFile = SkinEditorWindow::MDLFileTabs.size() - 1;

                result = true;
                break;
            }
            case NFD_CANCEL:
            {
                puts("User pressed cancel.");
                break;
            }
            case NFD_ERROR:
            {
                printf("Error: %s\n", NFD_GetError());
            }
            }

            NFD_Quit();

            if (filePath)
                free(filePath);
            return result;
        }

        bool SaveFileDialog(int fileTabIndex)
        {
            bool result = false;

            nfdu8char_t* filePath = nullptr;
            nfdu8filteritem_t filters[1] = { { "MDL", "mdl" } };
            nfdsavedialogu8args_t args = { 0 };
            args.filterList = filters;
            args.filterCount = 1;
            args.parentWindow = { NFD_WINDOW_HANDLE_TYPE_UNSET, SDL_GL_GetCurrentWindow() };
            nfdresult_t dialogResult = NFD_SaveDialogU8_With(&filePath, &args);
            switch (dialogResult)
            {
            case NFD_OKAY:
                if (fileTabIndex < 0 || fileTabIndex > SkinEditorWindow::MDLFileTabs.size())
                    break;

                // "empty name .mdl"
                if (strlen(filePath) - 4 <= 0)
                    break;

                puts("Success!");
                puts(filePath);

                result = SkinEditorWindow::MDLFileTabs[fileTabIndex]->SaveFile(filePath);
                break;
            case NFD_CANCEL:
                puts("User pressed cancel.");
                break;
            case NFD_ERROR:
                printf("Error: %s\n", NFD_GetError());
            }

            NFD_Quit();

            if (filePath)
                free(filePath);

            return result;
        }

        void Shutdown()
        {
            for (int i = 0; i < MDLFileTabs.size(); i++)
            {
                VrukSections::MDLFileTab* file = MDLFileTabs[i];
                delete file;
            }
            MDLFileTabs.clear();
        }
	}

    namespace MenuBar {

        static bool HelpWindow_Open = false;
        static bool LegalWindow_Open = false;
        constexpr static char zlibLicenseString[] = "This software is provided 'as-is', without any express or implied warranty.In no event will the authors be held liable for any damages arising from the use of this software.\n\nPermission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions :\n\n1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software.If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.\n2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.\n3. This notice may not be removed or altered from any source distribution.";
        constexpr static char mitLicenseString[] = "The MIT License (MIT)\n\nCopyright(c) 2014 - 2025 Omar Cornut\n\nPermission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files(the \"Software\"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and /or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :\n\n The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.\n\n THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.";

        void DrawMenuBar()
        {
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Load"))
                    {
                        VrukSections::SkinEditorWindow::OpenFileDialog();
                    }
                    if (ImGui::MenuItem("Save As.."))
                    {
                        VrukSections::SkinEditorWindow::SaveFileDialog(SkinEditorWindow::CurrentlySelectedFile);
                    }

                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Info"))
                {
                    if (ImGui::MenuItem("Help"))
                    {
                        HelpWindow_Open = true;
                    }
                    if (ImGui::MenuItem("Legal/Attributions"))
                    {
                        LegalWindow_Open = true;
                    }

                    ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }
            if (HelpWindow_Open)
            {
                DrawHelpPopup();
            }
            if (LegalWindow_Open)
            {
                DrawLegalPopup();
            }
        }

        void DrawHelpPopup()
        {
            const ImGuiViewport* main_viewport = ImGui::GetMainViewport();

            ImVec2 popupSize{ main_viewport->WorkSize.x / 2, main_viewport->WorkSize.y / 2 };

            ImVec2 popupPos{ main_viewport->WorkPos.x + main_viewport->WorkSize.x / 2 - popupSize.x /2, main_viewport->WorkPos.y + main_viewport->WorkSize.y / 2  - popupSize.y /2};

            ImGui::SetNextWindowSize(popupSize, ImGuiCond_Always);
            ImGui::SetNextWindowPos(popupPos, ImGuiCond_Always);


            ImGui::OpenPopup("HelpPopup");
            if(ImGui::BeginPopup("HelpPopup"), &HelpWindow_Open)
            {
                ImVec2 buttonSize{ 20, 20 };

                ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - 20);
                if (ImGui::Button("x", buttonSize))
                {
                    HelpWindow_Open = false;
                }
                ImGui::TextWrapped("To add or remove skins, textures, or texture paths: try right clicking them");
                ImGui::Spacing();
                ImGui::TextWrapped("You can swap skins by dragging and dropping their left-most label onto another skin");
            }
            else
            {
                HelpWindow_Open = false;
            }
            ImGui::EndPopup();
        }

        void DrawLegalPopup()
        {
            const ImGuiViewport* main_viewport = ImGui::GetMainViewport();

            ImVec2 popupSize{ main_viewport->WorkSize.x / 2, main_viewport->WorkSize.y / 2 };

            ImVec2 popupPos{ main_viewport->WorkPos.x + main_viewport->WorkSize.x / 2 - popupSize.x / 2, main_viewport->WorkPos.y + main_viewport->WorkSize.y / 2 - popupSize.y / 2 };

            ImGui::SetNextWindowSize(popupSize, ImGuiCond_Always);
            ImGui::SetNextWindowPos(popupPos, ImGuiCond_Always);

            ImGui::OpenPopup("LegalPopup");
            if (ImGui::BeginPopup("LegalPopup"), &LegalWindow_Open)
            {
                ImVec2 buttonSize{ 20, 20 };

                ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - 20);
                if (ImGui::Button("x", buttonSize))
                {
                    LegalWindow_Open = false;
                }
                ImGui::BeginChild("LegalPopupText");
                {
                    ImGui::TextWrapped("Vruk's MDL Skin Editor is Copyright (c) Vruk under the zlib license");
                    ImGui::TextWrapped("JustVruk@gmail.com");
                    ImGui::Spacing();
                    ImGui::Indent();
                    ImGui::TextWrapped(zlibLicenseString);
                    ImGui::Unindent();

                    ImGui::Spacing();
                    ImGui::Separator();
                    ImGui::Spacing();

                    ImGui::TextWrapped("This program was made with, and made possible, by:");
                    ImGui::Indent();

                    ImGui::Spacing();
                    ImGui::TextLinkOpenURL("Crowbar", "https://github.com/ZeqMacaw/Crowbar");
                    ImGui::SameLine();
                    ImGui::TextWrapped("'s MDL decompiling code that I used as a reference. Crowbar is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License, but they let me put this skin editor under zlib :)");
                    ImGui::Spacing();
                    ImGui::Indent();
                    ImGui::TextLinkOpenURL("The Creative Commons Attribution-ShareAlike 3.0 Unported License", "https://creativecommons.org/licenses/by-sa/3.0/");
                    ImGui::Unindent();

                    ImGui::Spacing();
                    ImGui::Spacing();
                    ImGui::Indent();
                    ImGui::TextLinkOpenURL("The Dear ImGui library", "https://github.com/ocornut/imgui");
                    ImGui::SameLine();
                    ImGui::TextWrapped(" under the MIT license:");
                    ImGui::Spacing();
                    ImGui::Indent();
                    ImGui::TextWrapped(mitLicenseString);
                    ImGui::Unindent();

                    ImGui::Spacing();
                    ImGui::TextLinkOpenURL("Native File Dialog Extended", "https://github.com/btzy/nativefiledialog-extended");
                    ImGui::SameLine();
                    ImGui::TextWrapped(" under the zlib license:");
                    ImGui::Spacing();
                    ImGui::Indent();
                    ImGui::TextWrapped(zlibLicenseString);
                    ImGui::Unindent();

                    ImGui::Spacing();
                    ImGui::TextLinkOpenURL("Simple DirectMedia Layer 3 (SDL3)", "https://github.com/libsdl-org/SDL");
                    ImGui::SameLine();
                    ImGui::TextWrapped(" under the zlib license:");
                    ImGui::Spacing();
                    ImGui::Indent();
                    ImGui::TextWrapped(zlibLicenseString);
                    ImGui::Unindent();

                    ImGui::Spacing();
                    ImGui::TextLinkOpenURL("SDL_image 3.0", "https://github.com/libsdl-org/SDL_image");
                    ImGui::SameLine();
                    ImGui::TextWrapped(" under the zlib license:");
                    ImGui::Spacing();
                    ImGui::Indent();
                    ImGui::TextWrapped(zlibLicenseString);
                    ImGui::Unindent();

                
                }
                ImGui::EndChild(); // LegalPopupText
            }
            else
            {
                LegalWindow_Open = false;
            }
            ImGui::EndPopup();
        }

    }
}