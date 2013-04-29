#pragma once

#include <cassert>
#include <set>

class CompositeVisual;

// parent may be null
class Visual
{
public:
	Visual(CompositeVisual* parent = 0);
	virtual ~Visual();
	virtual void Render() = 0;
	void setParent(CompositeVisual* new_parent);
	CompositeVisual* getParent(){return parent_visual;}
protected:
	CompositeVisual* parent_visual;
};

class CompositeVisual: public Visual
{
public:
	CompositeVisual(CompositeVisual* parent = 0):Visual(parent){}
	virtual void Render() {renderChildren();}

private:
	std::set<Visual*> children;
	void renderChildren();

	friend class Visual;

	void addChild(Visual* w){
		assert(w && children.count(w)==0);
		children.insert(w);
	}
	void removeChild(Visual* w){
		assert(w && children.count(w)!=0);
		children.erase(w);
	}
};