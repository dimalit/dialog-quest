#include "PlatformPrecomp.h"

#include "TextInput.h"
#include <Entity/TouchHandlerComponent.h>

TextInput::TextInput(int width, eFont font){
	this->width = width;
	setFont(font);
	GetVar("filtering")->Set(uint32(FILTERING_LOOSE));
	GetVar("inputLengthMax")->Set(uint32(255));			// infinite length
	GetVar("truncateTextIfNeeded")->Set(uint32(1));		// BUT truncate to the length!
}
TextInput::~TextInput(){
}

void TextInput::OnAdd(Entity* e){
	InputTextRenderComponent::OnAdd(e);
	e->AddComponent(new TouchHandlerComponent());
	float h = GetBaseApp()->GetFont(getFont())->GetLineHeight(1.0f);
	e->GetVar("size2d")->Set(width, h+6);
}

LuaTextInput::LuaTextInput(float w):TextInput(w){}
LuaTextInput::LuaTextInput(float w, eFont font):TextInput(w, font){}

void LuaTextInput::luabind(lua_State* L){
	luabind::module(L) [
		luabind::class_<LuaTextInput, EntityComponent>("TextInput")
			.def(luabind::constructor<float>())
			.def(luabind::constructor<float, eFont>())
			.property("width", &LuaTextInput::getWidth, &LuaTextInput::setWidth)
			.property("height", &LuaTextInput::getHeight)
			.property("text", &LuaTextInput::getText, &LuaTextInput::setText)
			.property("font", &LuaTextInput::getFont, &LuaTextInput::setFont)
	];	
}