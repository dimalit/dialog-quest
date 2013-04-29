#pragma once

#include <string>
#include <cassert>
#include <luabind/luabind.hpp>

#include "Widget.h"

class LuaWidget: public virtual Widget
{
public:
	static void luabind(lua_State* L);
	bool operator == (LuaWidget&){return false;}
protected:
	LuaWidget();

	luabind::object getRect(){
		luabind::object t = luabind::newtable(L);
		t["x1"] = x1;
		t["y1"] = y1;
		t["x2"] = x2;
		t["y2"] = y2;

		return t;
	}

private:
	LuaWidget(const LuaWidget& w):Widget(0,0){assert(false);}
	LuaWidget& operator=(const LuaWidget& w){assert(false);}
	static lua_State* L;

	luabind::object onDrag_cb;
	luabind::object onDbClick_cb;
	luabind::object onDragStart_cb;
	luabind::object onDragEnd_cb;
	luabind::object onChar_cb;
	luabind::object onFocusLose_cb;

	virtual void onDrag(float dx, float dy){
		Widget::onDrag(dx, dy);
		if(onDrag_cb)
			luabind::call_function<void>(onDrag_cb, this);
	}
	virtual void onDbClick(){
		Widget::onDbClick();
		if(onDbClick_cb)
			luabind::call_function<void>(onDbClick_cb, this);
	}
	virtual void onDragStart(){
		Widget::onDragStart();
		if(onDragStart_cb)
			luabind::call_function<void>(onDragStart_cb, this);
	}
	virtual void onDragEnd(){
		Widget::onDragEnd();
		try{
		if(onDragEnd_cb)
			luabind::call_function<void>(onDragEnd_cb, this);
		}catch(std::exception& e){
			std::cerr << "Error: " << e.what();
		}
	}
	virtual void onChar(int chr){
		Widget::onChar(chr);
		if(onChar_cb)
			luabind::call_function<void>(onChar_cb, this, chr);
	}
	virtual void onFocusLose(){
		Widget::onFocusLose();
		if(onFocusLose_cb)
			luabind::call_function<void>(onFocusLose_cb, this);
	}
};
