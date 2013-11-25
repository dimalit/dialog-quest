#pragma once

#include "LuaScreenItem.h"

#include "Entity/InputTextRenderComponent.h"

#include <luabind/luabind.hpp>

class TextInputItem: virtual public ScreenItem{
public:
	TextInputItem(int width, eFont font=FONT_SMALL);
	~TextInputItem();

	// I know my size better: ignore
	virtual void setWidth(float w){
		if(getParent())
			getParent()->requestLayOut();
	}
	virtual void setHeight(float h){
		if(getParent())
			getParent()->requestLayOut();
	}

	void setText(std::string txt){
		component->GetVar("text")->Set(txt);
	}
	std::string getText(){return component->GetVar("text")->GetString();}

	eFont getFont(){
		return (eFont)component->GetVar("font")->GetUINT32();
	}
	void setFont(eFont f){
		component->GetVar("font")->Set(uint32(f));
		float h = GetBaseApp()->GetFont(f)->GetLineHeight(1.0f);
		entity->GetVar("size2d")->Set(getWidth(), h+6);
	}
private:
	InputTextRenderComponent* component;
};

class LuaTextInputItem: public TextInputItem, public LuaScreenItem{
public:
	LuaTextInputItem(float w);
	LuaTextInputItem(float w, eFont font);
	static void luabind(lua_State* L);

private:
	LuaTextInputItem(const LuaTextInputItem&):TextInputItem(0){assert(false);}
	LuaTextInputItem& operator=(const LuaTextInputItem&){assert(false);}
};