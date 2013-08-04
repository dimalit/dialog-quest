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
	TextBox(std::string txt, int width, eAlignment align);
	~TextBox();

	void OnAdd(Entity* e);

	void setText(std::string txt){GetVar("text")->Set(txt);}
	std::string getText(){return GetVar("text")->GetString();}

	float getWidth(){
		return width;
	}
	float getHeight(){
		if(GetParent())
			return GetParent()->GetVar("size2d")->GetVector2().y;
		else{
			assert(0);
			return -1;
		}//else
	}
	void setWidth(int w){
		width = w;
		if(GetParent())
			GetParent()->GetVar("size2d")->Set(w, 0);
	}
	//void setHeight(int h){
	//	size.y = h;
	//	if(GetParent())
	//		GetParent()->GetVar("size2d")->Set(size);
	//}
private:
	int width;// used when attached
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
	LuaTextBox(std::string txt, int width, eAlignment align);
	static void luabind(lua_State* L);

private:
	LuaTextBox(const LuaText&):TextBox("", 0, ALIGNMENT_UPPER_LEFT){assert(false);}
	LuaTextBox& operator=(const LuaText&){assert(false);}
};