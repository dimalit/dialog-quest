#pragma once

#include "Entity/TextRenderComponent.h"
#include "Entity/TextBoxRenderComponent.h"
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

// TODO: Not very good from the point of view of code reuse!
class TextBox: public TextBoxRenderComponent
{
public:
	TextBox(std::string txt, int width, int height, eAlignment align);
	~TextBox();

	void OnAdd(Entity* e);

	void setText(std::string txt){GetVar("text")->Set(txt);}
	std::string getText(){return GetVar("text")->GetString();}

	float getWidth(){
		return size.x;
	}
	float getHeight(){
		return size.y;
	}
	void setWidth(int w){
		size.x = w;
		if(GetParent())
			GetParent()->GetVar("size2d")->Set(size);
	}
	void setHeight(int h){
		size.y = h;
		if(GetParent())
			GetParent()->GetVar("size2d")->Set(size);
	}
private:
	CL_Vec2f size;// used when attached
};

class LuaText: public Text{
public:
	LuaText(std::string);
	static void luabind(lua_State* L);

private:
	LuaText(const LuaText&):Text(""){assert(false);}
	LuaText& operator=(const LuaText&){assert(false);}
};

class LuaTextBox: public TextBox{
public:
	LuaTextBox(std::string txt, int width, int height, eAlignment align);
	static void luabind(lua_State* L);

private:
	LuaTextBox(const LuaText&):TextBox("", 0, 0, ALIGNMENT_UPPER_LEFT){assert(false);}
	LuaTextBox& operator=(const LuaText&){assert(false);}
};