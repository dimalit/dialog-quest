#include "Image.h"
#include <luabind/operator.hpp>

Image::Image()
{
	tex = 0;
	spr = 0;
}

Image::Image(std::string path){
	tex = 0;
	spr = 0;
	setFile(path);
}

void Image::Render(float x, float y, float rot){
	if(spr)spr->RenderEx(x, y, rot);
}

void Image::setFile(std::string path){
	free_all();
	tex = hge->Texture_Load(path.c_str());
	if(tex){
		float tw = hge->Texture_GetWidth(tex, true);		// true width
		float th = hge->Texture_GetHeight(tex, true);		// true height
		spr = new hgeSprite(tex, 0, 0, tw, th);
		float hx, hy;
//		spr->GetHotSpot(&hx, &hy);
//		std::cout << hx << " -- " << hy << "\n";
	}
}

LuaImage::LuaImage(std::string path):Image(path){
	return;
}

void LuaImage::luabind(lua_State* L){
	luabind::module(L) [
		luabind::class_<ScreenResource>("ScreenResource")
			.property("width", &ScreenResource::getWidth)
			.property("height", &ScreenResource::getHeight)
			.def(luabind::self == luabind::other<ScreenResource&>())		// remove operator ==
	];
	luabind::module(L) [
		luabind::class_<LuaImage, ScreenResource>("Image")
			.def(luabind::constructor<std::string>())
	];
}
