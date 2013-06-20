#pragma once

#include "Entity/Entity.h"
#include <cassert>
#include <set>

class CompositeVisual;

// parent may be null
class Visual
{
public:
	Visual(CompositeVisual* parent = 0);
	Entity* acquireEntity(Entity* e);
	virtual ~Visual();
//	virtual void Render() = 0;
	void setParent(CompositeVisual* new_parent);
	CompositeVisual* getParent(){return parent_visual;}
protected:
	CompositeVisual* parent_visual;
public: //!!! temporary
	Entity* entity;
	friend class CompositeVisual;
};

class CompositeVisual: public Visual
{
public:
	CompositeVisual(CompositeVisual* parent = 0):Visual(parent){}
//	virtual void Render() {renderChildren();}

private:
	std::set<Visual*> children;
//	void renderChildren();

	friend class Visual;

	void addChild(Visual* w){
		assert(w && children.count(w)==0);
		entity->AddEntity(w->entity);
		children.insert(w);
	}
	void removeChild(Visual* w){
		assert(w && children.count(w)!=0);
		entity->RemoveEntityByAddress(w->entity, false);	// don't delete entity
		children.erase(w);
	}
};