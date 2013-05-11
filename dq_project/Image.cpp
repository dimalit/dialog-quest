#include "PlatformPrecomp.h"
#include "Image.h"
#include <luabind/operator.hpp>

Image::Image()
{
	comp = new OverlayRenderComponent();
}

Image::Image(std::string path){
	comp = new OverlayRenderComponent();
	setFile(path);
}

//void Image::Render(float x, float y, float rot){
//	assert(0 && "Not implemented - just added to remove class abstractness.");
////	if(spr)spr->RenderEx(x, y, rot);
//}

void Image::setFile(std::string path){
	tex_file = path;			// will be caught by Container from Item
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
