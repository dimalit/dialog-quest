#include "PlatformPrecomp.h"

#include "PlatformSetup.h"
#include "App.h"
#include "lua_lib.h"

#include <stdlib.h>
#include <time.h>

float float_rand(){
	unsigned int r = rand();
	float res = (float)rand() / (float)RAND_MAX;
	return res;
}

namespace Lualib{
	void luabind(lua_State* L){

		srand(time(0));

		luabind::module(L)
		[
			luabind::def("rand", &float_rand)
		];

		// TODO what about screen rotation?
		luabind::globals(L)["screen_width"] = GetScreenSizeX();
		luabind::globals(L)["screen_height"] = GetScreenSizeY();
	}
}