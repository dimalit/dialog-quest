#pragma once

#include "timer.h"

#include "App.h"

#include <luabind/luabind.hpp>

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
		if(cb) luabind::call_function<void>(L, cb, this);
	}
};
