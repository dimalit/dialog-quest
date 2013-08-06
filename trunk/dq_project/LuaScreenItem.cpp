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
		Entity* e = new Entity("root");
		AddFocusIfNeeded(e);
		root->acquireEntity(e);
	}
	return root;
}

LuaScreenItem::LuaScreenItem(LuaCompositeItem* parent, int x, int y):ScreenItem(parent,x,y){
}

LuaCompositeItem* LuaScreenItem::getParent(){
	LuaCompositeItem* ret = dynamic_cast<LuaCompositeItem*>(ScreenItem::getParent());
	assert(ret || !ScreenItem::getParent());		// should be convertible! (or null)
	return ret;
}

void LuaScreenItem::setParent(LuaCompositeItem* p){
	ScreenItem::setParent(p);
}

void LuaScreenItem::luabind(lua_State* L){
	luabind::module(L) [
		luabind::class_<LuaScreenItem>("ScreenItem")
		.def(luabind::constructor<LuaCompositeItem*, int, int>())
		.def(luabind::constructor<LuaCompositeItem*>())
		.def(luabind::constructor<>())
		.property("parent", &LuaScreenItem::getParent, &LuaScreenItem::setParent)
		.property("x", &LuaScreenItem::getX, &LuaScreenItem::setX)
		.property("y", &LuaScreenItem::getY, &LuaScreenItem::setY)
		.def("move", &LuaScreenItem::move)
		.property("quad", &LuaScreenItem::getQuad)

		.property("rotation", &LuaScreenItem::getRotation, &LuaScreenItem::setRotation)

		.property("width", &LuaCompositeItem::getWidth, &LuaCompositeItem::setWidth)
		.property("height", &LuaCompositeItem::getHeight, &LuaCompositeItem::setHeight)
		.property("top", &LuaScreenItem::getTop)
		.property("bottom", &LuaScreenItem::getBottom)
		.property("left", &LuaScreenItem::getLeft)
		.property("right", &LuaScreenItem::getRight)
		.property("hpx", &LuaScreenItem::getHotSpotX)
		.property("hpy", &LuaScreenItem::getHotSpotY)
		.property("hpx_relative", &LuaScreenItem::getHotSpotRelativeX, &LuaScreenItem::setHotSpotRelativeX)
		.property("hpy_relative", &LuaScreenItem::getHotSpotRelativeY, &LuaScreenItem::setHotSpotRelativeY)

		.def(luabind::self == luabind::other<LuaScreenItem&>())				// remove operator ==
	];
}

void LuaCompositeItem::luabind(lua_State* L){
	luabind::module(L) [
		luabind::class_< LuaCompositeItem, LuaScreenItem >("CompositeItem")
		.def(luabind::constructor<LuaCompositeItem*,int,int>())
		.def(luabind::constructor<LuaCompositeItem*>())
		.def(luabind::constructor<>())

		.def(luabind::self == luabind::other<LuaCompositeItem&>())				// remove operator ==
	];

	luabind::globals(L)["root"] = dynamic_cast<LuaCompositeItem*>(root_item());
}

void LuaSimpleItem::luabind(lua_State* L){

	luabind::module(L) [
	luabind::class_<LuaSimpleItem, LuaScreenItem>("SimpleItem")
		.def(luabind::constructor<LuaCompositeItem*, int, int>())
		.def(luabind::constructor<LuaCompositeItem*>())

		.def_readwrite("onDbClick", &LuaSimpleItem::onDbClick_cb)
		.def_readwrite("onDrag", &LuaSimpleItem::onDrag_cb)
		.def_readwrite("onDragStart", &LuaSimpleItem::onDragStart_cb)
		.def_readwrite("onDragEnd", &LuaSimpleItem::onDragEnd_cb)
		.def_readwrite("onChar", &LuaSimpleItem::onChar_cb)
		.def("takeCharFocus", &LuaSimpleItem::takeCharFocus)
		.def("giveCharFocus", &LuaSimpleItem::giveCharFocus)
		.def_readwrite("onFocusLose", &LuaSimpleItem::onFocusLose_cb)

		// TODO How to adopt back to lua when assigning NULL?
		.property("view", &LuaSimpleItem::getView, &LuaSimpleItem::setView, luabind::detail::null_type(), luabind::adopt(_2))
//		.def_readwrite("visible", &LuaSimpleItem::visible)
		.def("destroy", &LuaSimpleItem::destroy)
 
		.def(luabind::self == luabind::other<LuaSimpleItem&>())				// remove operator ==

	];
}
