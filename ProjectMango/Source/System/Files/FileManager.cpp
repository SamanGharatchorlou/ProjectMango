#include "pch.h"
#include "FileManager.h"

#include <stdio.h>
#include <stdlib.h>
#include <fstream>


FileManager* FileManager::Get()
{
	static FileManager sInstance;
	return &sInstance;
}


void FileManager::init()
{
	folderPaths[None] = ".";
	folderPaths[Root] = pathToString(fs::current_path()) + "\\Resources\\";

	folderPaths[PreLoadFiles] = folderPaths[Root] + "PreLoadFiles\\";

	// Maps
	folderPaths[Maps] = folderPaths[Root] + "Maps\\";

	// Images
	folderPaths[Images] = folderPaths[Root] + "Images\\";
	folderPaths[Image_UI] = folderPaths[Images] + "UI\\";
	folderPaths[Image_Maps] = folderPaths[Images] + "Maps\\";
	folderPaths[Image_Weapons] = folderPaths[Images] + "Weapons\\";
	folderPaths[Image_Animations] = folderPaths[Images] + "Animations\\";

	// Audio
	folderPaths[Audio] = folderPaths[Root] + "Audio\\";
	folderPaths[Audio_Music] = folderPaths[Audio] + "Music\\";
	folderPaths[Audio_Sound] = folderPaths[Audio] + "Sound\\";
	folderPaths[Audio_SoundGroups] = folderPaths[Audio] + "SoundGroups\\";

	// Font
	folderPaths[Font] = folderPaths[Root] + "Font\\";

	// Configs
	folderPaths[Configs] = folderPaths[Root] + "Configs\\";
	folderPaths[Config_Animations] = folderPaths[Configs] + "Animations\\";
	folderPaths[Config_Data] = folderPaths[Configs] + "Data\\";

	for (int i = 0; i < Folder::Count; i++)
	{
		ASSERT(!folderPaths[(Folder)i].empty(), "The enum %d in the folderPath map has not been defined", i);
	}
}

void FileManager::free()
{
	folderPaths.clear();
}


FileManager::Folder FileManager::getFolderIndex(const char* directory)
{
	for (int i = 0; i < Folder::Count; i++)
	{
		BasicString directory_path = directory;
		if (directory_path + "\\" == folderPaths[(Folder)i])
			return static_cast<Folder>(i);
	}

	DebugPrint(Warning, "Folder path '%s' is not in the folder array", directory);
	return Folder::None;
}


BasicString FileManager::folderPath(const Folder folder) const
{
	BasicString buffer;

	if (folder < Folder::Count)
	{
		buffer = folderPaths.at(folder).c_str();
	}
	else
	{
		DebugPrint(Warning, "No folder found with folder enum %d", folder);
		buffer.clear();
	}

	return buffer;
}

fs::path FileManager::fsPath(const Folder folder) const
{
	BasicString buffer;

	if (folder < Folder::Count)
	{
		buffer = folderPaths.at(folder).c_str();

		if (!fs::is_directory(fs::path(buffer.c_str())))
		{
			DebugPrint(Warning, "Folder defined at enum %d but does not exist at filepath: %s", folder, buffer.c_str());
			buffer.clear();
		}
	}
	else
	{
		DebugPrint(Warning, "No folder found with folder enum %d", folder);
		buffer.clear();
	}

	return fs::path(buffer.c_str());
}


bool FileManager::exists(const Folder folder, const char* name) const
{
	BasicString outPath("");

	fs::path folder_path = fsPath(folder);
	if (!folder_path.empty())
	{
		for (const auto& directoryPath : fs::directory_iterator(folder_path))
		{
			if (!fs::is_directory(directoryPath) && getItemName(directoryPath.path()) == name)
			{
				return true;
			}
			// if directory, search all sub folders
			else if (fs::is_directory(directoryPath))
			{
				BasicString outPath("");
				outFilePath(outPath, directoryPath.path(), name);

			}

			if (!outPath.empty())
				return true;
		}
	}

	return false;
}

BasicString FileManager::findFile(const Folder folder, const char* name) const
{
	BasicString outPath("");

	fs::path folder_path = fsPath(folder);
	if (!folder_path.empty())
	{
		for (const auto& directoryPath : fs::directory_iterator(folder_path))
		{
			if (!fs::is_directory(directoryPath) && StringCompare(getItemName(directoryPath.path()).c_str(), name))
			{
				outPath = pathToString(directoryPath.path());
			}
			else if (fs::is_directory(directoryPath))
			{
				outFilePath(outPath, directoryPath.path(), name);
			}

			if (!outPath.empty())
				return outPath;
		}
	}

	DebugPrint(Warning, "No file named '%s' was found in the folder '%s'", name, folderPath(folder).c_str());
	return outPath;
}

BasicString FileManager::findFileEtx(const Folder folder, const char* name) const
{
	BasicString outPath("");

	fs::path folder_path = fsPath(folder);
	if (!folder_path.empty())
	{
		for (const auto& directoryPath : fs::directory_iterator(folder_path))
		{
			if (!fs::is_directory(directoryPath) && StringCompare(getItemNameAndExt(directoryPath.path()).c_str(), name))
			{
				outPath = pathToString(directoryPath.path());
			}
			else if (fs::is_directory(directoryPath))
			{
				outFilePath(outPath, directoryPath.path(), name);
			}

			if (!outPath.empty())
				return outPath;
		}
	}

	DebugPrint(Warning, "No file named '%s' was found in the folder '%s'", name, folderPath(folder).c_str());
	return outPath;

}

StringBuffer64 FileManager::getItemName(const char* filePath) const
{
	char fileName[50];
	errno_t error = _splitpath_s(filePath, NULL, 0, NULL, 0, fileName, 50, NULL, 0);
	return StringBuffer64(fileName);
}


StringBuffer64 FileManager::getItemName(const fs::path& filePath) const
{
	char fileName[50];
	errno_t error = _splitpath_s(pathToString(filePath).c_str(), NULL, 0, NULL, 0, fileName, 50, NULL, 0);
	return StringBuffer64(fileName);
}

StringBuffer64 FileManager::getItemNameAndExt(const fs::path& filePath) const
{
	char fileName[26];
	char ext[6];
	errno_t error = _splitpath_s(pathToString(filePath).c_str(), NULL, 0, NULL, 0, fileName, 26, ext, 6);
	return StringBuffer64(fileName) + ext;
}


bool FileManager::HasExt(const char* filePath, const char* extension)
{
	StringBuffer64 buffer;
	errno_t error = _splitpath_s(filePath, NULL, 0, NULL, 0, NULL, 0, buffer.buffer(), 6);
	return buffer == extension;
}

std::vector<BasicString> FileManager::fullPathsInFolder(const Folder folder) const
{
	std::vector<BasicString> fileNameList;

	fs::path folder_path = fsPath(folder);
	if (!folder_path.empty())
	{
		for (const auto& fullFilePath : fs::directory_iterator(folder_path))
		{
			fileNameList.push_back(pathToString(fullFilePath.path()));
		}
	}

	return fileNameList;
}

std::vector<BasicString> FileManager::fullPathsInFolder(const char* directoryPath) const
{
	std::vector<BasicString> fileNameList;
	if (strlen(directoryPath) > 0)
	{
		for (const auto& fullFilePath : fs::directory_iterator(directoryPath))
		{
			fileNameList.push_back(pathToString(fullFilePath.path()));
		}
	}

	return fileNameList;
}


// TODO: will also get folder names?
std::vector<BasicString> FileManager::fileNamesInFolder(const Folder folder) const
{
	std::vector<BasicString> fileList = allFilesInFolder(folder);
	std::vector<BasicString> fileNameList;

	for( u32 i = 0; i < fileList.size(); i++ )
	{
		fileNameList.push_back( getItemName(fileList[i].c_str()).c_str() );
	}

	return fileNameList;
}


std::vector<BasicString> FileManager::allFilesInFolder(const Folder folder) const
{
	std::vector<BasicString> fileNameList;

	fs::path folder_path = fsPath(folder);
	if (!folder_path.empty())
	{
		for (const auto& path : fs::directory_iterator(folder_path))
		{
			if (fs::is_directory(path))
				addFilesToList(fileNameList, path.path());
			else
				fileNameList.push_back(pathToString(path.path()));
		}
	}

	return fileNameList;
}

std::vector<BasicString> FileManager::allFilesInFolder(const fs::path& directoryPath) const
{
	std::vector<BasicString> fileNameList;

	if (!directoryPath.empty())
	{
		for (const auto& path : fs::directory_iterator(directoryPath))
		{
			if (fs::is_directory(path))
				addFilesToList(fileNameList, path.path());
			else
				fileNameList.push_back(pathToString(path.path()));
		}
	}

	return fileNameList;
}

// Stores all folders the contains at least 1 immeditate file
void FileManager::AllFoldersContainingFiles(const fs::path& directoryPath, std::unordered_set<BasicString>& folderList) const
{
	if (directoryPath.empty())
		return;

	for (const auto& path : fs::directory_iterator(directoryPath))
	{
		if (containsFile(directoryPath))
		{
			folderList.insert(pathToString(directoryPath));
		}

		if (fs::is_directory(path))
		{
			AllFoldersContainingFiles(path, folderList);
		}
	}
}

void FileManager::AllFolders(const fs::path& directoryPath, std::unordered_set<BasicString>& folderList) const
{
	if (directoryPath.empty())
		return;

	for (const auto& path : fs::directory_iterator(directoryPath))
	{
		if (fs::is_directory(path))
		{
			folderList.insert(pathToString(path));
			AllFolders(path, folderList);
		}
	}
}


bool FileManager::containsFile(const fs::path& directoryPath) const
{
	if (!directoryPath.empty())
	{
		for (const auto& path : fs::directory_iterator(directoryPath))
		{
			if (!fs::is_directory(path))
				return true;
		}
	}

	return false;
}

bool FileManager::containsDirectory(const fs::path& directoryPath) const
{
	if (!directoryPath.empty())
	{
		for (const auto& path : fs::directory_iterator(directoryPath))
		{
			if (fs::is_directory(path))
				return true;
		}
	}

	return false;
}


std::vector<BasicString> FileManager::foldersInFolder(const Folder folder) const
{
	std::vector<BasicString> folderPathsList;

	fs::path folder_path = fsPath(folder);
	if (!folder_path.empty())
	{
		for (const auto& path : fs::directory_iterator(folder_path))
		{
			if (fs::is_directory(path))
			{
				folderPathsList.push_back(pathToString(path.path()));
			}
		}
	}

	return folderPathsList;
}


// --- Private Functions --- //
void FileManager::addFilesToList(std::vector<BasicString>& fileList, const fs::path& directoryPath) const
{
	if (directoryPath.empty())
		return;

	for (const auto& path : fs::directory_iterator(directoryPath))
	{
		if (fs::is_directory(path))
			addFilesToList(fileList, path);
		else
			fileList.push_back(pathToString(path.path()));
	}
}


void FileManager::addFoldersToList(std::vector<BasicString>& folderList, const fs::path& directoryPath) const
{
	if (directoryPath.empty())
		return;

	for (const auto& path : fs::directory_iterator(directoryPath))
	{
		if (fs::is_directory(path))
			addFoldersToList(folderList, path);
		else
			folderList.push_back(pathToString(path.path()));
	}
}


void FileManager::outFilePath(BasicString& outValue, const fs::path& directoryPath, const StringBuffer64& name) const
{
	if (directoryPath.empty())
		return;

	for (const auto& dirPath : fs::directory_iterator(directoryPath))
	{
		if (!fs::is_directory(dirPath) && getItemName(dirPath.path()) == name)
		{
			outValue = pathToString(dirPath.path());
		}
		else if (fs::is_directory(dirPath))
		{
			outFilePath(outValue, dirPath.path(), name);
		}

		// End recursion
		if (!outValue.empty())
			return;
	}
}


void FileManager::outFolderPath(BasicString& outValue, const fs::path& directoryPath, const StringBuffer64& name) const
{
	if (directoryPath.empty())
		return;

	for (const auto& path : fs::directory_iterator(directoryPath))
	{
		if (fs::is_directory(path))
		{
			if (getItemName(path.path()) == name)
			{
				outValue = pathToString(path.path());
			}
			else if (fs::is_directory(path))
			{
				outFolderPath(outValue, path.path().string(), name);
			}
		}

		// End recursion
		if (!outValue.empty())
			return;
	}
}


BasicString pathToString(const fs::path& path)
{
	return BasicString(path.string().c_str());
}