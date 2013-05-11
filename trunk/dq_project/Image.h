#pragma once

#include "ScreenResource.h"
#include "Entity/OverlayRenderComponent.h"
#include <luabind/luabind.hpp>

class Image: public ScreenResource
{
private:
	void assignEntity(Entity* e){
		e->AddComponent(comp);
		// HACK: need to re-setup file name so component can catch it...
		comp->GetVar("fileName")->Set(tex_file);
	}
public:
	Image();
	Image(std::string path);
	~Image(){free_all();}
//	void Render(float x, float y, float rot);
	float getWidth(){
		return comp->GetVar("frameSize2d")->GetVector2().x;
	}
	float getHeight(){
		return comp->GetVar("frameSize2d")->GetVector2().y;
	}
	void setFile(std::string);

private:
	std::string tex_file;
	OverlayRenderComponent* comp;

	// TODO Guess this will be deleted by Entity...
	// utilitary
	void free_all()
	{
		if(comp && comp->GetParent()==0)
			delete comp;
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