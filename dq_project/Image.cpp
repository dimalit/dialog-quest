#include "PlatformPrecomp.h"
#include "Image.h"
#include <luabind/operator.hpp>

Image::Image()
{
}

Image::Image(std::string path){
	setFile(path);
}

//void Image::Render(float x, float y, float rot){
//	assert(0 && "Not implemented - just added to remove class abstractness.");
////	if(spr)spr->RenderEx(x, y, rot);
//}

void Image::OnAdd(Entity* e){
	OverlayRenderComponent::OnAdd(e);
	// HACK: components need all parameters AFTER attach...
	GetVar("fileName")->Set(tex_file);
}

void Image::setFile(std::string path){
	tex_file = path;			// will be caught by Container from Item
	GetVar("fileName")->Set(tex_file);
}

LuaImage::LuaImage(std::string path):Image(path){
	return;
}

// need it to bind EntityComponent
bool operator==(EntityComponent& c1, EntityComponent& c2){return false;}

void LuaImage::luabind(lua_State* L){
	//luabind::module(L) [
	//	luabind::class_<ScreenResource>("ScreenResource")
	//		.property("width", &ScreenResource::getWidth)
	//		.property("height", &ScreenResource::getHeight)
	//		.def(luabind::self == luabind::other<ScreenResource&>())		// remove operator ==
	//];

	luabind::module(L) [
		luabind::class_<EntityComponent>("_DoNotUse_EntityComponent")
			.def(luabind::self == luabind::other<EntityComponent&>())		// remove operator == (C2678)
	];

	luabind::module(L) [
		luabind::class_<LuaImage, EntityComponent>("Image")
			.def(luabind::constructor<std::string>())
			.property("width", &Image::getWidth)
			.property("height", &Image::getHeight)
			.def(luabind::self == luabind::other<LuaImage&>())		// remove operator ==
	];
}

//void LuaImage::luabind(lua_State* L){
//	//luabind::module(L) [
//	//	luabind::class_<ScreenResource>("ScreenResource")
//	//		.property("width", &ScreenResource::getWidth)
//	//		.property("height", &ScreenResource::getHeight)
//	//		.def(luabind::self == luabind::other<ScreenResource&>())		// remove operator ==
//	//];
//	luabind::module(L) [
//		luabind::class_<LuaImage>("Image")
//			.def(luabind::constructor<std::string>())
//			.property("width", &Image::getWidth)
//			.property("height", &Image::getHeight)
//			.def(luabind::self == luabind::other<Image&>())		// remove operator ==
//	];
//}
