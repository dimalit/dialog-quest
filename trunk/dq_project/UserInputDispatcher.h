#pragma once

#include <set>

#include "UserInputBasic.h"

class MouseInputObject{
public:
	MouseInputObject();
	~MouseInputObject();

	virtual bool isPointIn(float x,  float y) = 0;

	virtual void onMouseOver(){}
	virtual void onMouseOut(){}
	
	virtual void onDrag(float dx, float dy){}
	virtual void onDragStart(){}
	virtual void onDragEnd(){}

	virtual void onDbClick(){}
};

class CharInputObject: public MouseInputObject{
public:
	virtual void onChar(int chr){}
	virtual void onFocusLose(){}
};

class UserInputDispatcher: public RawInputObject
{
public:
	static UserInputDispatcher* getInstance(){return &instance;}

	// "raw" clients
	void addClient(RawInputObject* c){
		assert(c && raw_clients.count(c)==0);
		raw_clients.insert(c);
	}
	void removeClient(RawInputObject* c){
		assert(c && raw_clients.count(c)!=0);
		raw_clients.erase(c);

		// TODO: remove from overs and drags here too?
	}

	// "prepared" clients
	void addClient(MouseInputObject* c){
		assert(c && clients.count(c)==0);
		clients.insert(c);
	}
	void removeClient(MouseInputObject* c){
		assert(c && clients.count(c)!=0);
		clients.erase(c);

		if(over_objects.count(c) != 0)
			over_objects.erase(c);
		if(drag_objects.count(c) != 0)
			drag_objects.erase(c);
	}



	void setCharFocus(CharInputObject* chio){
		if(char_focus_target == chio) return;

		// lose
		if(char_focus_target)
			char_focus_target->onFocusLose();
		
		// gain
		char_focus_target = chio;
	}
protected:
	UserInputDispatcher(){
		char_focus_target = 0;
		UserInputBasic::getInstance()->setClient(this);
	}

	typedef std::set<MouseInputObject*> ClientsSet;
	typedef std::set<RawInputObject*> RawClientsSet;
	ClientsSet over_objects;
	ClientsSet drag_objects;

	virtual void onMouseMove(float x, float y, input_state state);
	virtual void onMouseDown(int btn, input_state state);
	virtual void onMouseUp(int btn, input_state state);
	virtual void onDbClick(input_state state);
	virtual void onChar(int chr, input_state state);

private:
	// mouse input consumers
	ClientsSet clients;
	RawClientsSet raw_clients;
	// keyboard focus is on it
	CharInputObject* char_focus_target;

	static UserInputDispatcher instance;
	UserInputDispatcher(const UserInputDispatcher&){}
	UserInputDispatcher* operator=(const UserInputDispatcher*){}
};
