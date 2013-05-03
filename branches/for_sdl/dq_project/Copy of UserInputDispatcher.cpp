#include <list>
#include "main.h"
#include "UserInputDispatcher.h"

UserInputDispatcher UserInputDispatcher::instance;
/*
MouseDragObject* UserInputDispatcher::find_over_client(float x, float y){

	for(std::set<MouseDragObject*>::iterator i = clients.begin();
		i != clients.end(); ++i)
	{
		MouseDragObject* c = *i;
		if(c->isPointIn(x, y))
			return c;
	}// for

	return 0;
}
*/
MouseDragObject::MouseDragObject(){
	UserInputDispatcher* source = UserInputDispatcher::getInstance();
	source->addClient(this);
}

MouseDragObject::~MouseDragObject(){
	UserInputDispatcher* source = UserInputDispatcher::getInstance();
	source->removeClient(this);
}

void UserInputDispatcher::onMouseMove(float x, float y){
	// drag mode
	if(drag_objects.size()){
		float dx = x - basic_state.mx;
		float dy = y - basic_state.my;

		for(std::set<MouseDragObject*>::iterator i = drag_objects.begin(); i != drag_objects.end(); i++){
			(*i)->onDrag(dx, dy);
		}
	}// drag mode
	// free move mode - send overs/outs
	else{
		
		// send overs and add to over_objects
		for(std::set<MouseDragObject*>::iterator i = clients.begin();
			i != clients.end(); ++i)
		{
			if((*i)->isPointIn(x, y) && over_objects.count(*i) == 0){
				(*i)->onMouseOver(x, y);
				over_objects.insert(*i);
			}
		}// for
		
		// send moves and outs
		std::list<MouseDragObject*> remove_them;
		for(std::set<MouseDragObject*>::iterator i = over_objects.begin();
			i != over_objects.end(); ++i)
		{
			if(!(*i)->isPointIn(x, y)){
				(*i)->onMouseOut();
				remove_them.push_back(*i);
			}
			else{
				(*i)->onMouseMove(x, y);
			}
		}

		// remove from over_objects
		for(std::list<MouseDragObject*>::iterator i = remove_them.begin();
			i != remove_them.end(); ++i)
		{
			over_objects.erase(*i);
		}

	}// if free move

	UserInputBasic::onMouseMove(x, y);
}
void UserInputDispatcher::onMouseDown(int btn){
	UserInputBasic::onMouseDown(btn);
	if(!over_objects.size() || btn > 0) return;

	for(std::set<MouseDragObject*>::iterator i = over_objects.begin();
		i != over_objects.end(); ++i)
	{
		MouseDragObject*  d = dynamic_cast<MouseDragObject*> (*i);

		// handle drag
		if(d){
			d->onDragStart(basic_state.mx, basic_state.my);
			drag_objects.insert(d);
		}
	}// for over_objects
}
void UserInputDispatcher::onMouseUp(int btn){
	UserInputBasic::onMouseUp(btn);

	// handle only drag end
	if(!drag_objects.size())
		return;
	for(std::set<MouseDragObject*>::iterator i = drag_objects.begin();
		i != drag_objects.end(); ++i)
	{
		(*i)->onDragEnd(basic_state.mx, basic_state.my);
	}
	drag_objects.clear();
}

void UserInputDispatcher::onDbClick(){

	float x = basic_state.mx;
	float y = basic_state.my;

	if(over_objects.size()){
		for(std::set<MouseDragObject*>::iterator i = over_objects.begin();
			i != over_objects.end(); ++i)
		{
			MouseDragObject* d = dynamic_cast<MouseDragObject*>(*i);
			if(d)
				d->onDbClick();
		}// for
	}

	UserInputBasic::onDbClick();
}

void UserInputDispatcher::onChar(int chr){
	luabind::object global_char_callback = luabind::globals(L)["global_char_callback"];
	if(global_char_callback){
		luabind::call_function<void>(global_char_callback, chr);
	}

	if(char_focus_target){
		char_focus_target->onChar(chr);
	}// if chio
}