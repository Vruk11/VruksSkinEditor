#include <MDLFile.h>

static int FindFileNameIndexInPath(std::string filePath)
{
	return filePath.find_last_of("/\\") + 1;
}

namespace VrukSections {
	class MDLFileTab
	{
	public:
		MDLFileTab() { open = false; isDirty = false; File = nullptr; };
		~MDLFileTab() {
			delete File;
		};

		enum
		{
			TabResult_None = 0,
			TabResult_Selected = 1 << 1,
			TabResult_Closed = 1 << 2,
		};

		virtual bool LoadFile(const char* filePath);
		virtual bool SaveFile(const char* filePath);

		virtual int DrawTab(int tabIndex);

		std::string GetFileName() { return this->fileName; };
		std::string GetFilePath() { return this->filePath; };

		bool open;
		bool isDirty;
	private:
		std::string fileName;
		std::string filePath;

		MDLFile* File;
	};
}