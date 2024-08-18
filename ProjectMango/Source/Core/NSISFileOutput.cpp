#include "pch.h"
#include "NSISFileOutput.h"


// WARNING: a bunch of string changes made here without testing yet
void OutputNSISFolderInfo()
{
	FileManager* fm = FileManager::Get();

	std::unordered_set<BasicString> allFolders;
	fm->AllFoldersContainingFiles(fs::path(fm->folderPath(FileManager::Root).c_str()), allFolders);

	const char* cwdPath = "C:\\Users\\Saman\\RougeLike\\RougeLike\\Resources\\";
	int cwdLength = (int)strlen(cwdPath);

	const int buffer_len = 512;
	char buffer[buffer_len];
	const char* installDir = "${GAME_FILES}";
	const char* resource = "${RESOURCES_ROOT}";

	std::vector<BasicString> defines;
	std::vector<BasicString> outFolders;

	int counter = 0;
	std::unordered_set<BasicString>::const_iterator setIter;
	for (setIter = allFolders.begin(); setIter != allFolders.end(); setIter++)
	{
		StringBuffer64 folderName = fm->getItemName(setIter->c_str());
		BasicString subbed = setIter->substr(cwdLength, setIter->length() - cwdLength);

		snprintf(buffer, buffer_len, "FOLDER_%s_%d", folderName.c_str(), counter);
		defines.push_back(buffer);

		snprintf(buffer, buffer_len, "%s\\%s", resource, subbed.c_str());
		outFolders.push_back(buffer);

		counter++;
	}

	// Print Defines
	for (int i = 0; i < outFolders.size(); i++)
	{
		// !define FOLDER_music "${RESOURCES_ROOT}\Audio\Music"
		DebugPrint(Log, "  !define %s \"%s\"", defines[i].c_str(), outFolders[i].c_str());
	}
	DebugPrint(Log, "\n");

	// SetOutpaths
	for (int i = 0; i < outFolders.size(); i++)
	{	
		// SetOutPath "$INSTDIR\${FOLDER_MUSIC}"
		// File / r "${FOLDER_MUSIC}\*.*"
		DebugPrint(Log, "  SetOutPath \"%s\\${%s}\"", installDir, defines[i].c_str());
		DebugPrint(Log, "  File /r \"${%s}\\*.*\"", defines[i].c_str());
	}
	DebugPrint(Log, "\n");


	// Delete all files
	std::vector<BasicString> allFiles = fm->allFilesInFolder(FileManager::Root);
	for (int i = 0; i < allFiles.size(); i++)
	{
		BasicString subbed = allFiles[i].substr(cwdLength, allFiles[i].length() - cwdLength);

		// Delete "$INSTDIR\${RESOURCES_ROOT}\PreLoadFiles\LoadingBar.png"
		DebugPrint(Log, "  Delete \"%s\\%s\\%s\"", installDir, resource, subbed.c_str());
	}
	DebugPrint(Log, "\n");


	std::unordered_set<BasicString> allGameFolders;
	fm->AllFolders(fs::path(fm->folderPath(FileManager::Root).c_str()), allGameFolders);

	std::list<BasicString> list;
	for (setIter = allGameFolders.begin(); setIter != allGameFolders.end(); setIter++)
	{
		list.push_back(*setIter);
	}
	list.sort(compare_length);

	std::list<BasicString>::const_iterator listIter;
	for (listIter = list.begin(); listIter != list.end(); listIter++)
	{
		BasicString subbed = listIter->substr(cwdLength, listIter->length() - cwdLength);

		// RMDir "$INSTDIR\${FOLDER_MUSIC}"
		DebugPrint(Log, "  RMDir \"%s\\%s\\%s\"", installDir, resource, subbed.c_str());
	}
	DebugPrint(Log, "\n");
}


bool compare_length(const BasicString& first, const BasicString& second)
{
	return first.length() > second.length();
}