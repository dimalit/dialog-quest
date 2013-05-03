#include "LuaTimer.h"

void Callback::operator()(){
	timer->fire();
}

LuaTimer::LuaTimer(luabind::object cb, float dt)
:Timer(Callback(this), dt)
{
	this->cb = cb;
}

void LuaTimer::luabind(lua_State* L, const char* classname){
	luabind::module(L) [
	luabind::class_<LuaTimer>(classname)
	.def(luabind::constructor<luabind::object , float>())
	.def(luabind::constructor<luabind::object>())
//	.def("start", &LuaTimer::start)
	.def("start", (void (LuaTimer::*)(float)) &LuaTimer::start)
	.def("start", (void (LuaTimer::*)(void)) &LuaTimer::start)
	.def("restart", (void (LuaTimer::*)(float)) &LuaTimer::restart)
	.def("restart", (void (LuaTimer::*)(void)) &LuaTimer::restart)
	.def("cancel", &LuaTimer::cancel)
	.def_readwrite("callback", &LuaTimer::cb)
	.def_readonly("elapsed", &LuaTimer::elapsed)
	];
}