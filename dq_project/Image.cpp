#include "PlatformPrecomp.h"
#include "Image.h"
#include <luabind/operator.hpp>

ImageItem::ImageItem()
{
	component = new OverlayRenderComponent();
	entity->AddComponent(component);
}

ImageItem::ImageItem(std::string path){
	component = new OverlayRenderComponent();
	entity->AddComponent(component);
	setFile(path);
}

void ImageItem::setFile(std::string path){
	tex_file = path;			// will be caught by Container from Item
	component->GetVar("fileName")->Set(tex_file);
}

LuaImageItem::LuaImageItem(std::string path):ImageItem(path){
	return;
}

// need it to bind EntityComponent
bool operator==(EntityComponent& c1, EntityComponent& c2){return false;}

void LuaImageItem::luabind(lua_State* L){
	luabind::module(L) [
		luabind::class_<LuaImageItem, LuaScreenItem>("ImageItem")
			.def(luabind::constructor<std::string>())
			.property("width", &LuaImageItem::getWidth)
			.property("height", &LuaImageItem::getHeight)
			.property("frameWidth", &LuaImageItem::getFrameWidth)
			.property("frameHeight", &LuaImageItem::getFrameHeight)
			.property("scaleX", &LuaImageItem::getScaleX, &LuaImageItem::setScaleX)
			.property("scaleY", &LuaImageItem::getScaleY, &LuaImageItem::setScaleY)
			.def(luabind::self == luabind::other<LuaImageItem&>())		// remove operator ==
	];
}
