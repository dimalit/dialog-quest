#pragma once
#include "ScreenItem.h"
#include "App.h"
#include <luabind/luabind.hpp>
#include <cassert>

class LuaCompositeItem;

class LuaScreenItem: virtual public ScreenItem{
public:
	bool operator == (LuaScreenItem&){return false;}
	LuaScreenItem();
	static void luabind(lua_State* L);
	LuaCompositeItem* getParent();

private:
	LuaScreenItem(const LuaScreenItem&){assert(false);}
	LuaScreenItem& operator=(const LuaScreenItem&){assert(false);}

	luabind::object getQuad(){
		int x[4], y[4];
		for(int i = 0; i<4; i++)
			compute_corner(i+1, x[i], y[i]);
		luabind::object res = luabind::newtable(L);
		res[1] = luabind::newtable(L);
			res[1]["x"] = x[0];
			res[1]["y"] = y[0];
		res[2] = luabind::newtable(L);
			res[2]["x"] = x[1];
			res[2]["y"] = y[1];
		res[3] = luabind::newtable(L);
			res[3]["x"] = x[2];
			res[3]["y"] = y[2];
		res[4] = luabind::newtable(L);
			res[4]["x"] = x[3];
			res[4]["y"] = y[3];
		return res;
	}

	luabind::object onMove_cb;
	luabind::object onDrag_cb;
	luabind::object onDbClick_cb;
	luabind::object onDragStart_cb;
	luabind::object onDragEnd_cb;
	luabind::object onChar_cb;
	luabind::object onFocusLose_cb;

	virtual void onMove(Variant* v){
		ScreenItem::onMove(v);
		if(onMove_cb)
			luabind::call_function<void>(onMove_cb, this);
	}

	virtual void onDrag(float dx, float dy){
		if(onDrag_cb)
			luabind::call_function<void>(onDrag_cb, this, dx, dy);
	}
	virtual void onDbClick(){
		if(onDbClick_cb)
			luabind::call_function<void>(onDbClick_cb, this);
	}
	virtual void onDragStart(){
		if(onDragStart_cb)
			luabind::call_function<void>(onDragStart_cb, this);
	}
	virtual void onDragEnd(){
		if(onDragEnd_cb)
			luabind::call_function<void>(onDragEnd_cb, this);
	}
	virtual void onChar(int chr){
		if(onChar_cb)
			luabind::call_function<void>(onChar_cb, this, chr);
	}
	virtual void onFocusLose(){
		if(onFocusLose_cb)
			luabind::call_function<void>(onFocusLose_cb, this);
	}
protected:
		virtual void destroy(){
			onDrag_cb = luabind::object();
			onDbClick_cb = luabind::object();
			onDragStart_cb = luabind::object();
			onDragEnd_cb = luabind::object();
			onChar_cb = luabind::object();
			onFocusLose_cb = luabind::object();

			// TODO: Implement it correctly! (visible=0, delete children...)
			setVisible(false);
		}
};

class LuaCompositeItem: public LuaScreenItem, public CompositeItem{
public:
	bool operator == (LuaScreenItem&){return false;}
	LuaCompositeItem():CompositeItem(), LuaScreenItem(), ScreenItem(){
		request_layout_is_running = false;
		children = luabind::newtable(L);
	}
	static void luabind(lua_State* L);

	LuaCompositeItem* add(LuaScreenItem* child){
		CompositeItem::add(child);
		return this;
	}

	LuaCompositeItem* remove(LuaScreenItem* child){
		CompositeItem::remove(child);
		return this;
	}

	LuaCompositeItem* add(luabind::object child){
		children[child] = true;

		LuaScreenItem* it;
		while(luabind::type(child) != LUA_TUSERDATA && luabind::type(child) != LUA_TNIL){
			child = child["item"];
		}
		it = luabind::object_cast<LuaScreenItem*>(child);
		if(it==NULL)
			luaL_error(child.interpreter(), "Can't convert value to ScreenItem!");
		else
			CompositeItem::add(it);
		return this;
	}

	LuaCompositeItem* remove(luabind::object child){
		children[child] = luabind::nil;

		LuaScreenItem* it;
		while(luabind::type(child) != LUA_TUSERDATA && luabind::type(child) != LUA_TNIL){
			child = child["item"];
		}
		it = luabind::object_cast<LuaScreenItem*>(child);
		if(it==NULL)
			luaL_error(child.interpreter(), "Can't convert value to ScreenItem!");
		else
			CompositeItem::remove(it);
		return this;
	}

	virtual void requestLayOut(ScreenItem* child){
		// Prevent deep recursion
		if(request_layout_is_running)
			return;
		request_layout_is_running = true;
		if(onRequestLayOut_cb)
			luabind::call_function<void>(onRequestLayOut_cb, this, child);
		request_layout_is_running = false;
	}
	virtual void requestLayOut(luabind::object child){
		// Prevent deep recursion
		if(request_layout_is_running)
			return;
		request_layout_is_running = true;
		if(onRequestLayOut_cb)
			luabind::call_function<void>(onRequestLayOut_cb, this, child);
		request_layout_is_running = false;
	}
private:
	bool request_layout_is_running;
	luabind::object onRequestLayOut_cb;
	luabind::object children;
	LuaCompositeItem(const LuaCompositeItem&):CompositeItem(){assert(false);}
	LuaCompositeItem& operator=(const LuaCompositeItem&){assert(false);}
};

class CompositeItem;

// needs to be public!
// error C2243: 'type cast' : conversion from 'LuaSimpleItem *' to 'ScreenItem *' exists, but is inaccessible
class LuaSimpleItem: public LuaScreenItem, public SimpleItem
{
public:
	// need to be public for luabind
	bool operator == (LuaScreenItem&){return false;}
	LuaSimpleItem():SimpleItem(), LuaScreenItem(), ScreenItem(){}
	~LuaSimpleItem(){
	}
	static void luabind(lua_State* L);

protected:
	virtual void destroy(){
		LuaScreenItem::destroy();
		this->setView(NULL);			// remove render component
	}
};
