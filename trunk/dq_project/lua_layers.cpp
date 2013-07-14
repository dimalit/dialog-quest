#include "PlatformPrecomp.h"

#include "lua_layers.h"
#include "ScreenItem.h"
#include <vector>

namespace Layers{

struct layer{
	std::string name;
	CompositeItem* item;
};

CompositeItem* root_item(){
	static CompositeItem* root	= 0;
	if(!root){
		root = new CompositeItem();
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
	layer l = {name, new CompositeItem()};
	layers.push_back(l);
	l.item->setParent(root_item());
	SimpleItem::setGlobalParent(l.item);
}

void set_layer(std::string name){
	int li = find(name);
	assert(li >= 0);
	SimpleItem::setGlobalParent(layers[li].item);
}

int num_layers(){
	return layers.size();
}

ScreenItem* get_layer(int i){
	assert(i>=0 && i<layers.size());
	return layers[i].item;
}

void luabind(lua_State* L){
	luabind::module(L)
    [
		luabind::def("add_layer", &add_layer),
		luabind::def("set_layer", &set_layer)
    ];
}

}// namespace