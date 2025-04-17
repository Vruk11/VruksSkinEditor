#pragma once
#include <vector>
#include "MDLFileTab.h"

namespace VrukSections
{
	namespace SkinEditorWindow
	{
		extern std::vector<VrukSections::MDLFileTab*> MDLFileTabs;
		extern int CurrentlySelectedFile;

		enum CloseFileResult_
		{
			CloseFileResult_None = 0,
			CloseFileResult_CloseWithoutSaving,
			CloseFileResult_SaveAs,
			CloseFileResult_Cancel,
		};


		bool ShowSkinEditorWindow(bool& TryingToCloseWindow);
		
		CloseFileResult_ ShowCloseFilePopup();

		bool OpenFileDialog();
		bool SaveFileDialog(int fileTabIndex);

		void Shutdown();
	}

    namespace MenuBar {


		void DrawMenuBar();

		void DrawHelpPopup();
		void DrawLegalPopup();


		//bool IsDesireToCloseDirtyFileOpen() { return EnsureDesireOpen; }

    }
}