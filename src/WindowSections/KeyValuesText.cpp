#include "KeyValuesText.h"
#include <MDLFile.h>
#include <imgui.h>
#include <imgui_stdlib.h>

namespace VrukSections {
	namespace KeyValuesText {
	
		void DrawKeyValuesTextBox(MDLFile* fileData)
		{
			if (ImGui::InputTextMultiline("##source", fileData->GetKeyValues()))
			{
				fileData->isDirty = true;
			}
		}

	}
}