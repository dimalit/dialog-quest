#include "PlatformPrecomp.h"
#include "Visual.h"
#include <algorithm>

Visual::Visual(CompositeVisual* parent)
	:parent_visual(parent)
{
	if(parent_visual)
		parent_visual->addChild(this);
}

Visual::~Visual(void)
{
	if(parent_visual)
		parent_visual->removeChild(this);
}

void Visual::setParent(CompositeVisual* new_parent){
	if(parent_visual)
		parent_visual->removeChild(this);
	parent_visual = new_parent;
	if(parent_visual)
		parent_visual->addChild(this);
}

void CompositeVisual::renderChildren(){
	class CallRender{
	public:
		void operator()(Visual* w){
			w->Render();
		}
	};
	std::for_each(children.begin(), children.end(), CallRender());
}