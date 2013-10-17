#include "PlatformPrecomp.h"
#include "ScreenItem.h"

#include <functional>

void draw_item_rect(ScreenItem* item, VariantList*){
	if(item->getDebugDrawBox())
		DrawRect(item->getAbsoluteX() - item->getHotSpotX(), item->getAbsoluteY() - item->getHotSpotY(), item->getWidth(), item->getHeight());
}

bool ScreenItem::getDebugDrawBox() const{
	return *debug_draw_box || (getParent() && getParent()->getDebugDrawBox());
}

ScreenItem::ScreenItem()
	:parent_item(NULL)
{
	orig_width = orig_height = 0.0f;

	entity = new Entity("ScreenItem");
	setVisible(1);

	setWidth(20.0f); setHeight(20.0f);			// for convergence
	orig_width = orig_height = 20.0f;			// no handler yet!!

	setHotSpotRelativeX(0.5f);
	setHotSpotRelativeY(0.5f);
	setX(0); setY(0);

	debug_draw_box = &entity->GetVarWithDefault("debugDrawBox", uint32(0))->GetUINT32();

	entity->GetVar("size2d")->GetSigOnChanged()->connect(1, boost::bind(&ScreenItem::OnSizeChange, this, _1));
	entity->GetVar("pos2d")->GetSigOnChanged()->connect(1, boost::bind(&ScreenItem::onMove, this, _1));
	entity->GetFunction("OnRender")->sig_function.connect(1, boost::bind(&draw_item_rect, this, _1));
	//entity->GetFunction("OnRender")->sig_function.connect(0, my_bind<void (*)(ScreenItem*), ScreenItem*>(&draw_item_rect, this));
}

ScreenItem::~ScreenItem(void)
{
	if(parent_item)
		parent_item->remove(this);
	delete entity;
}

Entity* ScreenItem::acquireEntity(Entity* e){
	assert(!parent_item && e);
	// take with us our properties
	if(entity!=0 && e!=0){
		entity->GetShared()->ResetNext();
		string key;
		Variant* v = entity->GetShared()->GetNext(key);
		while(v){
			e->GetShared()->GetVar(key)->Set(*v);
			v = entity->GetShared()->GetNext(key);		
		}
	}// if
	delete entity;
	entity = e;

	// rebind variables!!!
	debug_draw_box = &entity->GetVarWithDefault("debugDrawBox", uint32(0))->GetUINT32();

	return entity;
}

// my hotSpot is also rotation center. Proton's - not.
void ScreenItem::setX(float x){
	CL_Vec2f v = entity->GetVar("pos2d")->GetVector2();
	if(v.x != x - getHotSpotX()){
		v.x = x - getHotSpotX();
		entity->GetVar("pos2d")->Set(v);
	}
}
void ScreenItem::setY(float y){
	CL_Vec2f v = entity->GetVar("pos2d")->GetVector2();
	if(v.y != y - getHotSpotY()){
		v.y = y - getHotSpotY();
		entity->GetVar("pos2d")->Set(v);
	}
}
float ScreenItem::getX() const {
	float x = entity->GetVar("pos2d")->GetVector2().x + getHotSpotX();
	return  x;
}
float ScreenItem::getY() const {
	float y = entity->GetVar("pos2d")->GetVector2().y + getHotSpotY();
	return  y;
}
float ScreenItem::getAbsoluteX() const {
	if(parent_item == 0)
		return getX();
	else
		return parent_item->getAbsoluteX() - parent_item->getHotSpotX() + getX();
}
float ScreenItem::getAbsoluteY() const {
	if(parent_item == 0)
		return getY();
	else
		return parent_item->getAbsoluteY() - parent_item->getHotSpotY()  + getY();
}

void ScreenItem::setAbsoluteX(float gx){
	if(parent_item == 0)
		setX(gx);
	else{
		float parent_gleft = parent_item->getAbsoluteX() - parent_item->getHotSpotX();
		setX(gx - parent_gleft);
	}
}
void ScreenItem::setAbsoluteY(float gy){
	if(parent_item == 0)
		setY(gy);
	else{
		float parent_gtop = parent_item->getAbsoluteY() - parent_item->getHotSpotY();
		setY(gy - parent_gtop);
	}
}

float ScreenItem::getAbsoluteLeft() const {
	if(!parent_item)
		return getLeft();
	return parent_item->getAbsoluteLeft() + getLeft();
}
float ScreenItem::getAbsoluteRight() const {
	if(!parent_item)
		return getRight();
	return parent_item->getAbsoluteLeft() + getRight();
}
float ScreenItem::getAbsoluteTop() const {
	if(!parent_item)
		return getTop();
	return parent_item->getAbsoluteTop() + getTop();
}
float ScreenItem::getAbsoluteBottom() const {
	if(!parent_item)
		return getBottom();
	return parent_item->getAbsoluteTop() + getBottom();
}

bool ScreenItem::getReallyVisible() const {
		bool p = true;
		if(getParent())
			p = getParent()->getReallyVisible();
		return p && getVisible();
}

// very unsafe and is called only from parent
void ScreenItem::setParent(CompositeItem* p){
	parent_item = p;
}

void ScreenItem::OnSizeChange(Variant* /*NULL*/){
	CL_Vec2f new_size = entity->GetVar("size2d")->GetVector2();

	// skip if the same
	if(orig_width == new_size.x && orig_height == new_size.y)
		return;

	// move corner
	float dx = getHotSpotRelativeX() * (new_size.x - orig_width);
	float dy = getHotSpotRelativeY() * (new_size.y - orig_height);

	orig_width = new_size.x;
	orig_height = new_size.y;

	// call onMove even if dx=dy=0
	CL_Vec2f pos = entity->GetVar("pos2d")->GetVector2();
	entity->GetVar("pos2d")->Set(pos.x-dx, pos.y-dy);

	if(getParent())
		getParent()->requestLayOut();
}

void ScreenItem::onMove(Variant* /*NULL*/){
	// nothing
	// will be handled in Lua
}

CompositeItem::CompositeItem():ScreenItem(){
	need_lay_out = true;
	need_lay_out_children = true;
	// we need to render visibility for children
	entity->OnFilterAdd();
	entity->GetFunction("FilterOnRender")->sig_function.connect(boost::bind(&CompositeItem::FilterOnRender, this, _1));
}

void CompositeItem::doLayOutIfNeeded(){
	lay_out_children();
	need_lay_out = false;
}

SimpleItem::SimpleItem()
	:ScreenItem()
{
	view = 0;
//	this->visible = true;
}

SimpleItem::~SimpleItem(void)
{
}

//void ScreenItem::Render(){
//	if(visible && view){
//
//
//		// do it Proton way:
//		VariantList vl(Variant(0,0));
//		entity->GetFunction("OnRender")->sig_function(&vl);;
//		return;
//
//		// compute corner
//		float x1, y1;
//		compute_corner(1, x1, y1);
////!!!		view->Render(x1, y1, rot);
//
//		return;
//
//		// compute other corners
//		float x2, y2;
//		compute_corner(2, x2, y2);
//
//		float x3, y3;
//		compute_corner(3, x3, y3);
//
//		float x4, y4;
//		compute_corner(4, x4, y4);
//
//// debug box
////		unsigned long c = 0xFFFF0000;
////		hge->Gfx_RenderLine(x1, y1, x2, y2, c);
////		hge->Gfx_RenderLine(x2, y2, x3, y3, c);
////		hge->Gfx_RenderLine(x3, y3, x4, y4, c);
////		hge->Gfx_RenderLine(x4, y4, x1, y1, c);
//
//	}
//}

bool ScreenItem::isPointIn(float mx, float my){
	// local params
	float x = getAbsoluteX(), y = getAbsoluteY();
	float hpx = getHotSpotX(), hpy = getHotSpotY();
	float rot = getRotation();

	// move and rotate mx, my - make local
	float lx = (mx - x)*cos(rot)+(my-y)*sin(rot) + hpx;
	float ly = (mx - x)*sin(rot)+(my-y)*cos(rot) + hpy;

	// check bounds
	return (lx >= 0 && lx <= getWidth() && ly >= 0 && ly <= getHeight());
}

// for internal use only
void ScreenItem::takeCharFocus(){
//!!!	UserInputDispatcher::getInstance()->setCharFocus(this);
}
void ScreenItem::giveCharFocus(){
//!!!	UserInputDispatcher::getInstance()->setCharFocus(0);
}
