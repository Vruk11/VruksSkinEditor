#pragma once
#include <MDLFile.h>

namespace VrukSections
{
	namespace TexturesTable
	{
		bool DrawTexturesTable(MDLFile* fileData);

		bool DrawTexturePathsTable(MDLFile* fileData);

		void StartFailedToDeleteTexturePopup(const char* fileName);

		bool DrawFailedToDeleteTexturePopup();

	}
}