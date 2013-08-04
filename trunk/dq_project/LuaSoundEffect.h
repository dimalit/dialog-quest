#pragma once
#include "Timer.h"
#include "SoundEffect.h"
#include <luabind/luabind.hpp>

class LuaSoundEffect: public SoundEffect
{
private:
	// for timer
	class Callback{
		LuaSoundEffect* eff;
	public:
		Callback(LuaSoundEffect* e){eff = e;}
		void operator()();
	};
	friend class Callback;
	Timer<Callback> timer;
	luabind::object onFinish_cb;
public:
	LuaSoundEffect(std::string path): SoundEffect(path), timer(Callback(this)){}
	// make one full-featured function
	// and overload all numbers of arguments :(
	void play_4(luabind::object volume = luabind::object(),		// 4
			  luabind::object pan    = luabind::object(),
			  luabind::object pitch  = luabind::object(),
			  luabind::object loop   = luabind::object());
	void play(luabind::object volume,							// 3
			  luabind::object pan,
			  luabind::object pitch)
	{
		play_4(volume, pan, pitch);
	}
	void play(luabind::object volume,							// 2
			  luabind::object pan)
	{
		play_4(volume, pan);
	}
	void play(luabind::object volume){							// 1
		play_4(volume);	
	}
	void play(){												// 0
		play_4();
	}

	static void luabind(lua_State* L);
};
