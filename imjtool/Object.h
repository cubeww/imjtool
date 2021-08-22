#pragma once

#include <iostream>
#include <SFML/Graphics/Color.hpp>
#include <bitset>

#include "Game.h"

class Sprite;
enum class Index;

using namespace std;

class Object
{
public:
	int index = 0;
	bool needDestroy = false;

	int bboxLeft = -1;
	int bboxRight = -1;
	int bboxTop = -1;
	int bboxBottom = -1;

	shared_ptr<Sprite> sprite;
	shared_ptr<Sprite> maskSprite;

	float imageIndex = 0;
	float imageSpeed = 0;

	int depth = 0;

	float x = 0;
	float y = 0;

	float xorigin = 0;
	float yorigin = 0;

	float maskXorigin = 0;
	float maskYorigin = 0;

	float xprevious = 0;
	float yprevious = 0;

	float xscale = 1;
	float yscale = 1;

	float rotation = 0;

	sf::Color color = sf::Color::White;

	float hspeed = 0;
	float vspeed = 0;

	float gravity = 0;

	bool collisionLayers[256];

	void setSprite(string name, bool setMask = true);
	void setMask(string name);
	void setOrigin(float x, float y, bool setMask = true);
	void setMaskOrigin(float x, float y);
	void addCollision(Index layer);
	void updateSprite();
	void drawSelf();
	void drawMask();

	void calcBBox();

	shared_ptr<Object> placeMeeting(float x, float y, Index layer);
	void moveContact(float hspeed, float vspeed, Index layer);

	virtual void create()
	{
	}

	virtual void update()
	{
	}
};
