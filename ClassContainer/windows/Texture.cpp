#include "PlatformPrecomp.h"
#include "Texture.h"
#include <luabind/operator.hpp>

TextureItem::TextureItem(std::string path, int w, int h)
{
	this->path = path;
	if(!path.empty())
		tex = new Surface(path);
	else
		tex = 0;

	entity->GetVar("size2d")->Set(w, h);

	component = new EntityComponent();
	entity->AddComponent(component);

	pos = &entity->GetVar("pos2d")->GetVector2();
	size = &entity->GetVar("size2d")->GetVector2();
	m_pVisible = &entity->GetVarWithDefault("visible", uint32(1))->GetUINT32();

	entity->GetFunction("OnRender")->sig_function.connect(1, boost::bind(&TextureItem::OnRender, this, _1));	
}

TextureItem::~TextureItem(void)
{
	delete tex;
}

void TextureItem::OnRender(VariantList *args){
	if(!tex || !*m_pVisible)
		return;
	CL_Vec2f abs_pos = args->m_variant[0].GetVector2() + *(this->pos);
	tex->BlitRepeated(rtRectf(abs_pos.x, abs_pos.y, abs_pos.x+size->x, abs_pos.y+size->y));
}

LuaTextureItem::LuaTextureItem(std::string path, int w, int h):TextureItem(path, w, h){
	return;
}

void LuaTextureItem::luabind(lua_State* L){
	luabind::module(L) [
		luabind::class_<LuaTextureItem, LuaScreenItem>("TextureItem")
			.def(luabind::constructor<std::string, int, int>())
			// TODO: Also changing the dimensions? File name?
			.property("texture", &TextureItem::getTexture, &TextureItem::setTexture)
			.def(luabind::self == luabind::other<LuaTextureItem&>())		// remove operator ==
	];
}