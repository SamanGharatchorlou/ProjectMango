#include "pch.h"
#include "TextureManager.h"

#include "Renderer.h"
#include "Texture.h"
#include "Game/Data/LoadingManager.h"


TextureManager::TextureManager()
{
	DebugPrint(Log, "Texture manager created");
}

TextureManager::~TextureManager()
{
	DebugPrint(Log, "Texture manager destroyed");
}

TextureManager* TextureManager::Get()
{
	static TextureManager sInstance;
	return &sInstance;
}

void TextureManager::unload()
{
	std::unordered_map<FileManager::Folder, TextureMap>::iterator iter;
	for (iter = mTextures.begin(); iter != mTextures.end(); iter++)
	{
		TextureMap textureMap = iter->second;
		textureMap.free();
	}

	mTextures.clear();

	DebugPrint(Log, "Texture manager unloaded");
}

void TextureManager::preLoad()
{
	loadAllTexturesIn(FileManager::PreLoadFiles, FileManager::Image_UI);
}

// load all textures here
void TextureManager::load()
{
	FileManager* fm = FileManager::Get();

	// Folders to be loaded
	std::vector<FileManager::Folder> folders;
	std::vector<BasicString> folderPaths = fm->foldersInFolder(FileManager::Images);
	for (int i = 0; i < folderPaths.size(); i++)
	{
		FileManager::Folder folder = fm->getFolderIndex(folderPaths[i].c_str());
		folders.push_back(folder);
	}

	folders.push_back(FileManager::Maps);

	DebugPrint(Log, "\n--- Loading Textures ---");
	int fails = 0;

	for (int i = 0; i < folders.size(); i++)
	{
#if DEBUG_CHECK
		BasicString folderPath = fm->folderPath(folders[i]);
		BasicString rootPath = fm->folderPath(FileManager::Root);
		int start = rootPath.length();
		int length = folderPath.length() - start;
		folderPath = folderPath.substr(start, length);
		DebugPrint(Log, "\nLoading all textures within the folder '%s'", folderPath.c_str());
#endif
		fails += loadAllTexturesIn(folders[i]);
	}

	DebugPrint(Log, "\n--- Texture Loading Complete: %d Failures ---", fails);
}

int TextureManager::loadAllTexturesIn(FileManager::Folder resource_folder, FileManager::Folder placement_folder)
{
	if (placement_folder == FileManager::None)
		placement_folder = resource_folder;

	TextureMap& textureMap = mTextures[placement_folder];

	int fails = 0;
#if DEBUG_CHECK
	int count = 0;
#endif

	std::vector<BasicString> imagePaths = FileManager::Get()->allFilesInFolder(resource_folder);
	for (const BasicString& path : imagePaths)
	{
		if (!FileManager::HasExt(path.c_str(), ".png"))
			continue;

		fails += !loadTexture(textureMap, path.c_str());
#if DEBUG_CHECK
		count++;
#endif
	}

#if DEBUG_CHECK
	if (textureMap.size() != count)
		DebugPrint(Warning, "The final number of textures does not match the number of file paths provided.\ncount (%d) != map size (%d).", count, textureMap.size());
#endif

	return fails;
}

bool TextureManager::loadTexture(TextureMap& textureMap, const char* filePath)
{
	int mapSize = textureMap.size();
	bool success = true;
	FileManager* fm = FileManager::Get();
	Texture *texture = new Texture;

	Renderer::Get()->lock();
	if (texture->loadFromFile(filePath))
	{
		StringBuffer32 label = fm->getItemName(filePath);
		textureMap.add(label.c_str(), texture);

		// Add to has loaded files
		if(LoadingManager* lm = LoadingManager::Get())
			lm->successfullyLoaded(filePath);
		DebugPrint(Log, "Success: loaded texture '%s'", label.c_str());
	}
	else
	{
		StringBuffer32 label = fm->getItemName(filePath);
		DebugPrint(Log, "Failure: texture NOT loaded '%s' at '%s'", label.c_str(), filePath);
		delete texture;
		success = false;
	}

	Renderer::Get()->unlock();
	return success;
}

StringBuffer32 TextureManager::getTextureName(const Texture* texture) const
{
	std::unordered_map<FileManager::Folder, TextureMap>::const_iterator iter;
	for (iter = mTextures.begin(); iter != mTextures.end(); iter++)
	{
		TextureMap textureMap = iter->second;
		StringBuffer32 id = textureMap.find(texture);

		if (!id.empty())
			return id;
	}

	DebugPrint(Log, "Texture was not found within any texture map");
	return StringBuffer32();
}

Texture* TextureManager::getTexture(const char* label, const FileManager::Folder folder) const
{
	Texture* texture = nullptr;
	const TextureMap* textureMap = findTextureMap(folder);

	StringBuffer32 buffer = label;
	for (int i = strlen(label) - 1; i >= 0; i--)
	{
		if (label[i] == '.') 
		{
			buffer.buffer()[i] = '\0';
			break;
		}
	}

	if(textureMap)
		texture = textureMap->find(buffer.c_str());

	if (!texture)
	{
		DebugPrint(Log, "No item in folder map '%d' with label: '%s'", folder, buffer.c_str());

		texture = searchAllFiles(buffer.c_str());
		if (!texture)
		{
			DebugPrint(Warning, "No image file with label: '%s' exists in any loaded folder", buffer.c_str());
		}
	}

	return texture;
}

// --- Priavte Functions --- //
const TextureMap* TextureManager::findTextureMap(const FileManager::Folder folder) const
{
	std::unordered_map<FileManager::Folder, TextureMap>::const_iterator iter;
	for (iter = mTextures.begin(); iter != mTextures.end(); iter++)
	{
		if (folder == iter->first)
			return &iter->second;
	}

	DebugPrint(Warning, "There is no texture Map in the folder '%s'", FileManager::Get()->folderPath(folder).c_str());
	return nullptr;
}

Texture* TextureManager::searchAllFiles(const char* label) const
{
	Texture* texture = nullptr;

	std::unordered_map<FileManager::Folder, TextureMap>::const_iterator iter;
	for (iter = mTextures.begin(); iter != mTextures.end(); iter++)
	{
		const TextureMap* textureMap = findTextureMap(iter->first);
		texture = textureMap->find(label);

		if (texture != nullptr)
			break;
	}

	return texture;
}