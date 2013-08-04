#include "PlatformPrecomp.h"
#include "ScreenItem.h"

ScreenItem::ScreenItem(CompositeItem* parent)
	:parent_item(parent)
{
	entity = new Entity();
	if(parent_item)
		parent_item->addChild(this);
}

ScreenItem::~ScreenItem(void)
{
	if(parent_item)
		parent_item->removeChild(this);
	delete entity;
}

Entity* ScreenItem::acquireEntity(Entity* e){
	assert(!parent_item && e);
	delete entity;
	entity = e;
	return entity;
}

void ScreenItem::setParent(CompositeItem* new_parent){
	if(parent_item)
		parent_item->removeChild(this);
	parent_item = new_parent;
	if(parent_item)
		parent_item->addChild(this);
}

SimpleItem::SimpleItem(CompositeItem* parent, float x, float y)
	:ScreenItem(parent)
{
	view = 0;

	setX(x); setY(y);
//	this->visible = true;
	
	entity->GetVar("size2d")->GetSigOnChanged()->connect(1, boost::bind(&SimpleItem::OnSizeChange, this, _1));
	entity->GetVar("pos2d")->GetSigOnChanged()->connect(1, boost::bind(&SimpleItem::OnPosChange, this, _1));
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
	float x = getX(), y = getY();
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
