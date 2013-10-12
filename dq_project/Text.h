#pragma once

#include "Entity/TextRenderComponent.h"
#include "Entity/TextBoxRenderComponent.h"
#include <luabind/luabind.hpp>

class Text: public TextRenderComponent
{
public:
	Text(std::string, eFont font=FONT_SMALL);
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
	eFont getFont(){
		return (eFont)GetVar("font")->GetUINT32();
	}
	void setFont(eFont f){
		GetVar("font")->Set(uint32(f));
	}
	float getScale(){
		float sx = GetParent()->GetVar("scale2d")->GetVector2().x;
		return sx;
	}
	void setScale(float s){
		GetParent()->GetVar("scale2d")->Set(s, s);
	}
};

// TODO: Not very good from the point of view of code reuse!
class TextBox: public TextBoxRenderComponent
{
public:
	TextBox(std::string txt, float width, eAlignment align, eFont font=FONT_SMALL);
	~TextBox();

	void OnAdd(Entity* e);

	void setText(std::string txt){GetVar("text")->Set(txt);}
	std::string getText(){return GetVar("text")->GetString();}

	float getWidth() const{
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
	void setFirstLineDecrement(float val){
		// TODO It can be less than 0 btw!
		GetVar("firstLineDecrement")->Set(val);
	}
	float getFirstLineDecrement(){
		return GetVar("firstLineDecrement")->GetFloat();
	}
	const StairsProfile& getLeftObstacles() const{
		return left_obstacles;
	}
	const StairsProfile& getRightObstacles() const{
		return right_obstacles;
	}
	void setLeftObstacles(const StairsProfile& p){
		left_obstacles = p;
		GetVar("text")->GetSigOnChanged()->operator()(NULL);
	}
	void setRightObstacles(const StairsProfile& p){
		right_obstacles = p;
		GetVar("text")->GetSigOnChanged()->operator()(NULL);
	}
	eFont getFont(){
		return (eFont)GetVar("font")->GetUINT32();
	}
	void setFont(eFont f){
		GetVar("font")->Set(uint32(f));
	}
	float getScale(){
		float sx = GetVar("fontScale")->GetFloat();
		return sx;
	}
	void setScale(float s){
		GetVar("fontScale")->Set(s);
	}
	float getLastLineEndX(){
		return GetVar("lastLineEndX")->GetFloat();
	}
	float getLastLineEndY(){
		return GetVar("lastLineEndY")->GetFloat();
	}
private:
	float width;// used when attached
};

class LuaText: public Text{
public:
	LuaText(std::string);
	LuaText(std::string text, eFont font);
	static void luabind(lua_State* L);

private:
	LuaText(const LuaText&):Text(""){assert(false);}
	LuaText& operator=(const LuaText&){assert(false);}
};

class LuaStairsProfile;

class LuaTextBox: public TextBox{
public:
	LuaTextBox(std::string txt, float width, eAlignment align);
	LuaTextBox(std::string txt, float width, eAlignment align, eFont font);
	static void luabind(lua_State* L);

	const LuaStairsProfile getLeftObstacles() const;
	const LuaStairsProfile getRightObstacles() const;
	void setLeftObstacles(const LuaStairsProfile& p);
	void setRightObstacles(const LuaStairsProfile& p);
private:
	LuaTextBox(const LuaText&):TextBox("", 0, ALIGNMENT_UPPER_LEFT){assert(false);}
	LuaTextBox& operator=(const LuaText&){assert(false);}
};