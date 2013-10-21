#pragma once

#include "LuaScreenItem.h"

#include <luabind/luabind.hpp>

class ImageItem: virtual public ScreenItem
{
public:
	ImageItem();
	ImageItem(std::string path);
	~ImageItem(){}
	bool operator==(ImageItem& rhs){return false;}
	void setScaleX(float sx){
		assert(getParent());
		CL_Vec2f scale = entity->GetVar("scale2d")->GetVector2();
		scale.x = sx;
		entity->GetVar("scale2d")->Set(scale);
	}
	void setScaleY(float sy){
		assert(getParent());
		CL_Vec2f scale = entity->GetVar("scale2d")->GetVector2();
		scale.y = sy;
		entity->GetVar("scale2d")->Set(scale);
	}
	float getScaleX(){
		assert(getParent());
		return entity->GetVar("scale2d")->GetVector2().x;
	}
	float getScaleY(){
		assert(getParent());
		return entity->GetVar("scale2d")->GetVector2().y;
	}

	void setFile(std::string);

private:
	std::string tex_file;
	OverlayRenderComponent* component;
};

class LuaImageItem: public ImageItem, public LuaScreenItem{
public:
	LuaImageItem(std::string path);
	static void luabind(lua_State* L);
	bool operator==(ImageItem& rhs){return false;}

private:
	LuaImageItem(const LuaImageItem&){assert(false);}
	LuaImageItem& operator=(const LuaImageItem&){assert(false);}
};