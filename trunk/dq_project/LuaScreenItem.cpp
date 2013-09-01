#include "PlatformPrecomp.h"

#include "LuaScreenItem.h"
#include "lua_lib.h"
#include "Image.h"
#include <luabind/operator.hpp>
#include <luabind/object.hpp>
#include <luabind/adopt_policy.hpp>
#include <luabind/dependency_policy.hpp>

CompositeItem* root_item(){
	static LuaCompositeItem* root = 0;
	if(!root){
	root = new LuaCompositeItem();
	root->setHotSpotRelativeX(0.0f);
	root->setHotSpotRelativeY(0.0f);
		Entity* e = new Entity("root");
		AddFocusIfNeeded(e);
		root->acquireEntity(e);
	}
	return root;
}

LuaScreenItem::LuaScreenItem():ScreenItem(){
}

LuaCompositeItem* LuaScreenItem::getParent(){
	LuaCompositeItem* ret = dynamic_cast<LuaCompositeItem*>(ScreenItem::getParent());
	assert(ret || !ScreenItem::getParent());		// should be convertible! (or null)
	return ret;
}

void LuaScreenItem::luabind(lua_State* L){
	luabind::module(L) [
		luabind::class_<LuaScreenItem>("ScreenItem")
		.def(luabind::constructor<>())
		.property("parent", &LuaScreenItem::getParent)
		.property("gx", &LuaScreenItem::getAbsoluteX)
		.property("gy", &LuaScreenItem::getAbsoluteY)
		.property("visible", &LuaScreenItem::getVisible, &LuaScreenItem::setVisible)
		.def("move", &LuaScreenItem::move)
		.property("quad", &LuaScreenItem::getQuad)

		.property("top", &LuaScreenItem::getTop)
		.property("bottom", &LuaScreenItem::getBottom)
		.property("left", &LuaScreenItem::getLeft)
		.property("right", &LuaScreenItem::getRight)
		.property("hpx", &LuaScreenItem::getHotSpotX)
		.property("hpy", &LuaScreenItem::getHotSpotY)

		// set/get properties
		.property("x", &LuaScreenItem::getX, &LuaScreenItem::setX)
		.property("y", &LuaScreenItem::getY, &LuaScreenItem::setY)
		.property("rotation", &LuaScreenItem::getRotation, &LuaScreenItem::setRotation)
		.property("width", &LuaScreenItem::getWidth, &LuaScreenItem::setWidth)
		.property("height", &LuaScreenItem::getHeight, &LuaScreenItem::setHeight)
		.property("hpx_relative", &LuaScreenItem::getHotSpotRelativeX, &LuaScreenItem::setHotSpotRelativeX)
		.property("hpy_relative", &LuaScreenItem::getHotSpotRelativeY, &LuaScreenItem::setHotSpotRelativeY)

		.def_readwrite("onMove", &LuaScreenItem::onMove_cb)
		.def_readwrite("onDbClick", &LuaScreenItem::onDbClick_cb)
		.def_readwrite("onDrag", &LuaScreenItem::onDrag_cb)
		.def_readwrite("onDragStart", &LuaScreenItem::onDragStart_cb)
		.def_readwrite("onDragEnd", &LuaScreenItem::onDragEnd_cb)
		.def_readwrite("onChar", &LuaScreenItem::onChar_cb)
		.def("takeCharFocus", &LuaScreenItem::takeCharFocus)
		.def("giveCharFocus", &LuaScreenItem::giveCharFocus)
		.def_readwrite("onFocusLose", &LuaScreenItem::onFocusLose_cb)

		.def("destroy", &LuaScreenItem::destroy)

		.def(luabind::self == luabind::other<LuaScreenItem&>())				// remove operator ==
	];
}

void LuaCompositeItem::luabind(lua_State* L){
	luabind::module(L) [
		luabind::class_< LuaCompositeItem, LuaScreenItem >("CompositeItem")
		.def(luabind::constructor<>())
		.def("add", &LuaCompositeItem::add)
		.def("remove", &LuaCompositeItem::remove)
		.def_readwrite("onRequestLayOut", &LuaCompositeItem::onRequestLayOut_cb)
		.def("requestLayOut", (void (LuaCompositeItem::*)(ScreenItem*))&LuaCompositeItem::requestLayOut)
		.def("requestLayOut", (void (LuaCompositeItem::*)(luabind::object))&LuaCompositeItem::requestLayOut)

		.def(luabind::self == luabind::other<LuaCompositeItem&>())				// remove operator ==
	];

	luabind::globals(L)["root"] = dynamic_cast<LuaCompositeItem*>(root_item());
}

void LuaSimpleItem::luabind(lua_State* L){

	luabind::module(L) [
	luabind::class_<LuaSimpleItem, LuaScreenItem>("SimpleItem")
		.def(luabind::constructor<>())

		// TODO How to adopt back to lua when assigning NULL?
		.property("view", &LuaSimpleItem::getView, &LuaSimpleItem::setView, luabind::detail::null_type(), luabind::adopt(_2))
		.def(luabind::self == luabind::other<LuaSimpleItem&>())				// remove operator ==

	];
}
