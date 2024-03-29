#include "PlatformPrecomp.h"

#include "LuaScreenItem.h"
#include "lua_lib.h"
#include "Image.h"
#include <luabind/operator.hpp>
#include <luabind/object.hpp>
#include <luabind/adopt_policy.hpp>
#include <luabind/dependency_policy.hpp>
#include <luabind/out_value_policy.hpp>

int rec_depth = 0;

CompositeItem* root_item(){
	static LuaCompositeItem* root = 0;
	if(!root){
		root = new LuaCompositeItem();
		root->setHotSpotRelativeX(0.0f);
		root->setHotSpotRelativeY(0.0f);
		root->setX(0.0f); root->setY(0.0f);
		root->setWidth(GetScreenSizeX());
		root->setHeight(GetScreenSizeY());

		Entity* e = new Entity("root");
		AddFocusIfNeeded(e);
		root->acquireEntity(e);

		// do layout from root before render
		//GetBaseApp()->m_sig_render.connect(
		//	boost::bind(std::mem_fun(&CompositeItem::_specialEntryForRenderSignal), root_item()),			
		//	boost::signals::at_front);
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

void LuaCompositeItem::adjustSize(bool& rigid_width, bool& rigid_height){
	// if no callback - we are rigid
	if(!onRequestSize_cb){
		rigid_width = true;
		rigid_height = true;
		return;
	}

	// if there are no changes
	if(!need_lay_out && !need_lay_out_children &&
		this->getWidth()==last_requested_width && this->getHeight()==last_requested_height)
	{
		rigid_width = last_requested_rigid_width;
		rigid_height = last_requested_rigid_height;
		return;
	}// if

	// call and cache
//	cout << "C++ calls" << endl;
	// TODO: for some reason I couldn't make this work and used raw lua_call :(
//	luabind::call_function< void >(L, onRequestSize_cb, this);

	onRequestSize_cb.push(L);
	lua_pushlightuserdata(L, (void*)this);

	lua_call(L, 1, 2);
	rigid_width = lua_toboolean(L, -2);
	rigid_height = lua_toboolean(L, -1);
		lua_pop(L, 2);

	last_requested_rigid_width = rigid_width;
	last_requested_rigid_height = rigid_height;
	last_requested_width = getWidth();
	last_requested_height = getHeight();

//	cout << "C++ returns " << rigid_width << " " << rigid_height << endl;
}

void LuaCompositeItem::adjustLayout(){
	assert(global_layout_mode);

	// 1 if adjust ONLY children OR there is even no layout callback
	if(need_lay_out_children && (!need_lay_out || !onRequestLayOut_cb)){
		need_lay_out_children = false;
		for(std::set<ScreenItem*>::iterator i = children.begin(); i != children.end(); ++i){
			CompositeItem* c = dynamic_cast<CompositeItem*>(*i);
			if(c){
				c->adjustLayout();
				assert(!c->need_lay_out);
			}
		}// for
		assert(!need_lay_out_children);
	}// if

	// 2 adjust myself, maybe after children
	if(need_lay_out){
		if(onRequestLayOut_cb)
			luabind::call_function<void>(L, onRequestLayOut_cb, this);		// for now - hope it won't correct cassowary's decisions!
		// TODO: Generally this sould be before - but there is no "moving_children_now" - so can't prevent from being raised need_lay_out flag
		need_lay_out = false;
	}

	assert(!need_lay_out);
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
		
		.property("gtop", &LuaScreenItem::getAbsoluteTop)
		.property("gbottom", &LuaScreenItem::getAbsoluteBottom)
		.property("gleft", &LuaScreenItem::getAbsoluteLeft)
		.property("gright", &LuaScreenItem::getAbsoluteRight)

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
		.property("debugDrawColor", &LuaScreenItem::getDebugDrawColor, &LuaScreenItem::setDebugDrawColor)

		.def(luabind::self == luabind::other<LuaScreenItem&>())				// remove operator ==
		.def(luabind::self < luabind::other<LuaScreenItem&>())				// for use in map
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
		.def_readwrite("onRequestSize", &LuaCompositeItem::onRequestSize_cb)
		.def("requestLayOut", &LuaCompositeItem::requestLayOut)
		.def("adjustLayout", &LuaCompositeItem::adjustLayout)
		.def("adjustSize", &LuaCompositeItem::adjustSize, luabind::pure_out_value(_2) + luabind::pure_out_value(_3))
		// TODO: should be readonly: used only once as hack!
		.def_readwrite("need_lay_out", &LuaCompositeItem::need_lay_out)
		.def_readonly("need_lay_out_children", &LuaCompositeItem::need_lay_out_children)

		.def(luabind::self == luabind::other<LuaScreenItem&>())				// remove operator ==
	];

	luabind::globals(L)["root"] = dynamic_cast<LuaCompositeItem*>(root_item());
}
