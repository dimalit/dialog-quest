#pragma once
#include <luabind/luabind.hpp>
#include "timer.h"

class LuaTimer;

class Callback{
private:
	LuaTimer* timer;
public:
	Callback(LuaTimer* t){timer = t;}
	void operator()();
};

class LuaTimer:	public Timer<Callback>
{

public:
	LuaTimer(luabind::object cb, float dt = -1.0f);
	static void luabind(lua_State* L, const char* classname);
private:
	friend class Callback;
	// call lua
	luabind::object cb;
	void fire(){
		if(cb) luabind::call_function<void>(cb, this);
	}
};
