#include "pch.h"
#include "TextureManager.h"

#include "Renderer.h"
#include "STexture.h"
#include "Game/Data/LoadingManager.h"


TextureManager::TextureManager()
{
	DebugPrint(Log, "Texture manager created");
}

TextureManager::~TextureManager()
{
	DebugPrint(Log, "Texture manager destroyed");

	std::unordered_map<FileManager::Folder, TextureMap>::iterator iter;
	for (iter = mTextures.begin(); iter != mTextures.end(); iter++)
	{
		TextureMap textureMap = iter->second;
		textureMap.free();
	}

	mTextures.clear();
}

TextureManager* TextureManager::Get()
{
	GameData& gd = GameData::Get();
	ASSERT(gd.textureManager != nullptr, "Texture manager has no been set up yet");
	return gd.textureManager;
}


void TextureManager::preLoad()
{
	//loadAllTexturesIn(FileManager::PreLoadFiles, FileManager::Image_UI);
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
		FileManager::Folder folder = fm->GetFolderFromPath(folderPaths[i].c_str());
		folders.push_back(folder);
	}

	folders.push_back(FileManager::Maps);

	DebugPrint(Log, "\n--- Loading Textures ---");
	int fails = 0;

	for (int i = 0; i < folders.size(); i++)
	{
#if DEBUG_MODE
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
#if DEBUG_MODE
	int count = 0;
#endif

	std::vector<BasicString> image_paths;
	FileManager::Get()->GetFilesInFolder(resource_folder, image_paths);
	for (const BasicString& path : image_paths)
	{
		if (!FileManager::HasExt(path.c_str(), ".png"))
			continue;

		fails += !loadTexture(textureMap, path.c_str());
#if DEBUG_MODE
		count++;
#endif
	}

#if DEBUG_MODE
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
	STexture *texture = new STexture;

	Renderer* renderer = Renderer::Get();
	renderer->lock();
	if (texture->loadFromFile(filePath, renderer->sdlRenderer()))
	{
		StringBuffer64 label = fm->getItemName(filePath);
		textureMap.add(label.c_str(), texture);

		// Add to has loaded files
		if(LoadingManager* lm = LoadingManager::Get())
			lm->successfullyLoaded(filePath);
		DebugPrint(Log, "Success: loaded texture '%s'", label.c_str());
	}
	else
	{
		StringBuffer64 label = fm->getItemName(filePath);
		DebugPrint(Log, "Failure: texture NOT loaded '%s' at '%s'", label.c_str(), filePath);
		delete texture;
		success = false;
	}

	renderer->unlock();
	return success;
}

StringBuffer64 TextureManager::getTextureName(const STexture* texture) const
{
	std::unordered_map<FileManager::Folder, TextureMap>::const_iterator iter;
	for (iter = mTextures.begin(); iter != mTextures.end(); iter++)
	{
		TextureMap textureMap = iter->second;
		StringBuffer64 id = textureMap.find(texture);

		if (!id.empty())
			return id;
	}

	DebugPrint(Log, "STexture was not found within any texture map");
	return StringBuffer64();
}

STexture* TextureManager::getTexture(const char* label, const FileManager::Folder folder) const
{
	if(label)
	{
		std::vector<const TextureMap*> texture_maps = FindTextureMaps(folder);

		StringBuffer64 buffer = label;
		for (int i = (u32)strlen(label) - 1; i >= 0; i--)
		{
			if (label[i] == '.') 
			{
				buffer.buffer()[i] = '\0';
				break;
			}
		}

	for (u32 i = 0; i < texture_maps.size(); i++)
	{
		const TextureMap* tm = texture_maps[i];
		if (STexture* texture = tm->find(buffer.c_str()))
			return texture;
	}

		//DebugPrint(Warning, "No item in folder map '%d' with label: '%s'", folder, buffer.c_str());
	}
	return nullptr;
}

// --- Priavte Functions --- //
std::vector<const TextureMap* > TextureManager::FindTextureMaps(const FileManager::Folder folder) const
{
	std::vector<const TextureMap*> maps;

	std::vector<FileManager::Folder> folders;
	FileManager::Get()->FindAllFoldersInFolder(folder, folders);

	for (u32 i = 0; i < folders.size(); i++)
	{
		if (mTextures.contains(folders[i]))
		{
			maps.push_back(&mTextures.at(folders[i]));
		}
	}

	if(maps.size() == 0)
		DebugPrint(Warning, "There is no texture Map in the folder '%s'", FileManager::Get()->folderPath(folder).c_str());

	return maps;
}
