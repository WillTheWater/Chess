#include "Framework/AssetManager.h"

namespace we
{
	unique<AssetManager> AssetManager::UAssetManager{nullptr};

	AssetManager& AssetManager::GetAssetManager()
	{
		if (!UAssetManager)
		{
			UAssetManager = std::move(unique<AssetManager>{new AssetManager});
		}
		return *UAssetManager;
	}

	shared<sf::Texture> AssetManager::LoadTexture(const std::string& TexturePath)
	{
		auto FoundTexture = LoadedTextures.find(TexturePath);
		if (FoundTexture != LoadedTextures.end())
		{
			return FoundTexture->second;
		}
		shared<sf::Texture> NewTexture{ new sf::Texture };
		if (NewTexture->loadFromFile(RootDirectory + TexturePath))
		{
			LoadedTextures.insert({ TexturePath, NewTexture });
			return NewTexture;
		}
		return shared<sf::Texture> {nullptr};
	}

	shared<sf::Font> AssetManager::LoadFont(const string& FontPath)
	{
		auto FoundFont = LoadedFonts.find(FontPath);
		if (FoundFont != LoadedFonts.end())
		{
			return FoundFont->second;
		}
		shared<sf::Font> NewFont{ new sf::Font };
		if (NewFont->openFromFile(RootDirectory + FontPath))
		{
			LoadedFonts.insert({ FontPath, NewFont });
			return NewFont;
		}
		return shared<sf::Font> {nullptr};
	}

	void AssetManager::GarbageCollectionCycle()
	{
		for (auto i = LoadedTextures.begin(); i != LoadedTextures.end();)
		{
			if (i->second.unique())
			{
				i = LoadedTextures.erase(i);
			}
			else
			{
				i++;
			}
		}

		for (auto i = LoadedFonts.begin(); i != LoadedFonts.end();)
		{
			if (i->second.unique())
			{
				i = LoadedFonts.erase(i);
			}
			else
			{
				i++;
			}
		}
	}

	void AssetManager::SetAssetRootDirctory(const std::string& Directory)
	{
		RootDirectory = Directory;
	}

	AssetManager::AssetManager()
		: RootDirectory{}
	{

	}
}