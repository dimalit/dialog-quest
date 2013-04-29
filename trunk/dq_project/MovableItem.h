#pragma once

#include <hgeanim.h>

#include <luabind/luabind.hpp>

#include "main.h"
#include "WantFrameUpdate.h"
#include "UserInputDispatcher.h"
#include "Visual.h"

class MovableItem: WantFrameUpdate, MouseInputObject, public Visual
{
public:
	MovableItem(float ax, float ay);
	MovableItem(hgeAnimation *s, float ax = 0.f, float ay = 0.0f);
	~MovableItem();
	virtual void Update(float dt);
	virtual void Render();
	void move(float dx, float dy);
	virtual void onChange();
	virtual void onDragEnd(float mx, float my);

	static void luabind(lua_State* L);
	luabind::object callback;
private:

	virtual bool isPointIn(float mx, float my)
		{return hgeRect(x, y, x+sprite->GetWidth(), y+sprite->GetHeight()).TestPoint(mx, my);}
	virtual void onDrag(float dx, float dy);

	hgeAnimation* sprite;
	float x, y;
	bool selected;
	float prev_x, prev_y;
};
