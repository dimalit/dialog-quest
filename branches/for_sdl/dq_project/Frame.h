#pragma once
#include <cassert>
#include <luabind/luabind.hpp>
#include "screenresource.h"

class Frame: public ScreenResource
{
public:
	Frame(float w, float h);
	~Frame();
	void Render(float x, float y, float rot);
	float getWidth(){return width;}
	float getHeight(){return height;}
	void setWidth(float w){width = w;}
	void setHeight(float h){height = h;}
private:
	float width, height;
};

class LuaFrame: public Frame{
public:
	LuaFrame(float w, float h): Frame(w, h){}
	static void luabind(lua_State* L);

private:
	LuaFrame(const LuaFrame&):Frame(20, 20){assert(false);}
	LuaFrame& operator=(const LuaFrame&){assert(false);}
};