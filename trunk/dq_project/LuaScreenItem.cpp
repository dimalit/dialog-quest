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

//LuaCompositeItem* LuaCompositeItem::add(luabind::object child){
//	assert(children_lua[child] == false);
//	children_lua[child] = true;
//
//	while(luabind::type(child) != LUA_TUSERDATA && luabind::type(child) != LUA_TNIL){
//		child = luabind::getmetatable(child)["__luabinded_base"];
//	}
//
//	LuaScreenItem* luabinded_base = luabind::object_cast_nothrow<LuaScreenItem*>(child).get_value_or(NULL);
//	if(luabinded_base == NULL)
//		luaL_error(child.interpreter(), "Can't convert value to ScreenItem!");
//	else
//		CompositeItem::add(luabinded_base);
//	return this;
//}
//
//LuaCompositeItem* LuaCompositeItem::remove(luabind::object child){
//	assert(children_lua[child] == true);
//	children_lua[child] = false;
//
//	while(luabind::type(child) != LUA_TUSERDATA && luabind::type(child) != LUA_TNIL){
//		child = luabind::getmetatable(child)["__luabinded_base"];
//	}
//
//	LuaScreenItem* luabinded_base = luabind::object_cast_nothrow<LuaScreenItem*>(child).get_value_or(NULL);
//	if(luabinded_base == NULL)
//		luaL_error(child.interpreter(), "Can't convert value to ScreenItem!");
//	else
//		CompositeItem::remove(luabinded_base);
//	return this;
//}

void LuaCompositeItem::requestLayOut(ScreenItem* child){
	// Prevent deep recursion
	if(request_layout_is_running)
		return;
	request_layout_is_running = true;
	if(onRequestLayOut_cb)
		luabind::call_function<void>(onRequestLayOut_cb, this, child);
	request_layout_is_running = false;
}
void LuaCompositeItem::requestLayOut(luabind::object child){
	// Prevent deep recursion
	if(request_layout_is_running)
		return;
	request_layout_is_running = true;
	if(onRequestLayOut_cb)
		luabind::call_function<void>(onRequestLayOut_cb, this, child);
	request_layout_is_running = false;
}

///////////////////// WRAPPERS //////////////////////////
// used to derive from ScreenItem in Lua
// FIXME: For now we ignore the possibility to call overridden in Lua functions from C++!

//class LuaScreenItemWrapper: public LuaScreenItem, public luabind::wrap_base{
//public:
//	LuaScreenItemWrapper()
//		:LuaScreenItem()
//	{}
//};
//
//class LuaCompositeItemWrapper: public LuaCompositeItem, public luabind::wrap_base{
//public:
//	LuaCompositeItemWrapper()
//		:LuaCompositeItem()
//	{}
//};
//
//class LuaSimpleItemWrapper: public LuaSimpleItem, public luabind::wrap_base{
//public:
//	LuaSimpleItemWrapper()
//		:LuaSimpleItem()
//	{}
//};
//////////////////// END WRAPPERS //////////////////////

void LuaScreenItem::luabind(lua_State* L){
	luabind::module(L) [
		luabind::class_<LuaScreenItem/*, LuaScreenItemWrapper*/>("ScreenItem")
		.def(luabind::constructor<>())
//TODO: May be delete it completely?
//		.property("parent", &LuaScreenItem::getParent)
		.property("gx", &LuaScreenItem::getAbsoluteX, &LuaScreenItem::setAbsoluteX)
		.property("gy", &LuaScreenItem::getAbsoluteY, &LuaScreenItem::setAbsoluteY)
		.property("visible", &LuaScreenItem::getVisible, &LuaScreenItem::setVisible)
		.property("really_visible", &LuaScreenItem::getReallyVisible)
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
		.property("rel_hpx", &LuaScreenItem::getHotSpotRelativeX, &LuaScreenItem::setHotSpotRelativeX)
		.property("rel_hpy", &LuaScreenItem::getHotSpotRelativeY, &LuaScreenItem::setHotSpotRelativeY)

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
		.property("debugDrawBox", &LuaScreenItem::getDebugDrawBox, &LuaScreenItem::setDebugDrawBox)

		.def(luabind::self == luabind::other<LuaScreenItem&>())				// remove operator ==
	];
}

void LuaCompositeItem::luabind(lua_State* L){
	luabind::module(L) [
		luabind::class_< LuaCompositeItem, LuaScreenItem/*, LuaCompositeItemWrapper*/>("CompositeItem")
		.def(luabind::constructor<>())
		.def("add", (LuaCompositeItem* (LuaCompositeItem::*)(LuaScreenItem*))&LuaCompositeItem::add)
//		.def("add", (LuaCompositeItem* (LuaCompositeItem::*)(luabind::object))&LuaCompositeItem::add)
		.def("remove", (LuaCompositeItem* (LuaCompositeItem::*)(LuaScreenItem*))&LuaCompositeItem::remove)
//		.def("remove", (LuaCompositeItem* (LuaCompositeItem::*)(luabind::object))&LuaCompositeItem::remove)
		.def_readwrite("onRequestLayOut", &LuaCompositeItem::onRequestLayOut_cb)
		.def("requestLayOut", (void (LuaCompositeItem::*)(ScreenItem*))&LuaCompositeItem::requestLayOut)
		.def("requestLayOut", (void (LuaCompositeItem::*)(luabind::object))&LuaCompositeItem::requestLayOut)

		.def(luabind::self == luabind::other<LuaScreenItem&>())				// remove operator ==
	];

	luabind::globals(L)["root"] = dynamic_cast<LuaCompositeItem*>(root_item());
}

void LuaSimpleItem::luabind(lua_State* L){

	luabind::module(L) [
	luabind::class_<LuaSimpleItem, LuaScreenItem/*, LuaSimpleItemWrapper*/>("SimpleItem")
		.def(luabind::constructor<>())

		// TODO How to adopt back to lua when assigning NULL?
		.property("view", &LuaSimpleItem::getView, &LuaSimpleItem::setView, luabind::detail::null_type(), luabind::adopt(_2))
		.def(luabind::self == luabind::other<LuaScreenItem&>())				// remove operator ==

	];
}
