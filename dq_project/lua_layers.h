#pragma once

#include <string>
#include <luabind/luabind.hpp>
#include "Visual.h"

namespace Layers{
	extern void add_layer(std::string name);
	extern void set_layer(std::string name);
	extern int num_layers();
	extern Visual* get_layer(int i);
	extern void luabind(lua_State* L);

	extern CompositeVisual* root_visual();
}// namespace