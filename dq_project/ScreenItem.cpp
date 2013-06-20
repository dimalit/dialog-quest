#include "PlatformPrecomp.h"
#include "ScreenItem.h"

CompositeVisual* ScreenItem::parent_visual = 0;

ScreenItem::ScreenItem(float x, float y)
{
	view = 0;

	setX(x); setY(y);
//	this->visible = true;
	
	// add then init
	this->setParent(parent_visual);

	entity->GetVar("size2d")->GetSigOnChanged()->connect(1, boost::bind(&ScreenItem::OnSizeChange, this, _1));
	entity->GetVar("pos2d")->GetSigOnChanged()->connect(1, boost::bind(&ScreenItem::OnPosChange, this, _1));
}

ScreenItem::~ScreenItem(void)
{
}
#include "Text.h"
void ScreenItem::OnPosChange(Variant* /*NULL*/){
	int x;
	if(view && dynamic_cast<Text*>(view) && view->GetVar("text")->GetString().size()==3){
		x = 2;
	}
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
void ScreenItem::takeCharFocus(){
//!!!	UserInputDispatcher::getInstance()->setCharFocus(this);
}
void ScreenItem::giveCharFocus(){
//!!!	UserInputDispatcher::getInstance()->setCharFocus(0);
}
