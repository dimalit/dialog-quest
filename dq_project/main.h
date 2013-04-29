#pragma once

#include <string>
#include <cassert>

#define APP_NAME "Demo"

#include <hge.h>
#include <hgeresource.h>
#include <luabind/luabind.hpp>
extern HGE* hge;
extern lua_State* L;
extern hgeResourceManager* res_manager;
extern int screen_width;
extern int screen_height;

extern void switch_to_english();
extern bool http2file(std::string from_path, std::string to_path="");
extern bool download_resources();
extern std::string get_localdata_folder();