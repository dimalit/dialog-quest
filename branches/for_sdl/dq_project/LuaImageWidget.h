#pragma once

#include <string>
#include <cassert>
#include <luabind/luabind.hpp>

#include "LuaWidget.h"
#include "ImageWidget.h"

class LuaImageWidget: public ImageWidget, public LuaWidget
{
public:
	LuaImageWidget(luabind::object conf, float x = 0, float y = 0);
	static void luabind(lua_State* L);

private:
	LuaImageWidget(const LuaWidget& w):Widget(0,0){assert(false);}
	LuaImageWidget& operator=(const LuaWidget& w){assert(false);}
};

