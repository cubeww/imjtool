#include "ResourceManager.h"

#include <fstream>
#include <filesystem>
#include <json.hpp>

using namespace std;

void ResourceManager::createTexture(string name, string filename)
{
	auto tex = make_shared<sf::Texture>();
	tex->loadFromFile(filename);
	textures[name] = tex;
}

Sprite& ResourceManager::createSprite(string name)
{
	auto spr = make_shared<Sprite>();
	sprites[name] = spr;
	return *spr;
}

void ResourceManager::createSound(string name, string filename)
{
	auto buf = make_shared<sf::SoundBuffer>();
	buf->loadFromFile(filename);
	auto snd = make_shared<sf::Sound>();
	snd->setBuffer(*buf);
	soundBuffers[name] = buf;
	sounds[name] = snd;
}

void ResourceManager::loadTextures()
{
	ifstream f("textures/define.json");
	nlohmann::json j;
	f >> j;
	for (auto i : j)
	{
		string filename = i["file"];
		string name = filesystem::path(filename).stem().string();

		auto x = 1;
		auto y = 1;
		if (i["x"] != nullptr)
			x = i["x"];
		if (i["y"] != nullptr)
			y = i["y"];

		createTexture(name, "textures/" + filename);
		createSprite(name).addSheet(textures[name], x, y);
	}
	f.close();
}

void ResourceManager::loadSounds()
{
	ifstream f("sfx/define.json");
	nlohmann::json j;
	f.open("sfx/define.json");
	f >> j;
	for (auto i : j)
	{
		string filename = i["file"];
		string name = filesystem::path(filename).stem().string();

		createSound(name, "sfx/" + filename);
	}
	f.close();
}


