#include <cassert>
#include "MovableItem.h"

//lua_State* MovableItem::lua = 0;

void MovableItem::luabind(lua_State* L){
	// Export our class with LuaBind
	luabind::module(L) [
	luabind::class_<MovableItem>("MovableItem")
	.def(luabind::constructor<float, float>())
	.def("move", &MovableItem::move)
	.def_readwrite("callback", &MovableItem::callback)
	];
}

void MovableItem::move(float dx, float dy){
	x+=dx;
	y+=dy;
}

void MovableItem::onChange(){
	if(callback){
		//luabind::call_function<void>(lua, "onChange");
		luabind::call_function<void>(callback, this);
	}
}

void MovableItem::onDragEnd(float mx,float my){
	selected = !selected;
	onChange();
}

MovableItem::MovableItem(float ax, float ay)
{
	HTEXTURE circle;
	circle = hge->Texture_Load("fire_glow_strip5.png");
	hgeAnimation *anim = new hgeAnimation(circle, 4, 12, 0, 0, 32, 32); //circle, 9, 3, 0, 0, 32, 32);
	anim->SetMode(HGEANIM_FWD | HGEANIM_LOOP);
	anim->Play();
	sprite = anim;
	x = ax; y = ay;
	selected = false;
}

MovableItem::MovableItem(hgeAnimation *s, float ax, float ay)
{
	assert(s);
	sprite = s;
	x = ax; y = ay;
	selected = false;
}

MovableItem::~MovableItem(void)
{
}

void MovableItem::Update(float dt){
	// sprite
	sprite->Update(dt);
	return;
}

void MovableItem::onDrag(float dx, float dy){
	x += dx;
	y += dy;
	onChange();
}

void MovableItem::Render(){
	if(sprite){
		sprite->Render(x, y);
		if(selected){
			hgeRect rect;
			sprite->GetBoundingBox(x, y, &rect);
			int d = 2;
			hge->Gfx_RenderLine(rect.x1 - d, rect.y1 - d, rect.x2 + d, rect.y1 - d);
			hge->Gfx_RenderLine(rect.x2 + d, rect.y1 - d, rect.x2 + d, rect.y2 + d);
			hge->Gfx_RenderLine(rect.x2 + d, rect.y2 + d, rect.x1 - d, rect.y2 + d);
			hge->Gfx_RenderLine(rect.x1 - d, rect.y2 + d, rect.x1 - d, rect.y1 - d);
		}
	}

}