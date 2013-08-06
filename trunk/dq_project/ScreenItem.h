#pragma once

#include "WantFrameUpdate.h"
#include "Entity/Component.h"
#include "UserInputDispatcher.h"

#include "Entity/Entity.h"

#include <cassert>
#include <set>
#include <limits>

class CompositeItem;

class ScreenItem: protected CharInputObject
{
public:
	// entity
	Entity* acquireEntity(Entity* e);

	// ctor/dtor
	ScreenItem(CompositeItem* parent = 0, int x = 0, int y = 0);
	virtual ~ScreenItem();

	// parent/child relations
	void setParent(CompositeItem* new_parent);
	CompositeItem* getParent(){return parent_item;}

	// geometry
	// polymorphic
	virtual bool isPointIn(float mx, float my);

	// position
	void move(float dx, float dy){
		entity->GetVar("pos2d")->GetVector2() += CL_Vec2f(dx, dy);
	}
	// my hotSpot is also rotation center. Proton's - not.
	void setX(float x){
		CL_Vec2f v = entity->GetVar("pos2d")->GetVector2();
		v.x = x - getHotSpotX();
		entity->GetVar("pos2d")->Set(v);
	}
	void setY(float y){
		CL_Vec2f v = entity->GetVar("pos2d")->GetVector2();
		v.y = y - getHotSpotY();
		entity->GetVar("pos2d")->Set(v);
	}
	float getX() const {return entity->GetVar("pos2d")->GetVector2().x + getHotSpotX();}
	float getY() const {return entity->GetVar("pos2d")->GetVector2().y + getHotSpotY();}

	// rotation
	void rotate(float r){entity->GetVar("rotation")->GetFloat() += r * 180.0f / (float)M_PI;}
	void setRotation(float r){entity->GetVar("rotation")->Set(r * 180.0f / (float)M_PI);}
	float getRotation() const {return entity->GetVar("rotation")->GetFloat() / 180.0f * (float)M_PI;}

	// dimensions and hot-spot
	void setHotSpotRelativeX(float x) {
		// TODO: try to set by = and by Set() and see if it works!
		// TODO: will it work with rotation?
		CL_Vec2f v = entity->GetVar("rotationCenter")->GetVector2();
		float dx = getWidth() * (x - v.x);
		v.x = x;
		entity->GetVar("rotationCenter")->Set(v);
		move(-dx, 0);
	}
	void setHotSpotRelativeY(float y) {
		CL_Vec2f v = entity->GetVar("rotationCenter")->GetVector2();
		float dy = getWidth() * (y - v.y);
		v.y = y;
		entity->GetVar("rotationCenter")->Set(v);
		move(0, -dy);
	}
	float getHotSpotRelativeX() const{
		return entity->GetVar("rotationCenter")->GetVector2().x;
	}
	float getHotSpotRelativeY() const{
		return entity->GetVar("rotationCenter")->GetVector2().y;
	}
	float getWidth()	const {
		return entity->GetVar("size2d")->GetVector2().x;
	}
	float getHeight()	const {
		return entity->GetVar("size2d")->GetVector2().y;
	}
	void setWidth(float w){
		CL_Vec2f v = entity->GetVar("size2d")->GetVector2();
		v.x = w;
		entity->GetVar("size2d")->Set(v);
		// note: pos is automatically adjusted in OnResize
	}
	void setHeight(float h){
		// then grow it
		CL_Vec2f v = entity->GetVar("size2d")->GetVector2();
		v.y = h;
		entity->GetVar("size2d")->Set(v);
	}
	float getHotSpotX()	const {
		return entity->GetVar("rotationCenter")->GetVector2().x * getWidth();
	}
	float getHotSpotY()	const {
		return entity->GetVar("rotationCenter")->GetVector2().y * getHeight();
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

protected:
	CompositeItem* parent_item;
public: //!!! temporary
	Entity* entity;
protected:
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
	void OnSizeChange(Variant* /*NULL*/){
		// move corner
		CL_Vec2f new_size = entity->GetVar("size2d")->GetVector2();
		float dx = getHotSpotRelativeX() * (new_size.x - orig_width);
		float dy = getHotSpotRelativeY() * (new_size.y - orig_height);
		move(-dx, -dy);
		
		orig_width = new_size.x;
		orig_height = new_size.y;
	}
private:
	float orig_width, orig_height;
};

class CompositeItem: virtual public ScreenItem{
	friend class ScreenItem;
public:
	//TODO Add x abd y parameters to CompositeItem
	CompositeItem(CompositeItem* parent, int x=0, int y=0):ScreenItem(parent,x,y)
	{
	}
	virtual ~CompositeItem(){
		// TODO: What should we do here with our children? They will have inexisting parent!
	}

private:
	void addChild(ScreenItem* w){
		assert(w && children.count(w)==0);
		entity->AddEntity(w->entity);
		children.insert(w);
	}
	void removeChild(ScreenItem* w){
		assert(w && children.count(w)!=0);
		entity->RemoveEntityByAddress(w->entity, false);	// don't delete entity
		children.erase(w);
	}

	std::set<ScreenItem*> children;
};

// TODO: inheritance of CharInputObject should be in ScreenItem!
class SimpleItem: virtual public ScreenItem
{
//!!!protected: for a while:
public:
	SimpleItem(CompositeItem* prent, float x=0.0f, float y=0.0f);
	~SimpleItem();

	// aggregates
	void setView(EntityComponent* r){
		// remove old
		if(view)
			entity->RemoveComponentByAddress(view);
		// set new
		view = r;
		// connect component
		float w = getWidth();
		float h = getHeight();
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
	// utilitary
	void takeCharFocus();
	void giveCharFocus();

private:
	// no value semantic!
	SimpleItem& operator=(const SimpleItem&){assert(false);}
	SimpleItem(const SimpleItem&){assert(false);}
};

extern CompositeItem* root_item();