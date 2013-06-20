#include "PlatformPrecomp.h"
#include "LuaScreenItem.h"
#include "Image.h"
#include <luabind/operator.hpp>
#include <luabind/object.hpp>
#include <luabind/adopt_policy.hpp>
#include <luabind/dependency_policy.hpp>

void LuaScreenItem::luabind(lua_State* L){

	//luabind::module(L) [
	//	luabind::class_<LuaScreenItem>("ScreenItem")
	//];

	luabind::module(L) [
	luabind::class_<LuaScreenItem>("ScreenItem")
		.def(luabind::constructor<float, float>())
		.def(luabind::constructor<>())

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

		// TODO How to adopt back to lua when assigning NULL?
		.property("view", &LuaScreenItem::getView, &LuaScreenItem::setView, luabind::detail::null_type(), luabind::adopt(_2))
//		.def_readwrite("visible", &LuaScreenItem::visible)
		.def("destroy", &LuaScreenItem::destroy)
 
		.def(luabind::self == luabind::other<LuaScreenItem&>())				// remove operator ==

	];
}
