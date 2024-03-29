#pragma once

#include "WantFrameUpdate.h"
#include "Entity/Component.h"
#include "UserInputDispatcher.h"

#include "Entity/Entity.h"

#include <cassert>
#include <set>
#include <limits>

class ScreenItem: protected CharInputObject
{
friend class CompositeItem;
public:
	// entity
	Entity* acquireEntity(Entity* e);

	// ctor/dtor
	ScreenItem();
	virtual ~ScreenItem();

	// parent/child relations
	CompositeItem* getParent() const {return parent_item;}

	bool getVisible() const{
		return entity->GetVar("visible")->GetUINT32();
	}
	void setVisible(bool v){
		entity->GetVar("visible")->Set(uint32(v));
	}
	bool getReallyVisible() const;

	// geometry
	// polymorphic
	virtual bool isPointIn(float mx, float my);
	virtual void setWidth(float w){
		assert(_finite(w));
		CL_Vec2f v = entity->GetVar("size2d")->GetVector2();
		// prevent parent requestLayOut
		if(v.x != w){
			v.x = w;
			entity->GetVar("size2d")->Set(v);
		}
		// note: pos is automatically adjusted in OnResize
	}
	virtual void setHeight(float h){
		assert(_finite(h));
		// then grow it
		CL_Vec2f v = entity->GetVar("size2d")->GetVector2();
		// prevent parent requestLayOut
		if(v.y != h){
			v.y = h;
			entity->GetVar("size2d")->Set(v);
		}
	}

	
	// position
	void move(float dx, float dy){
		if(dx != 0.0f || dy != 0.0f){
			CL_Vec2f pos = entity->GetVar("pos2d")->GetVector2();
			entity->GetVar("pos2d")->Set(pos.x+dx, pos.y+dy);
		}
	}
	float getX() const;
	float getY() const;
	void setX(float x);
	void setY(float y);
	float getAbsoluteX() const;
	float getAbsoluteY() const;
	void setAbsoluteX(float gx);
	void setAbsoluteY(float gy);

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
		float dy = getHeight() * (y - v.y);
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
	float getAbsoluteLeft() const;
	float getAbsoluteRight() const;
	float getAbsoluteTop() const;
	float getAbsoluteBottom() const;

	bool getDebugDrawBox() const;
	void setDebugDrawBox(bool v){
		entity->GetVar("debugDrawBox")->Set(uint32(v));
	}
	uint32 getDebugDrawColor() const{
		return entity->GetVar("debugDrawColor")->GetUINT32();
	}
	void setDebugDrawColor(uint32 c){
		entity->GetVar("debugDrawColor")->Set(uint32(c));
	}

protected:
	void setParent(CompositeItem* p);
	// utilitary
	void takeCharFocus();
	void giveCharFocus();
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

	virtual void OnSizeChange(Variant* /*NULL*/);
	virtual void onMove(Variant* /*NULL*/);
private:
	uint32* debug_draw_box;
	uint32* debug_draw_color;
	float orig_width, orig_height;
	CompositeItem* parent_item;
};

class CompositeItem: virtual public ScreenItem{
	friend class ScreenItem;
	friend class LuaCompositeItem;
public:
	CompositeItem();
	virtual ~CompositeItem(){
		std::set<ScreenItem*>::iterator it;
		for(it=children.begin(); it != children.end(); it++){
			delete *it;		// it will disconnect from me itself
		}// for
	}
	CompositeItem* add(ScreenItem* w){
		assert(w && children.count(w)==0);
		assert(w->parent_item == NULL);
		w->setParent(this);
		entity->AddEntity(w->entity);
		children.insert(w);

		requestLayOut();
		return this;
	}
	CompositeItem* remove(ScreenItem* w){
		assert(w && children.count(w)!=0);
		entity->RemoveEntityByAddress(w->entity, false);	// don't delete entity
		children.erase(w);
		w->setParent(NULL);

		requestLayOut();
		return this;
	}
	// called by children when they change their size
	// binded to Lua
	void requestLayOut();

	void _specialEntryForRenderSignal(){
		assert(!global_layout_mode);
		global_layout_mode = true;
		adjustLayout();
		global_layout_mode = false;
	}

protected:
	// called by children if they want to get adjustLayout
	// not binded to Lua
	void requestLayOutChildren();

	virtual void adjustLayout();

	bool need_lay_out;
	bool need_lay_out_children;
	static bool global_layout_mode;			// it's time to do scheduled lay-out

	std::set<ScreenItem*> children;

private:
	// used to prevent invisible Entities from rendering
	void FilterOnRender(VariantList* pVList){
		if(!entity->GetVarWithDefault("visible", uint32(1))->GetUINT32())
			pVList->m_variant[Entity::FILTER_INDEX].Set(uint32(Entity::FILTER_REFUSE_ALL));
	}
	// change MY size!
	virtual void OnSizeChange(Variant* v){
		requestLayOut();				// maybe change another dimension?
		ScreenItem::OnSizeChange(v);
	}
};

extern CompositeItem* root_item();