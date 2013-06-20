#pragma once

#include "Entity/OverlayRenderComponent.h"
#include <luabind/luabind.hpp>

class Image: public OverlayRenderComponent
{
public:
	Image();
	Image(std::string path);
	~Image(){}
	bool operator==(Image& rhs){return false;}
//	void Render(float x, float y, float rot);
	virtual void OnAdd(Entity* e);
	float getWidth(){
		return GetVar("frameSize2d")->GetVector2().x;
	}
	float getHeight(){
		return GetVar("frameSize2d")->GetVector2().y;
	}
	void setFile(std::string);

private:
	std::string tex_file;
};

class LuaImage: public Image{
public:
	LuaImage(std::string path);
	static void luabind(lua_State* L);
	bool operator==(Image& rhs){return false;}

private:
	LuaImage(const LuaImage&){assert(false);}
	LuaImage& operator=(const LuaImage&){assert(false);}
};