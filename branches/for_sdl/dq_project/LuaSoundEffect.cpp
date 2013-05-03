#include <luabind/luabind.hpp>
#include "LuaSoundEffect.h"

void LuaSoundEffect::Callback::operator()(){
	if(eff->onFinish_cb)
		luabind::call_function<void>(eff->onFinish_cb, eff);
}

void LuaSoundEffect::play_4(luabind::object volume, luabind::object pan, luabind::object pitch, luabind::object loop){
	// some args may be nil
	// prepare c-counterparts
	bool  c_loop = false;
	float c_pitch = 1.0f;
	int   c_pan = 0, c_volume = 100;

	// check
	if(loop)
		c_loop = luabind::object_cast<bool>(loop);
	if(pitch)
		c_pitch = luabind::object_cast<float>(pitch);
	if(pan)
		c_pan = luabind::object_cast<int>(pan);
	if(volume)
		c_volume = luabind::object_cast<int>(volume);

	SoundEffect::play(c_volume, c_pan, c_pitch, c_loop);
	timer.start(getLength());
}

void LuaSoundEffect::luabind(lua_State* L){
	luabind::module(L) [
	luabind::class_<LuaSoundEffect>("SoundEffect")
	.def(luabind::constructor<std::string>())
	.def("play", (void (LuaSoundEffect::*)(luabind::object, luabind::object, luabind::object, luabind::object)) &LuaSoundEffect::play_4)
		.def("play", (void (LuaSoundEffect::*)(luabind::object, luabind::object, luabind::object)) &LuaSoundEffect::play)
		.def("play", (void (LuaSoundEffect::*)(luabind::object, luabind::object)) &LuaSoundEffect::play)
		.def("play", (void (LuaSoundEffect::*)(luabind::object)) &LuaSoundEffect::play)
		.def("play", (void (LuaSoundEffect::*)(void)) &LuaSoundEffect::play)
		.def_readwrite("onFinish", &LuaSoundEffect::onFinish_cb)
	];
}