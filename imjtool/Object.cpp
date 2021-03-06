#include "Object.h"

#include "Game.h"
#include "InGame.h"

void Object::addCollision(Index layer)
{
	collisionLayers[static_cast<int>(layer)] = true;
}

void Object::removeCollision(Index layer)
{
	collisionLayers[static_cast<int>(layer)] = false;
}

void Object::setSprite(string name, bool setMask)
{
	sprite = ResMgr.sprites[name];
	if (setMask)
	{
		maskSprite = sprite;
	}
}

void Object::setMask(string name)
{
	maskSprite = ResMgr.sprites[name];
}

void Object::updateSprite()
{
	imageIndex += imageSpeed;
}

void Object::drawSelf()
{
	if (highlight)
	{
		color = sf::Color(255, 204, 204);
		if (highlightTimer-- == 0)
		{
			color = sf::Color::White;
			highlight = false;
		}
	}

	if (sprite != nullptr)
		sprite->draw(FloorToInt(imageIndex), x, y, sprite->xOrigin, sprite->yOrigin, xscale, yscale, rotation, color);
}

void Object::setHighlight()
{
	highlight = true;
	highlightTimer = 10;
}

void Object::drawMask()
{
	if (maskSprite != nullptr)
		maskSprite->draw(FloorToInt(imageIndex), x, y, maskSprite->xOrigin, maskSprite->yOrigin, xscale, yscale, rotation, color);
}

shared_ptr<Object> Object::placeMeeting(float x, float y, Index layer)
{
	auto oldX = this->x;
	auto oldY = this->y;

	this->x = x;
	this->y = y;

	calcBBox();

	auto& item1 = maskSprite->items[static_cast<int>(imageIndex) % maskSprite->items.size()];

	auto scale1x = 1.0f / xscale;
	auto scale1y = 1.0f / yscale;

	for (const auto& obj : ObjMgr.objects)
	{
		if (obj->maskSprite == nullptr || !obj->collisionLayers[static_cast<int>(layer)] || obj.get() == this)
			continue;

		obj->calcBBox();

		auto scale2x = 1.0f / obj->xscale;
		auto scale2y = 1.0f / obj->yscale;

		auto& item2 = obj->maskSprite->items[static_cast<int>(obj->imageIndex) % obj->maskSprite->items.size()];

		auto l = max(bboxLeft, obj->bboxLeft);
		auto r = min(bboxRight, obj->bboxRight);
		auto t = max(bboxTop, obj->bboxTop);
		auto b = min(bboxBottom, obj->bboxBottom);

		// no rotation or scaling
		if (equalF(scale1x, 1) && equalF(scale2x, 1) && equalF(scale1y, 1) && equalF(scale2y, 1) && equalF(rotation, 0) && equalF(obj->rotation, 0))
		{
			for (auto j = t; j <= b; j++)
			{
				for (auto i = l; i <= r; i++)
				{
					auto xx = RoundToInt(i - x + maskSprite->xOrigin);
					if (xx < 0 || xx >= item1->w) continue;
					auto yy = RoundToInt(j - y + maskSprite->yOrigin);
					if (yy < 0 || yy >= item1->h) continue;
					if (!item1->data[xx + yy * item1->w]) continue;

					xx = RoundToInt(i - obj->x + obj->maskSprite->xOrigin);
					if (xx < 0 || xx >= item2->w) continue;
					yy = RoundToInt(j - obj->y + obj->maskSprite->yOrigin);
					if (yy < 0 || yy >= item2->h) continue;
					if (!item2->data[xx + yy * item2->w]) continue;

					this->x = oldX;
					this->y = oldY;
					return obj;
				}
			}
		}

		// scaling but no rotation
		else if (equalF(rotation, 0) && equalF(obj->rotation, 0))
		{
			for (auto j = t; j <= b; j++)
			{
				for (auto i = l; i <= r; i++)
				{
					auto xx = FloorToInt((i - x) * scale1x + maskSprite->xOrigin);
					if (xx < 0 || xx >= item1->w) continue;
					auto yy = FloorToInt((j - y) * scale1y + maskSprite->yOrigin);
					if (yy < 0 || yy >= item1->h) continue;
					if (!item1->data[xx + yy * item1->w]) continue;

					xx = FloorToInt((i - obj->x) * scale2x + obj->maskSprite->xOrigin);
					if (xx < 0 || xx >= item2->w) continue;
					yy = FloorToInt((j - obj->y) * scale2y + obj->maskSprite->yOrigin);
					if (yy < 0 || yy >= item2->h) continue;
					if (!item2->data[xx + yy * item2->w]) continue;

					this->x = oldX;
					this->y = oldY;
					return obj;
				}
			}
		}

		// scaling and rotation
		else
		{
			auto ss1 = sin(-rotation * PI / 180);
			auto cc1 = cos(-rotation * PI / 180);
			auto ss2 = sin(-obj->rotation * PI / 180);
			auto cc2 = cos(-obj->rotation * PI / 180);

			for (auto j = t; j <= b; j++)
			{
				for (auto i = l; i <= r; i++)
				{
					auto xx = FloorToInt((cc1 * (i - x) + ss1 * (j - y)) * scale1x + maskSprite->xOrigin);
					if (xx < 0 || xx >= item1->w) continue;
					auto yy = FloorToInt((cc1 * (j - y) - ss1 * (i - x)) * scale1y + maskSprite->yOrigin);
					if (yy < 0 || yy >= item1->h) continue;
					if (!item1->data[xx + yy * item1->w]) continue;

					xx = FloorToInt((cc2 * (i - obj->x) + ss2 * (j - obj->y)) * scale2x + obj->maskSprite->xOrigin);
					if (xx < 0 || xx >= item2->w) continue;
					yy = FloorToInt((cc2 * (j - obj->y) - ss2 * (i - obj->x)) * scale2y + obj->maskSprite->yOrigin);
					if (yy < 0 || yy >= item2->h) continue;
					if (!item2->data[xx + yy * item2->w]) continue;

					this->x = oldX;
					this->y = oldY;
					return obj;
				}
			}
		}
	}
	this->x = oldX;
	this->y = oldY;
	return nullptr;
}

void Object::moveContact(float hspeed, float vspeed, Index layer)
{
	if (placeMeeting(x, y, layer) != nullptr)
		return;

	auto len = sqrtf(powf(hspeed, 2) + powf(vspeed, 2));
	auto dx = hspeed / len;
	auto dy = vspeed / len;
	len = RoundToInt(len);

	for (auto i = 0; i <= len; i++)
	{
		if (placeMeeting(x + dx, y + dy, layer) == nullptr)
		{
			x += dx;
			y += dy;
		}
		else
		{
			return;
		}
	}
}

void Object::calcBBox()
{
	auto& item = maskSprite->items[FloorToInt(imageIndex) % maskSprite->items.size()];

	if (equalF(rotation, 0))
	{
		bboxLeft = RoundToInt(x + xscale * (item->left - maskSprite->xOrigin));
		bboxRight = RoundToInt(x + xscale * (item->right - maskSprite->xOrigin + 1) - 1);

		if (bboxLeft > bboxRight)
			swap(bboxLeft, bboxRight);

		bboxTop = RoundToInt(y + yscale * (item->top - maskSprite->yOrigin));
		bboxBottom = RoundToInt(y + yscale * (item->bottom - maskSprite->yOrigin + 1) - 1);

		if (bboxTop > bboxBottom)
			swap(bboxTop, bboxBottom);
	}
	else
	{
		auto xmin = xscale * (item->left - maskSprite->xOrigin);
		auto xmax = xscale * (item->right - maskSprite->xOrigin + 1) - 1;

		auto ymin = yscale * (item->top - maskSprite->yOrigin);
		auto ymax = yscale * (item->bottom - maskSprite->yOrigin + 1) - 1;

		auto cc = cos(rotation * PI / 180);
		auto ss = sin(rotation * PI / 180);

		auto ccXmax = cc * xmax;
		auto ccXmin = cc * xmin;
		auto ssYmax = ss * ymax;
		auto ssYmin = ss * ymin;

		if (ccXmax < ccXmin)
			swap(ccXmax, ccXmin);

		if (ssYmax < ssYmin)
			swap(ssYmax, ssYmin);

		bboxLeft = FloorToInt(x + ccXmin + ssYmin);
		bboxRight = FloorToInt(x + ccXmax + ssYmax);

		auto ccYmax = cc * ymax;
		auto ccYmin = cc * ymin;
		auto ssXmax = ss * xmax;
		auto ssXmin = ss * xmin;
		if (ccYmax < ccYmin)
			swap(ccYmax, ccYmin);

		if (ssXmax < ssXmin)
			swap(ssXmax, ssXmin);

		bboxTop = FloorToInt(y + ccYmin - ssXmax);
		bboxBottom = FloorToInt(y + ccYmax - ssXmin);
	}
}

void Object::applySkin()
{
	auto name = spriteOf(index);
	auto obj = SkinMgr.getCurrent(name);
	if (obj != nullptr)
	{
		sprite = obj->sprite;
		if (!isnan(obj->speed))
		{
			imageSpeed = obj->speed;
		}
	}
	else sprite = ResMgr.sprites[name];
}
