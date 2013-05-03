#pragma once

// something with visual representation
// but without screen position
class ScreenResource
{
public:
	virtual void Render(float x, float y, float rot) = 0;
	virtual float getWidth() = 0;
	virtual float getHeight() = 0;
	bool operator==(const ScreenResource& rhs){return false;}
};
