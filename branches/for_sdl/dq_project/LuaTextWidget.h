#pragma once

#include <cassert>
#include <string>
#include <luabind/luabind.hpp>

#include "TextWidget.h"
#include "LuaWidget.h"

class LuaTextWidget: public TextWidget, public LuaWidget
{
public:
	LuaTextWidget(std::string text, float x = 0, float y = 0, luabind::object conf = luabind::object()):Widget(x,y), TextWidget(text, x, y){
		std::string names[] = {"normal", "over", "checked"};
		view_type types[] = {VIEW_NORMAL, VIEW_OVER, VIEW_CHECKED};

		for(int i=0; conf && i<3; i++){
			luabind::object o = conf[names[i].c_str()];
			if(o){
				std::string file = luabind::object_cast<std::string>(o);
				setFont(types[i], file);
			}// if
		}// for
		update_view();		
	}
	static void luabind(lua_State* L);

private:
	LuaTextWidget(const LuaTextWidget& w):Widget(0,0), TextWidget(std::string()){assert(false);}
	LuaTextWidget& operator=(const LuaTextWidget& w){assert(false);}
};
