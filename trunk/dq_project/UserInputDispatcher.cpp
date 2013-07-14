#include "PlatformPrecomp.h"

#include "UserInputDispatcher.h"
#include "App.h"
#include <list>
//#include "main.h"

UserInputDispatcher* UserInputDispatcher::instance;
/*
MouseInputObject* UserInputDispatcher::find_over_client(float x, float y){

	for(std::set<MouseInputObject*>::iterator i = clients.begin();
		i != clients.end(); ++i)
	{
		MouseInputObject* c = *i;
		if(c->isPointIn(x, y))
			return c;
	}// for

	return 0;
}
*/
MouseInputObject::MouseInputObject(){
	UserInputDispatcher* source = UserInputDispatcher::getInstance();
	source->addClient(this);
}

MouseInputObject::~MouseInputObject(){
	UserInputDispatcher* source = UserInputDispatcher::getInstance();
	source->removeClient(this);
}

void UserInputDispatcher::onMouseMove(float x, float y, input_state state){
	// raw
	for(RawClientsSet::iterator it = raw_clients.begin(); it != raw_clients.end(); ++it)
		(*it)->onMouseMove(x, y, state);

	// drag mode
	if(drag_objects.size()){
		float dx = x - state.mx;
		float dy = y - state.my;

		for(std::set<MouseInputObject*>::iterator i = drag_objects.begin(); i != drag_objects.end(); i++){
			(*i)->onDrag(dx, dy);
		}
	}// drag mode
	// free move mode - send overs/outs
	else{
		
		// send overs and add to over_objects
		for(std::set<MouseInputObject*>::iterator i = clients.begin();
			i != clients.end(); ++i)
		{
			if((*i)->isPointIn(x, y) && over_objects.count(*i) == 0){
				(*i)->onMouseOver();
				over_objects.insert(*i);
			}
		}// for
		
		// send moves and outs
		std::list<MouseInputObject*> remove_them;
		for(std::set<MouseInputObject*>::iterator i = over_objects.begin();
			i != over_objects.end(); ++i)
		{
			if(!(*i)->isPointIn(x, y)){
				(*i)->onMouseOut();
				remove_them.push_back(*i);
			}
//			else{
//				(*i)->onMouseMove(x, y);
//			}
		}
		// remove from over_objects
		for(std::list<MouseInputObject*>::iterator i = remove_them.begin();
			i != remove_them.end(); ++i)
		{
			over_objects.erase(*i);
		}

	}// if free move
}
void UserInputDispatcher::onMouseDown(int btn, input_state state){
	// raw
	for(RawClientsSet::iterator it = raw_clients.begin(); it != raw_clients.end(); ++it)
		(*it)->onMouseDown(btn, state);

	if(!over_objects.size() || btn > 0) return;

	for(std::set<MouseInputObject*>::iterator i = over_objects.begin();
		i != over_objects.end(); ++i)
	{
		MouseInputObject*  d = dynamic_cast<MouseInputObject*> (*i);

		// handle drag
		if(d){
			d->onDragStart();
			drag_objects.insert(d);
//			std::cout << "added " << d << " to drags\n";
		}
	}// for over_objects
}
void UserInputDispatcher::onMouseUp(int btn, input_state state){
	// raw
	for(RawClientsSet::iterator it = raw_clients.begin(); it != raw_clients.end(); ++it)
		(*it)->onMouseUp(btn, state);

	// handle only drag end
	if(!drag_objects.size())
		return;
	for(std::set<MouseInputObject*>::iterator i = drag_objects.begin();
		i != drag_objects.end(); ++i)
	{
		(*i)->onDragEnd();
	}
	drag_objects.clear();
//	std::cout << "removed all drags\n";
}

void UserInputDispatcher::onDbClick(input_state state){
	// raw
	for(RawClientsSet::iterator it = raw_clients.begin(); it != raw_clients.end(); ++it)
		(*it)->onDbClick(state);

	float x = state.mx;
	float y = state.my;

	if(over_objects.size()){
		for(std::set<MouseInputObject*>::iterator i = over_objects.begin();
			i != over_objects.end(); ++i)
		{
			MouseInputObject* d = dynamic_cast<MouseInputObject*>(*i);
			if(d)
				d->onDbClick();
		}// for
	}
}

void UserInputDispatcher::onChar(int chr, input_state state){
	// raw
	for(RawClientsSet::iterator it = raw_clients.begin(); it != raw_clients.end(); ++it)
		(*it)->onChar(chr, state);

	luabind::object global_char_callback = luabind::globals(L)["global_char_callback"];
	if(global_char_callback){
		luabind::call_function<void>(global_char_callback, chr);
	}

	if(char_focus_target){
		char_focus_target->onChar(chr);
	}// if chio
}