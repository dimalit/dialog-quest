#pragma once

#include <Entity/Component.h>

// something with visual representation
// but without screen position
class ScreenResource
{
	friend class ScreenItem;
	virtual void assignEntity(Entity* e) = 0;
public:
//	virtual void Render(float x, float y, float rot) = 0;
	virtual float getWidth() = 0;
	virtual float getHeight() = 0;
	bool operator==(const ScreenResource& rhs){return false;}
};
