#pragma once
#include "ScreenItem.h"
#include "App.h"
#include <luabind/luabind.hpp>
#include <cassert>

class LuaCompositeItem;

class LuaScreenItem: virtual public ScreenItem{
public:
	bool operator == (LuaScreenItem&){return false;}
	bool operator < (LuaScreenItem& rhs){return this < &rhs;}		// for storing in map
	LuaScreenItem();
	static void luabind(lua_State* L);
	LuaCompositeItem* getParent();

// HACK: For some reason its needed by luabind - so I left it in public
	LuaScreenItem(const LuaScreenItem&){assert(false);}
private:
	LuaScreenItem& operator=(const LuaScreenItem&){assert(false);}

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
			luabind::call_function<void>(L, onMove_cb, this);
	}

	virtual void onDrag(float dx, float dy){
		if(onDrag_cb)
			luabind::call_function<void>(L, onDrag_cb, this, dx, dy);
	}
	virtual void onDbClick(){
		if(onDbClick_cb)
			luabind::call_function<void>(L, onDbClick_cb, this);
	}
	virtual void onDragStart(){
		if(onDragStart_cb)
			luabind::call_function<void>(L, onDragStart_cb, this);
	}
	virtual void onDragEnd(){
		if(onDragEnd_cb)
			luabind::call_function<void>(L, onDragEnd_cb, this);
	}
	virtual void onChar(int chr){
		if(onChar_cb)
			luabind::call_function<void>(L, onChar_cb, this, chr);
	}
	virtual void onFocusLose(){
		if(onFocusLose_cb)
			luabind::call_function<void>(L, onFocusLose_cb, this);
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
		last_requested_width = -1;			// will break comparison to cached in adjustSize()
		last_requested_height = -1;
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

private:
	LuaCompositeItem(const LuaCompositeItem&):CompositeItem(){assert(false);}
	LuaCompositeItem& operator=(const LuaCompositeItem&){assert(false);}
	
	luabind::object onRequestLayOut_cb;
	luabind::object onRequestSize_cb;
	virtual void adjustLayout();
	void adjustSize(bool& rigid_width, bool& rigid_height);

	// "cache" vars for adjustSize
	int last_requested_width, last_requested_height;
	bool last_requested_rigid_width, last_requested_rigid_height;
};
