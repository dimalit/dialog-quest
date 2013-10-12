#pragma once

#include "Entity/InputTextRenderComponent.h"
#include <luabind/luabind.hpp>

class TextInput: public InputTextRenderComponent{
public:
	TextInput(int width, eFont font=FONT_SMALL);
	~TextInput();

	void OnAdd(Entity* e);

	void setText(std::string txt){
		GetVar("text")->Set(txt);
	}
	std::string getText(){return GetVar("text")->GetString();}

	float getWidth(){
		return GetParent()->GetVar("size2d")->GetVector2().x;
	}
	float getHeight(){
		return GetParent()->GetVar("size2d")->GetVector2().y;
	}
	void setWidth(float w){
		width = w;
		int h = getHeight();
		GetParent()->GetVar("size2d")->Set(width, h);	
	}
	eFont getFont(){
		return (eFont)GetVar("font")->GetUINT32();
	}
	void setFont(eFont f){
		GetVar("font")->Set(uint32(f));
		float h = GetBaseApp()->GetFont(f)->GetLineHeight(1.0f);
		if(GetParent())
			GetParent()->GetVar("size2d")->Set(getWidth(), h+6);
	}
private:
	float width;
};

class LuaTextInput: public TextInput{
public:
	LuaTextInput(float w);
	LuaTextInput(float w, eFont font);
	static void luabind(lua_State* L);

private:
	LuaTextInput(const LuaTextInput&):TextInput(0){assert(false);}
	LuaTextInput& operator=(const LuaTextInput&){assert(false);}
};