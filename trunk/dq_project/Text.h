#pragma once
#include <hgeFont.h>
#include "main.h"
#include "ScreenResource.h"

class Text: public ScreenResource
{
public:
	Text(std::string, std::string="font1.fnt");
	~Text();

	void setText(std::string txt){this->text = txt;}
	std::string getText(){return this->text;}
	void setFont(std::string fnt){load_font(fnt);}

	void Render(float x, float y, float rot);
	float getWidth(){return fnt->GetStringWidth(text.c_str());}
	float getHeight(){return fnt->GetHeight();}

private:
	std::string text;
	hgeFont *fnt;

	// utilitary
	void load_font(std::string path);
};

class LuaText: public Text{
public:
	LuaText(std::string, std::string);
	LuaText(std::string);
	static void luabind(lua_State* L);

private:
	LuaText(const LuaText&):Text("", ""){assert(false);}
	LuaText& operator=(const LuaText&){assert(false);}
};