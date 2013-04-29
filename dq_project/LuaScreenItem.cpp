#include "LuaScreenItem.h"
#include <luabind/operator.hpp>
#include <luabind/object.hpp>

void LuaScreenItem::luabind(lua_State* L){
	luabind::module(L) [
	luabind::class_<LuaScreenItem>("ScreenItem")
		.def(luabind::constructor<>())
		.def(luabind::constructor<float, float>())

		.property("x", &LuaScreenItem::getX, &LuaScreenItem::setX)
		.property("y", &LuaScreenItem::getY, &LuaScreenItem::setY)
		.def("move", &LuaScreenItem::move)
		.property("quad", &LuaScreenItem::getQuad)

		.property("rotation", &LuaScreenItem::getRotation, &LuaScreenItem::setRotation)

		.property("width", &LuaScreenItem::getWidth)
		.property("height", &LuaScreenItem::getHeight)
		.property("top", &LuaScreenItem::getTop)
		.property("bottom", &LuaScreenItem::getBottom)
		.property("left", &LuaScreenItem::getLeft)
		.property("right", &LuaScreenItem::getRight)
		.property("hpx", &LuaScreenItem::getHotSpotX, &LuaScreenItem::setHotSpotX)
		.property("hpy", &LuaScreenItem::getHotSpotY, &LuaScreenItem::setHotSpotY)

		.def_readwrite("onDbClick", &LuaScreenItem::onDbClick_cb)
		.def_readwrite("onDrag", &LuaScreenItem::onDrag_cb)
		.def_readwrite("onDragStart", &LuaScreenItem::onDragStart_cb)
		.def_readwrite("onDragEnd", &LuaScreenItem::onDragEnd_cb)
		.def_readwrite("onChar", &LuaScreenItem::onChar_cb)
		.def("takeCharFocus", &LuaScreenItem::takeCharFocus)
		.def("giveCharFocus", &LuaScreenItem::giveCharFocus)
		.def_readwrite("onFocusLose", &LuaScreenItem::onFocusLose_cb)
		
		.property("view", &LuaScreenItem::getView, &LuaScreenItem::setView)
		.def_readwrite("visible", &LuaScreenItem::visible)
		.def("destroy", &LuaScreenItem::destroy)
 
		.def(luabind::self == luabind::other<LuaScreenItem&>())				// remove operator ==

	];
}