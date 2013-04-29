#include <stdlib.h>
#include <time.h>
#include "main.h"
#include "lua_lib.h"

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

		luabind::globals(L)["screen_width"] = screen_width;
		luabind::globals(L)["screen_height"] = screen_height;
	}
}