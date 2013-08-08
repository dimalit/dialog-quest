#include "PlatformPrecomp.h"
#include "Texture.h"
#include <luabind/operator.hpp>

Texture::Texture(std::string path, int w, int h)
{
	tex = new Surface(path);
	this->size = new CL_Vec2f(w, h);
}

Texture::~Texture(void)
{
	delete tex;
}

void Texture::OnAdd(Entity *e){
	EntityComponent::OnAdd(e);

	pos = &GetParent()->GetVar("pos2d")->GetVector2();
	GetParent()->GetVar("size2d")->Set(*size);
	delete size;
	size = &GetParent()->GetVar("size2d")->GetVector2();

	GetParent()->GetFunction("OnRender")->sig_function.connect(1, boost::bind(&Texture::OnRender, this, _1));	
}

void Texture::OnRender(VariantList *args){
	if(!tex)
		return;
	CL_Vec2f abs_pos = args->m_variant[0].GetVector2() + *(this->pos);
	tex->BlitRepeated(rtRectf(abs_pos.x, abs_pos.y, abs_pos.x+size->x, abs_pos.y+size->y));
}

LuaTexture::LuaTexture(std::string path, int w, int h):Texture(path, w, h){
	return;
}

void LuaTexture::luabind(lua_State* L){
	luabind::module(L) [
		luabind::class_<LuaTexture, EntityComponent>("Texture")
			.def(luabind::constructor<std::string, int, int>())
			// TODO: Also changing the dimensions? File name?
			.property("width", &Texture::getWidth)
			.property("height", &Texture::getHeight)
			.def(luabind::self == luabind::other<LuaTexture&>())		// remove operator ==
	];
}