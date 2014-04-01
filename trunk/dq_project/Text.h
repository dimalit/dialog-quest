#pragma once

#include "LuaScreenItem.h"

#include "Entity/TextRenderComponent.h"
#include "Entity/TextBoxRenderComponent.h"

#include <luabind/luabind.hpp>

class TextItem: virtual public ScreenItem
{
public:
	TextItem(std::string, eFont font=FONT_SMALL);
	~TextItem();

	void setText(std::string txt){component->GetVar("text")->Set(txt);}
	std::string getText(){return component->GetVar("text")->GetString();}
//	void setFont(std::string fnt){load_font(fnt);}

	eFont getFont(){
		return (eFont)component->GetVar("font")->GetUINT32();
	}
	void setFont(eFont f){
		component->GetVar("font")->Set(uint32(f));
	}
	float getScale(){
		assert(entity);
		float sx = entity->GetVar("scale2d")->GetVector2().x;
		return sx;
	}
	void setScale(float s){
		assert(entity);
		entity->GetVar("scale2d")->Set(s, s);
	}
private:
	TextRenderComponent* component;
};

// TODO: Not very good from the point of view of code reuse!
class TextBoxItem: virtual public ScreenItem
{
public:
	TextBoxItem(std::string txt, eAlignment align, eFont font=FONT_SMALL);
	~TextBoxItem();

	void setText(std::string txt){component->GetVar("text")->Set(txt);}
	std::string getText(){return component->GetVar("text")->GetString();}

	void setFirstLineDecrement(float val){
		// TODO It can be less than 0 btw!
		component->GetVar("firstLineDecrement")->Set(val);
	}
	float getFirstLineDecrement(){
		return component->GetVar("firstLineDecrement")->GetFloat();
	}
	const StairsProfile& getLeftObstacles() const{
		return component->getLeftObstacles();
	}
	const StairsProfile& getRightObstacles() const{
		return component->getRightObstacles();
	}
	void setLeftObstacles(const StairsProfile& p){
		component->setLeftObstacles(p);
	}
	void setRightObstacles(const StairsProfile& p){
		component->setRightObstacles(p);
	}
	eFont getFont(){
		return (eFont)component->GetVar("font")->GetUINT32();
	}
	void setFont(eFont f){
		component->GetVar("font")->Set(uint32(f));
	}
	float getScale(){
		float sx = component->GetVar("fontScale")->GetFloat();
		return sx;
	}
	void setScale(float s){
		component->GetVar("fontScale")->Set(s);
	}
	float getLastLineEndX(){
		return component->GetVar("lastLineEndX")->GetFloat();
	}
	float getLastLineEndY(){
		return component->GetVar("lastLineEndY")->GetFloat();
	}
	float getOneLineWidth() const{
		return component->GetVar("oneLineWidth")->GetFloat();
	}
private:
	TextBoxRenderComponent* component;
};

class LuaTextItem: public TextItem, public LuaScreenItem{
public:
	LuaTextItem(std::string);
	LuaTextItem(std::string text, eFont font);
	static void luabind(lua_State* L);

private:
	LuaTextItem(const LuaTextItem&):TextItem(""){assert(false);}
	LuaTextItem& operator=(const LuaTextItem&){assert(false);}
};

class LuaStairsProfile;

class LuaTextBoxItem: public TextBoxItem, public LuaScreenItem{
public:
	LuaTextBoxItem(std::string txt);
	LuaTextBoxItem(std::string txt, eFont font);
	static void luabind(lua_State* L);

	const LuaStairsProfile getLeftObstacles() const;
	const LuaStairsProfile getRightObstacles() const;
	void setLeftObstacles(const LuaStairsProfile& p);
	void setRightObstacles(const LuaStairsProfile& p);
private:
	LuaTextBoxItem(const LuaTextItem&):TextBoxItem("", ALIGNMENT_UPPER_LEFT){assert(false);}
	LuaTextBoxItem& operator=(const LuaTextBoxItem&){assert(false);}
};