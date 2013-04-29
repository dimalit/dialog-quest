#include "Text.h"

Text::Text(std::string txt, std::string fnt)
{
	this->text = txt;
	this->fnt = 0;
	load_font(fnt);
}

Text::~Text()
{
	if(fnt)delete fnt;
}

void Text::Render(float x, float y, float rot){
	// x, y must be upper left corner
	fnt->Render(x + getWidth() / 2, y, HGETEXT_CENTER, text.c_str());
}

void Text::load_font(std::string path){
	if(fnt)delete fnt;
	this->fnt = new hgeFont(path.c_str());
}

LuaText::LuaText(std::string text, std::string font):Text(text, font){}

LuaText::LuaText(std::string text):Text(text){}

void LuaText::luabind(lua_State* L){
	luabind::module(L) [
		luabind::class_<LuaText, ScreenResource>("Text")
			.def(luabind::constructor<std::string, std::string>())
			.def(luabind::constructor<std::string>())
			.property("text", &LuaText::getText, &LuaText::setText)
	];	
}