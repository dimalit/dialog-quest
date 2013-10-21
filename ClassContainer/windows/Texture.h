#pragma once

#include "LuaScreenItem.h"

#include "Entity/Component.h"
#include "Entity/Entity.h"
#include "Renderer/Surface.h"

#include <luabind/luabind.hpp>

class TextureItem: virtual public ScreenItem
{
public:
	TextureItem(std::string path, int w, int h);
	~TextureItem();

	string getTexture() const {
		return path;
	}
	void setTexture(const string& new_path){
		delete tex;
		path = new_path;
		tex = new Surface(path);
	}
private:
	EntityComponent* component;

	string path;
	Surface* tex;
	CL_Vec2f *pos, *size;
	uint32 *m_pVisible;

	void OnRender(VariantList *args);
};

class LuaTextureItem: public TextureItem, public LuaScreenItem{
public:
	LuaTextureItem(std::string path, int w, int h);
	static void luabind(lua_State* L);
	bool operator==(TextureItem& rhs){return false;}

private:
	LuaTextureItem(const LuaTextureItem&):TextureItem("",0,0){assert(false);}
	LuaTextureItem& operator=(const LuaTextureItem&){assert(false);}
};