#include "LuaWidget.h"
#include <luabind/operator.hpp>
#include "ImageWidget.h"
#include "TextWidget.h"

lua_State* LuaWidget::L = 0;
//lua_State* LuaWidget<TextWidget>::L = 0;

// function for operator==
bool return_false(const LuaWidget& w1, const LuaWidget& w2){
	return false;
}

LuaWidget::LuaWidget():Widget(0,0){

}

void LuaWidget::luabind(lua_State* L){
	LuaWidget::L = L;

	luabind::module(L) [
	luabind::class_<LuaWidget>("Widget")
	.property("x", &LuaWidget::getX, &LuaWidget::setX)
	.property("y", &LuaWidget::getY, &LuaWidget::setY)
	.def("move", &LuaWidget::move)
//	.property("rect", &LuaWidget::getRect)
	.def_readonly("left", &LuaWidget::x1)
	.def_readonly("top", &LuaWidget::y1)
	.def_readonly("right", &LuaWidget::x2)
	.def_readonly("bottom", &LuaWidget::y2)
	.def("rotate", &LuaWidget::rotate)
	.property("rotation", &LuaWidget::getRotation, &LuaWidget::setRotation)
	.property("visible", &LuaWidget::getVisible, &LuaWidget::setVisible)
	.property("enabled", &LuaWidget::getEnabled, &LuaWidget::setEnabled)
	.property("checked", &LuaWidget::getChecked, &LuaWidget::setChecked)
	.property("over_response", &LuaWidget::getOverResponse, &LuaWidget::setOverResponse)
	.property("drag_response", &LuaWidget::getDragResponse, &LuaWidget::setDragResponse)
	.property("click_response", &LuaWidget::getClickResponse, &LuaWidget::setClickResponse)
	.def_readwrite("onDbClick", &LuaWidget::onDbClick_cb)
	.def_readwrite("onDrag", &LuaWidget::onDrag_cb)
	.def_readwrite("onDragStart", &LuaWidget::onDragStart_cb)
	.def_readwrite("onDragEnd", &LuaWidget::onDragEnd_cb)
	.def_readwrite("onChar", &LuaWidget::onChar_cb)
	.def("takeCharFocus", &LuaWidget::takeCharFocus)
	.def("giveCharFocus", &LuaWidget::giveCharFocus)
	.def_readwrite("onFocusLose", &LuaWidget::onFocusLose_cb)
	.def(luabind::self == luabind::other<LuaWidget&>())				// remove operator ==
	];
}