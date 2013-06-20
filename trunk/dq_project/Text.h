#pragma once

#include "Entity/TextRenderComponent.h"
#include <luabind/luabind.hpp>

class Text: public TextRenderComponent
{
public:
	Text(std::string/*, std::string="font1.fnt"*/);
	~Text();

	void OnAdd(Entity* e);

	void setText(std::string txt){GetVar("text")->Set(txt);}
	std::string getText(){return GetVar("text")->GetString();}
//	void setFont(std::string fnt){load_font(fnt);}

	float getWidth(){
		return GetParent()->GetVar("size2d")->GetVector2().x;
	}
	float getHeight(){
		return GetParent()->GetVar("size2d")->GetVector2().y;
	}
};

class LuaText: public Text{
public:
//	LuaText(std::string, std::string);
	LuaText(std::string);
	static void luabind(lua_State* L);

private:
	LuaText(const LuaText&):Text(""){assert(false);}
	LuaText& operator=(const LuaText&){assert(false);}
};