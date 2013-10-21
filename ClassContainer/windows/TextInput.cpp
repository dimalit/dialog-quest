#include "PlatformPrecomp.h"

#include "TextInput.h"

#include <Entity/TouchHandlerComponent.h>

TextInputItem::TextInputItem(int width, eFont font){


	component = new InputTextRenderComponent();
	entity->AddComponent(component);
	setFont(font);
	component->GetVar("filtering")->Set(uint32(InputTextRenderComponent::FILTERING_LOOSE));
	component->GetVar("inputLengthMax")->Set(uint32(255));			// infinite length
	component->GetVar("truncateTextIfNeeded")->Set(uint32(1));		// BUT truncate to the length!

	entity->AddComponent(new TouchHandlerComponent());
	float h = GetBaseApp()->GetFont(getFont())->GetLineHeight(1.0f);
	entity->GetVar("size2d")->Set(width, h+6);

}
TextInputItem::~TextInputItem(){
}

LuaTextInputItem::LuaTextInputItem(float w):TextInputItem(w){}
LuaTextInputItem::LuaTextInputItem(float w, eFont font):TextInputItem(w, font){}

void LuaTextInputItem::luabind(lua_State* L){
	luabind::module(L) [
		luabind::class_<LuaTextInputItem, LuaScreenItem>("TextInputItem")
			.def(luabind::constructor<float>())
			.def(luabind::constructor<float, eFont>())
			.property("text", &LuaTextInputItem::getText, &LuaTextInputItem::setText)
			.property("font", &LuaTextInputItem::getFont, &LuaTextInputItem::setFont)
	];	
}