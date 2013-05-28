#include "PlatformPrecomp.h"

#include "lua_layers.h"
#include "ScreenItem.h"
#include <vector>

namespace Layers{

struct layer{
	std::string name;
	CompositeVisual* visual;
};

CompositeVisual* root_visual(){
	static CompositeVisual* root	= 0;
	if(!root){
		root = new CompositeVisual();
		Entity* e = new Entity("root");
		AddFocusIfNeeded(e);
		root->acquireEntity(e);
	}
	return root;
}
std::vector<layer> layers;

int find(std::string name){
	for(int i=0; i<layers.size(); i++)
	{	
		if(layers[i].name == name) return i;
	}
	return -1;
}

void add_layer(std::string name){
	layer l = {name, new CompositeVisual()};
	layers.push_back(l);
	l.visual->setParent(root_visual());
	ScreenItem::setParentVisual(l.visual);
}

void set_layer(std::string name){
	int li = find(name);
	assert(li >= 0);
	ScreenItem::setParentVisual(layers[li].visual);
}

int num_layers(){
	return layers.size();
}

Visual* get_layer(int i){
	assert(i>=0 && i<layers.size());
	return layers[i].visual;
}

void luabind(lua_State* L){
	luabind::module(L)
    [
		luabind::def("add_layer", &add_layer),
		luabind::def("set_layer", &set_layer)
    ];
}

}// namespace