#pragma once
#include "ScreenItem.h"
#include "App.h"
#include <luabind/luabind.hpp>
#include <cassert>

class LuaScreenItem: virtual public ScreenItem{
public:
	LuaScreenItem(CompositeItem* parent = 0):ScreenItem(parent){}
	static void luabind(lua_State* L);

private:
	LuaScreenItem(const LuaScreenItem&){assert(false);}
	LuaScreenItem& operator=(const LuaScreenItem&){assert(false);}
};

class LuaCompositeItem: public CompositeItem, public LuaScreenItem{
public:
	LuaCompositeItem(CompositeItem* parent = 0):CompositeItem(parent), LuaScreenItem(NULL){}
	static void luabind(lua_State* L);

private:
	LuaCompositeItem(const LuaCompositeItem&){assert(false);}
	LuaCompositeItem& operator=(const LuaCompositeItem&){assert(false);}
};

// needs to be public!
// error C2243: 'type cast' : conversion from 'LuaSimpleItem *' to 'ScreenItem *' exists, but is inaccessible
class LuaSimpleItem: public SimpleItem, public LuaScreenItem
{
public:
	// need to be public for luabind
	bool operator == (LuaSimpleItem&){return false;}
	LuaSimpleItem(float x, float y): SimpleItem(x, y), LuaScreenItem(NULL){}
	LuaSimpleItem(): SimpleItem(){}
	~LuaSimpleItem(){
	}
	static void luabind(lua_State* L);

private:
	luabind::object getQuad(){
		float x[4], y[4];
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

	luabind::object onDrag_cb;
	luabind::object onDbClick_cb;
	luabind::object onDragStart_cb;
	luabind::object onDragEnd_cb;
	luabind::object onChar_cb;
	luabind::object onFocusLose_cb;

	void destroy(){
		onDrag_cb = luabind::object();
		onDbClick_cb = luabind::object();
		onDragStart_cb = luabind::object();
		onDragEnd_cb = luabind::object();
		onChar_cb = luabind::object();
		onFocusLose_cb = luabind::object();

		this->setView(NULL);			// remove render component
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
};
