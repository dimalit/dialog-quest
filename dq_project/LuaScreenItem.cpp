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

LuaSimpleItem::LuaSimpleItem(CompositeItem* parent, float x, float y): SimpleItem(parent, x, y), LuaScreenItem(parent), ScreenItem(parent){
//	this->setParent(root_item());
}
LuaSimpleItem::LuaSimpleItem(CompositeItem* parent): SimpleItem(parent), LuaScreenItem(parent), ScreenItem(parent){
//	this->setParent(root_item());
}

void LuaScreenItem::luabind(lua_State* L){
	luabind::module(L) [
	luabind::class_<LuaScreenItem>("ScreenItem")
	.def(luabind::constructor<CompositeItem*>())
	.def(luabind::constructor<>())
	.property("parent", &LuaScreenItem::getParent, &LuaScreenItem::setParent)
	];
}

void LuaCompositeItem::luabind(lua_State* L){
	luabind::module(L) [
	luabind::class_<LuaCompositeItem, LuaScreenItem>("CompositeItem")
	.def(luabind::constructor<CompositeItem*>())
	.def(luabind::constructor<>())
	];

	luabind::globals(L)["root"] = root_item();
}

void LuaSimpleItem::luabind(lua_State* L){

	luabind::module(L) [
	luabind::class_<LuaSimpleItem, LuaScreenItem>("SimpleItem")
		.def(luabind::constructor<CompositeItem*, float, float>())
		.def(luabind::constructor<CompositeItem*>())

		.property("x", &LuaSimpleItem::getX, &LuaSimpleItem::setX)
		.property("y", &LuaSimpleItem::getY, &LuaSimpleItem::setY)
		.def("move", &LuaSimpleItem::move)
		.property("quad", &LuaSimpleItem::getQuad)

		.property("rotation", &LuaSimpleItem::getRotation, &LuaSimpleItem::setRotation)

		.property("width", &LuaSimpleItem::getWidth)
		.property("height", &LuaSimpleItem::getHeight)
		.property("top", &LuaSimpleItem::getTop)
		.property("bottom", &LuaSimpleItem::getBottom)
		.property("left", &LuaSimpleItem::getLeft)
		.property("right", &LuaSimpleItem::getRight)
		.property("hpx", &LuaSimpleItem::getHotSpotX, &LuaSimpleItem::setHotSpotX)
		.property("hpy", &LuaSimpleItem::getHotSpotY, &LuaSimpleItem::setHotSpotY)

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
