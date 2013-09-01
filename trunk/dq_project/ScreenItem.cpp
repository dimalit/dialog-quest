#include "PlatformPrecomp.h"
#include "ScreenItem.h"

ScreenItem::ScreenItem()
	:parent_item(NULL)
{
	orig_width = orig_height = 0;

	entity = new Entity("ScreenItem");
	setVisible(1);

	setHotSpotRelativeX(0.5f);
	setHotSpotRelativeY(0.5f);
	setX(0); setY(0);

	entity->GetVar("size2d")->GetSigOnChanged()->connect(1, boost::bind(&ScreenItem::OnSizeChange, this, _1));
	entity->GetVar("pos2d")->GetSigOnChanged()->connect(1, boost::bind(&ScreenItem::onMove, this, _1));
}

ScreenItem::~ScreenItem(void)
{
	if(parent_item)
		parent_item->remove(this);
	delete entity;
}

Entity* ScreenItem::acquireEntity(Entity* e){
	assert(!parent_item && e);
	delete entity;
	entity = e;
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

// very unsafe and is called only from parent
void ScreenItem::setParent(CompositeItem* p){
	parent_item = p;
}

void ScreenItem::OnSizeChange(Variant* /*NULL*/){
	// move corner
	CL_Vec2f new_size = entity->GetVar("size2d")->GetVector2();
	float dx = getHotSpotRelativeX() * (new_size.x - orig_width);
	float dy = getHotSpotRelativeY() * (new_size.y - orig_height);

	orig_width = new_size.x;
	orig_height = new_size.y;

	// call onMove even if dx=dy=0
	CL_Vec2f pos = entity->GetVar("pos2d")->GetVector2();
	entity->GetVar("pos2d")->Set(pos.x-dx, pos.y-dy);

	if(getParent())
		getParent()->requestLayOut(this);
}

void ScreenItem::onMove(Variant* /*NULL*/){
	// nothing
	// will be handled in Lua
}

CompositeItem::CompositeItem():ScreenItem(){
	// we need to render visibility for children
	entity->OnFilterAdd();
	entity->GetFunction("FilterOnRender")->sig_function.connect(boost::bind(&CompositeItem::FilterOnRender, this, _1));
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
