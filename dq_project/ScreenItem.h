#pragma once
#include "Visual.h"
#include "ScreenResource.h"
#include "WantFrameUpdate.h"
//!!! #include "UserInputDispatcher.h"

#include <limits>

class ScreenItem: public Visual
//!!!	,protected CharInputObject
{
public:
	static CompositeVisual* getParentVisual(){return parent_visual;}
	static void setParentVisual(CompositeVisual* v){parent_visual = v;}

//!!!protected: for a while:
public:
	ScreenItem(float x=0.0f, float y=0.0f);
	~ScreenItem();

	// polymorphic
	virtual void Render();
	virtual bool isPointIn(float mx, float my);

	// position
	void move(float dx, float dy){
		x += dx; y += dy;
	}
	void setX(float x){this->x = x;}
	void setY(float y){this->y = y;}
	float getX(){return x;}
	float getY(){return y;}

	// rotation
	void rotate(float r){rot += r;}
	void setRotation(float r){rot = r;}
	float getRotation(){return rot;}

	// dimensions and hot-spot
	void setHotSpotX(float x) {
		hpx = x;
	}
	void setHotSpotY(float y) {
		hpy = y;
	}

	float getWidth()	const {return view ? view->getWidth() : 0;}
	float getHeight()	const {return view ? view->getHeight() : 0;}
	float getHotSpotX()	const {
		return hpx;
	}
	float getHotSpotY()	const {
		return hpy;
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
	void setView(ScreenResource* r){
		view = r;
		if(view){
			setHotSpotX( getWidth() / 2  );
			setHotSpotY( getHeight() / 2 );
			// TODO how to remove component from Entity? (assign NULL?)
			view->assignEntity(entity);
		}
	}
	ScreenResource* getView(){
		return view;
	}

protected:
	Entity* entity;

	float x, y, rot;
	float hpx, hpy;
	ScreenResource* view;
	bool visible;

	// utilitary
	void takeCharFocus();
	void giveCharFocus();
	void compute_corner(int no, float &rx, float &ry) const {
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
};
