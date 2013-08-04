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
	ScreenItem(CompositeItem* parent = 0);
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
		// TODO: GetParent() is me - so..?
		return entity->GetVar("size2d")->GetVector2().x;
	}
	float getHeight()	const {
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

protected:
	CompositeItem* parent_item;
	CL_Vec2f hp_pos;	// used when it changes
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
};

class CompositeItem: virtual public ScreenItem{
	friend class ScreenItem;
public:
	//TODO Add x abd y parameters to CompositeItem
	CompositeItem(CompositeItem* parent):ScreenItem(parent){}
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
public:
	static CompositeItem* getGlobalParent(){return global_parent;}
	static void setGlobalParent(CompositeItem* v){global_parent = v;}
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

	void OnPosChange(Variant* /*NULL*/){}

	// utilitary
	void takeCharFocus();
	void giveCharFocus();

	static CompositeItem* global_parent;

private:
	// no value semantic!
	SimpleItem& operator=(const SimpleItem&){assert(false);}
	SimpleItem(const SimpleItem&){assert(false);}
};

extern CompositeItem* root_item();