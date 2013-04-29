#include "LuaTextWidget.h"
#include <luabind/operator.hpp>

void LuaTextWidget::luabind(lua_State* L){
	luabind::module(L) [
		luabind::class_<LuaTextWidget, LuaWidget>("TextWidget")
			.def(luabind::constructor<std::string, float, float, luabind::object>())
			.property("text", &LuaTextWidget::getText, &LuaTextWidget::setText)
	];
}