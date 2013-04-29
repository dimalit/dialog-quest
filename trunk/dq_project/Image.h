#pragma once
#include <hgeSprite.h>
#include "main.h"
#include "ScreenResource.h"

class Image: public ScreenResource
{
public:
	Image();
	Image(std::string path);
	~Image(){free_all();}
	void Render(float x, float y, float rot);
	float getWidth(){
		float r = spr ? spr->GetWidth() : 0;
		return r;
	}
	float getHeight(){
		float r = spr ? spr->GetHeight() : 0;
		return r;
	}
	void setFile(std::string);

private:
	HTEXTURE tex;
	hgeSprite* spr;

	// utilitary
	void free_all()
	{
		if(spr)delete spr;
		if(tex)hge->Texture_Free(tex);
	}
};

class LuaImage: public Image{
public:
	LuaImage(std::string path);
	static void luabind(lua_State* L);

private:
	LuaImage(const LuaImage&){assert(false);}
	LuaImage& operator=(const LuaImage&){assert(false);}
};