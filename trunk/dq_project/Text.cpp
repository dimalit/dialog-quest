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

// TODO: think about enums in Lua and here (don't want to use Proton's enums!)
TextBox::TextBox(std::string txt, int width, int height, eAlignment align)
{
	GetVar("text")->Set(txt);
	GetVar("textAlignment")->Set((uint32)align);
	size = CL_Vec2f(width, height);
}

TextBox::~TextBox()
{
}

void TextBox::OnAdd(Entity* e){
	TextBoxRenderComponent::OnAdd(e);
	// TODO MUST first set size then text. Very bad!
	e->GetVar("size2d")->Set(size);
	std::string t = GetVar("text")->GetString();
	GetVar("text")->Set(t);
}

LuaText::LuaText(std::string text):Text(text){}

void LuaText::luabind(lua_State* L){
	luabind::module(L) [
		luabind::class_<LuaText, EntityComponent>("Text")
			.def(luabind::constructor<std::string>())
			.property("width", &LuaText::getWidth)
			.property("height", &LuaText::getHeight)
			.property("text", &LuaText::getText, &LuaText::setText)
	];	
}

LuaTextBox::LuaTextBox(std::string txt, int width, int height, eAlignment align)
	:TextBox(txt, width, height, align){}

void LuaTextBox::luabind(lua_State* L){
	luabind::module(L) [
		luabind::class_<LuaTextBox, EntityComponent>("TextBox")
			// TODO how to bind alignment constants to Lua?
			// TODO need function to query real text height
			.def(luabind::constructor<std::string, int, int, eAlignment>())
			.property("width", &LuaTextBox::getWidth, &LuaTextBox::setWidth)
			.property("height", &LuaTextBox::getHeight, &LuaTextBox::setHeight)
			.property("text", &LuaTextBox::getText, &LuaTextBox::setText)
	];	
}