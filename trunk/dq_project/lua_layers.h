#pragma once

#include "ScreenItem.h"
#include <luabind/luabind.hpp>
#include <string>

namespace Layers{
	extern void add_layer(std::string name);
	extern void set_layer(std::string name);
	extern int num_layers();
	extern ScreenItem* get_layer(int i);
	extern void luabind(lua_State* L);

	extern CompositeItem* root_item();
}// namespace