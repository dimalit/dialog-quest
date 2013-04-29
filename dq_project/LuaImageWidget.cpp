#include "LuaImageWidget.h"
#include <luabind/operator.hpp>

LuaImageWidget::LuaImageWidget(luabind::object conf, float x, float y):Widget(x,y), ImageWidget(x,y){
	std::string names[] = {"normal", "over", "checked", "normal2checked"};
	view_type types[] = {VIEW_NORMAL, VIEW_OVER, VIEW_CHECKED, NORMAL2CHECKED};

	for(int i=0; i<4; i++){
		luabind::object o = conf[names[i].c_str()];
		if(o){
			std::string file = luabind::object_cast<std::string>(o["file"]);
			float w = luabind::object_cast<float>(o["width"]);
			float h = luabind::object_cast<float>(o["height"]);
			float tex_x = luabind::object_cast<float>(o["tex_x"]);
			float tex_y = luabind::object_cast<float>(o["tex_y"]);
			// optional
			float nframes = 1;
			float fps = 1;
			try{
				nframes = luabind::object_cast<float>(o["nframes"]);
				fps = luabind::object_cast<float>(o["fps"]);
			}catch(luabind::cast_failed){/*ignore*/}
			setAnim(types[i], file, w, h, tex_x, tex_y, nframes, fps);
		}// if
	}// for
	update_view();
}

void LuaImageWidget::luabind(lua_State* L){
	luabind::module(L) [
		luabind::class_<LuaImageWidget, LuaWidget>("ImageWidget")
			.def(luabind::constructor<luabind::object, float, float>())
	];	
}