#include "PlatformPrecomp.h"
#include "ScreenItem.h"

ScreenItem::ScreenItem(int x, int y)
	:parent_item(NULL)
{
	orig_width = orig_height = 0;

	entity = new Entity("ScreenItem");
	if(parent_item)
		parent_item->add(this);

	setHotSpotRelativeX(0.5f);
	setHotSpotRelativeY(0.5f);
	setX(x); setY(y);

	entity->GetVar("size2d")->GetSigOnChanged()->connect(1, boost::bind(&SimpleItem::OnSizeChange, this, _1));
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
	v.x = x - getHotSpotX();
	if(parent_item)
		v.x + parent_item->getHotSpotX();
	entity->GetVar("pos2d")->Set(v);
}
void ScreenItem::setY(float y){
	CL_Vec2f v = entity->GetVar("pos2d")->GetVector2();
	v.y = y - getHotSpotY();
	if(parent_item)
		v.y + parent_item->getHotSpotY();
	entity->GetVar("pos2d")->Set(v);
}
float ScreenItem::getX() const {
	float x = entity->GetVar("pos2d")->GetVector2().x + getHotSpotX();
	if(parent_item)
		x -= parent_item->getHotSpotX();
	return  x;
}
float ScreenItem::getY() const {
	float y = entity->GetVar("pos2d")->GetVector2().y + getHotSpotY();
	if(parent_item)
		y -= parent_item->getHotSpotY();
	return  y;
}
float ScreenItem::getAbsoluteX() const {
	if(parent_item == 0)
		return getX();
	else
		return parent_item->getAbsoluteX() + getX();
}
float ScreenItem::getAbsoluteY() const {
	if(parent_item == 0)
		return getY();
	else
		return parent_item->getAbsoluteY() + getY();
}

void ScreenItem::setParent(CompositeItem* p){
	// need move myself
	float x1 = parent_item ? parent_item->getHotSpotX() : 0;
	float x2 = p		   ? p->getHotSpotX()			: 0;
	float y1 = parent_item ? parent_item->getHotSpotY() : 0;
	float y2 = p		   ? p->getHotSpotY()			: 0;
	move(x2-x1, y2-y1);
	parent_item = p;
}

SimpleItem::SimpleItem(float x, float y)
	:ScreenItem(x, y)
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
void SimpleItem::takeCharFocus(){
//!!!	UserInputDispatcher::getInstance()->setCharFocus(this);
}
void SimpleItem::giveCharFocus(){
//!!!	UserInputDispatcher::getInstance()->setCharFocus(0);
}
