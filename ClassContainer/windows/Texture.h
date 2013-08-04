#pragma once

#include "Entity/Component.h"
#include "Entity/Entity.h"
#include "Renderer/Surface.h"
#include <luabind/luabind.hpp>

class Texture: public EntityComponent
{
public:
	Texture(std::string path, int w, int h);
	~Texture();
	virtual void OnAdd(Entity* e);

	float getWidth() const {
		return size->x;
	}

	float getHeight() const {
		return size->y;
	}
private:
	Surface* tex;
	CL_Vec2f *pos, *size;

	void OnRender(VariantList *args);
};

class LuaTexture: public Texture{
public:
	LuaTexture(std::string path, int w, int h);
	static void luabind(lua_State* L);
	bool operator==(Texture& rhs){return false;}

private:
	LuaTexture(const LuaTexture&):Texture("",0,0){assert(false);}
	LuaTexture& operator=(const LuaTexture&){assert(false);}
};