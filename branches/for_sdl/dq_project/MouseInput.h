#pragma once

#include <cassert>
#include <set>

#include <hgeRect.h>

#include "WantFrameUpdate.h"

class WantMouseInput; 

// singleton that manages mouse input
class MouseInput: WantFrameUpdate
{
public:
	static MouseInput* getInstance(){return &theInput;}

	void addClient(WantMouseInput* c){
		assert(c && clients.count(c)==0);
		clients.insert(c);
	}

	void removeClient(WantMouseInput* c){
		assert(c && clients.count(c)!=0);
		clients.erase(c);
	}
private:
	virtual void Update(float dt);
	float prev_x, prev_y;
	WantMouseInput *over_client, *drag_client;
private:
	static MouseInput theInput;
	MouseInput();
	~MouseInput();

	std::set<WantMouseInput*> clients;
};
