#pragma once

#include <cassert>
#include <iostream>
#include <set>

#include "WantFrameUpdate.h"

// singleton that manages frame updates for all objects
class FrameTimer
{
public:
	static FrameTimer* getInstance(){return &theTimer;}
	
	void Update(float dt);

	void addClient(WantFrameUpdate* c){
		assert(c && clients.count(c)==0);
		clients.insert(c);
//		std::cout<< "Add: " << c << std::endl;
	}
	void removeClient(WantFrameUpdate* c){
		assert(c && clients.count(c)!=0);
		clients.erase(c);
//		std::cout<< "Remove: " << c << std::endl;
	}
private:
	static FrameTimer theTimer;
	FrameTimer();
	~FrameTimer();

	std::set<WantFrameUpdate*> clients;
};
