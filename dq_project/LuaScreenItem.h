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
	LuaScreenItem(bool soft=true);
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
	LuaCompositeItem(bool soft=false):CompositeItem(soft), LuaScreenItem(soft), ScreenItem(soft){
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
	virtual void doLayOutIfNeeded();
};

class LuaRigidCompositeItem: public LuaCompositeItem{
public:
	LuaRigidCompositeItem():LuaCompositeItem(false){}
};
class LuaSoftCompositeItem: public LuaCompositeItem{
public:
	LuaSoftCompositeItem():LuaCompositeItem(true){}
};