#include "PlatformPrecomp.h"
#include "Text.h"

Text::Text(std::string txt)
{
	GetVar("text")->Set(txt);
}

Text::~Text()
{
}

void Text::OnAdd(Entity* e){
	TextRenderComponent::OnAdd(e);
	// HACK: components need all parameters AFTER attach...
	std::string t = GetVar("text")->GetString();
	GetVar("text")->Set(t);
}

LuaText::LuaText(std::string text):Text(text){}

void LuaText::luabind(lua_State* L){
	luabind::module(L) [
		luabind::class_<LuaText, EntityComponent>("Text")
//			.def(luabind::constructor<std::string, std::string>())
			.def(luabind::constructor<std::string>())
			.property("width", &LuaText::getWidth)
			.property("height", &LuaText::getHeight)
			.property("text", &LuaText::getText, &LuaText::setText)
	];	
}