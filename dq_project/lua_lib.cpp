#include "PlatformPrecomp.h"

#include "lua_lib.h"
#include "PlatformSetup.h"
#include "App.h"

#include <luabind/iterator_policy.hpp>
#include <stdlib.h>
#include <time.h>

float float_rand(){
	unsigned int r = rand();
	float res = (float)rand() / (float)RAND_MAX;
	return res;
}

luabind::object random_permutation(int n){
	std::vector<int> res(n);
	for(int i=0; i<n; i++)
		res[i] = i+1;
	for(int i=0; i<n-1; i++)
		std::swap(res[i], res[i + rand()%(n-i)]);

	luabind::object o = luabind::newtable(L);
	for(int i=0; i<n; i++)
		o[i+1]=res[i];
	return o;
}

namespace Lualib{
	void luabind(lua_State* L){

		srand(time(0));

		luabind::module(L)
		[
			luabind::def("rand", &float_rand),
			luabind::def("random_permutation", &random_permutation)
		];

		// TODO what about screen rotation?
		luabind::globals(L)["screen_width"] = GetScreenSizeX();
		luabind::globals(L)["screen_height"] = GetScreenSizeY();
	}
}