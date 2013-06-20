#pragma once
#include "Visual.h"
#include "WantFrameUpdate.h"
#include "Entity/Component.h"
#include "UserInputDispatcher.h"

#include <limits>

class ScreenItem: public Visual
	 ,protected CharInputObject
{
public:
	static CompositeVisual* getParentVisual(){return parent_visual;}
	static void setParentVisual(CompositeVisual* v){parent_visual = v;}

//!!!protected: for a while:
public:
	ScreenItem(float x=0.0f, float y=0.0f);
	~ScreenItem();

	// polymorphic
//	virtual void Render();
	virtual bool isPointIn(float mx, float my);

	// position
	void move(float dx, float dy){
		entity->GetVar("pos2d")->GetVector2() += CL_Vec2f(dx, dy);
		hp_pos += CL_Vec2f(dx, dy);
	}
	// my hotSpot is also rotation center. Proton's - not.
	void setX(float x){
		CL_Vec2f v = entity->GetVar("pos2d")->GetVector2();
		v.x = x - getHotSpotX();
		entity->GetVar("pos2d")->Set(v);
		hp_pos.x = x;
	}
	void setY(float y){
		CL_Vec2f v = entity->GetVar("pos2d")->GetVector2();
		v.y = y - getHotSpotY();
		entity->GetVar("pos2d")->Set(v);
		hp_pos.y = y;
	}
	float getX() const {return entity->GetVar("pos2d")->GetVector2().x + getHotSpotX();}
	float getY() const {return entity->GetVar("pos2d")->GetVector2().y + getHotSpotY();}

	// rotation
	void rotate(float r){entity->GetVar("rotation")->GetFloat() += r * 180.0f / (float)M_PI;}
	void setRotation(float r){entity->GetVar("rotation")->Set(r * 180.0f / (float)M_PI);}
	float getRotation() const {return entity->GetVar("rotation")->GetFloat() / 180.0f * (float)M_PI;}

	// dimensions and hot-spot
	void setHotSpotX(float x) {
		// TODO: try to set by = and by Set() and see if it works!
		// TODO: will it work with rotation?
		float dx = x - getHotSpotX();
		entity->GetVar("pos2d")->GetVector2() += CL_Vec2f(-dx, 0);

		float xx = getWidth() > 0 ? x / getWidth() : 0;
		float yy = getHeight() > 0 ? getHotSpotY() / getHeight() : 0;
		entity->GetVar("rotationCenter")->Set(xx, yy);
	}
	void setHotSpotY(float y) {
		float dy = y - getHotSpotY();
		entity->GetVar("pos2d")->GetVector2() += CL_Vec2f(0, -dy);

		float xx = getWidth() > 0 ? getHotSpotX() / getWidth() : 0;
		float yy = getHeight() > 0 ? y / getHeight() : 0;

		entity->GetVar("rotationCenter")->Set(xx, yy);
	}

	float getWidth()	const {
		if(view == NULL)
			return 0;
		// TODO: GetParent() is me - so..?
		return entity->GetVar("size2d")->GetVector2().x;
	}
	float getHeight()	const {
		if(view == NULL)
			return 0;
		// TODO: GetParent() is me - so..?
		return entity->GetVar("size2d")->GetVector2().y;
	}
	float getHotSpotX()	const {
		return hp_pos.x - entity->GetVar("pos2d")->GetVector2().x;//entity->GetVar("rotationCenter")->GetVector2().x * getWidth();
	}
	float getHotSpotY()	const {
		return hp_pos.y - entity->GetVar("pos2d")->GetVector2().y;//entity->GetVar("rotationCenter")->GetVector2().y * getHeight();
}
	float getTop()		const {
		float res = std::numeric_limits<float>::infinity();
		for(int i=1; i<=4; i++){
			float rx, ry;
			compute_corner(i, rx, ry);
			if(ry < res)
				res = ry;
		}
		return res;
	}
	float getBottom()	const {
		float res = - std::numeric_limits<float>::infinity();
		for(int i=1; i<=4; i++){
			float rx, ry;
			compute_corner(i, rx, ry);
			if(ry > res)
				res = ry;
		}
		return res;
	}
	float getLeft()		const {
		float res = std::numeric_limits<float>::infinity();
		for(int i=1; i<=4; i++){
			float rx, ry;
			compute_corner(i, rx, ry);
			if(rx < res)
				res = rx;
		}
		return res;
	}
	float getRight()	const {
		float res = - std::numeric_limits<float>::infinity();
		for(int i=1; i<=4; i++){
			float rx, ry;
			compute_corner(i, rx, ry);
			if(rx > res)
				res = rx;
		}
		return res;
	}

	// aggregates
	void setView(EntityComponent* r){
		// remove old
		if(view)
			entity->RemoveComponentByAddress(view);
		// set new
		view = r;
		// connect component
		if(view){
			entity->AddComponent(view);
		}
		// accomodate its size
		OnSizeChange(NULL);

		// prepare Entity
//		entity->GetVar("pos2d")->Set(x, y);
//		entity->GetVar("rotation")->Set(rot /(float) M_PI * 180.0f);
//		entity->GetVar("rotationCenter")->Set(hpx / (float)getWidth(), hpy / (float)getHeight());

	}
	EntityComponent* getView(){
		return view;
	}

protected:
	EntityComponent* view;

	void OnSizeChange(Variant* /*NULL*/){
		float w = getWidth();
		float h = getHeight();
		setHotSpotX( w / 2  );
		setHotSpotY( h / 2 );
	}

	void OnPosChange(Variant* /*NULL*/);

	// utilitary
	void takeCharFocus();
	void giveCharFocus();
	void compute_corner(int no, float &rx, float &ry) const {

		// local params
		float x = getX(), y = getY();
		float hpx = getHotSpotX(), hpy = getHotSpotY();
		float rot = getRotation();

		switch(no){
		case 1:
			rx = - hpx*cos(rot) - hpy*sin(rot) + x;
			ry =   hpx*sin(rot) - hpy*cos(rot) + y;
			break;
		case 2:
			rx = - (hpx-getWidth())*cos(rot) - hpy*sin(rot) + x;
			ry =   (hpx-getWidth())*sin(rot) - hpy*cos(rot) + y;
			break;
		case 3:
			rx = - (hpx-getWidth())*cos(rot) - (hpy-getHeight())*sin(rot) + x;
			ry =   (hpx-getWidth())*sin(rot) - (hpy-getHeight())*cos(rot) + y;
			break;
		case 4:
			rx = - hpx*cos(rot) - (hpy-getHeight())*sin(rot) + x;
			ry =   hpx*sin(rot) - (hpy-getHeight())*cos(rot) + y;
			break;
		default:
			assert(false);
		}// sw
	}
	static CompositeVisual* parent_visual;

private:
	// no value semantic!
	ScreenItem& operator=(const ScreenItem&){assert(false);}
	ScreenItem(const ScreenItem&){assert(false);}
	CL_Vec2f hp_pos;	// used when it changes
};
