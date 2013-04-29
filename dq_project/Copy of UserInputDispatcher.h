#pragma once

#include <set>

#include "UserInputBasic.h"

class MouseDragObject{
public:
	MouseDragObject();
	~MouseDragObject();

	virtual bool isPointIn(float x,  float y) = 0;

	virtual void onMouseOver(float x, float y){}
	virtual void onMouseOut(){}
	virtual void onMouseMove(float mx, float my){}

	virtual void onDrag(float dx, float dy){}
	virtual void onDragStart(float x, float y){}
	virtual void onDragEnd(float x, float y){}

	virtual void onDbClick(){}
};

class CharInputObject: public MouseDragObject{
public:
	virtual void onChar(int chr){}
	virtual void onFocusLose(){}
};

class UserInputDispatcher: public UserInputBasic
{
public:
	static UserInputDispatcher* getInstance(){return &instance;}

	void addClient(MouseDragObject* c){
		assert(c && clients.count(c)==0);
		clients.insert(c);
	}
	void removeClient(MouseDragObject* c){
		assert(c && clients.count(c)!=0);
		clients.erase(c);
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
	UserInputDispatcher(){ char_focus_target = 0; }

	typedef std::set<MouseDragObject*> ClientsSet;
	ClientsSet over_objects;
	ClientsSet drag_objects;

	virtual void onMouseMove(float x, float y);
	virtual void onMouseDown(int btn);
	virtual void onMouseUp(int btn);
	virtual void onDbClick();
	virtual void onChar(int chr);

private:
	// mouse input consumers
	ClientsSet clients;
	// keyboard focus is on it
	CharInputObject* char_focus_target;

	static UserInputDispatcher instance;
	UserInputDispatcher(const UserInputDispatcher&){}
	UserInputDispatcher* operator=(const UserInputDispatcher*){}
};
