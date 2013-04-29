#pragma once

#include <iostream>
#include <boost/signal.hpp>
#include "WantFrameUpdate.h"

template<class Callee>
class Timer: public WantFrameUpdate
{
public:
	Timer(Callee c, float dt = - 1.0f);						// default = -1 - don't start
	virtual void Update(float);
	void start(){											// default - start immediately
		return restart(0.0f);
	}
	void start(float dt){
		return restart(dt);
	}
	void restart(float dt);									// default - restart previous interval
	void restart(){
		restart(-1.0);
	}
	void cancel(){
		running = false;
	}
	void setCallback(Callee arg){sig.disconntect_all_slots(); sig.connect(arg);}

protected:
	float elapsed;
	float interval;			// if <0 - not started
	bool running;
private:
	boost::signal<void ()> sig;
};

template<class Callee>
Timer<Callee>::Timer(Callee c, float dt)
{
	elapsed = 0.0f;
	interval = dt;
	running = false;

	sig.connect(c);

	if(interval >= 0.0f)
		start(interval);
}

template<class Callee>
void Timer<Callee>::Update(float dt){
	elapsed += dt;
	if(running && elapsed >= interval){
		running = false;
		sig();
	}
}

template<class Callee>
void Timer<Callee>::restart(float dt){
	if(dt >= 0) interval = dt;		// else use old interval
	assert(interval >= 0.0f);

	elapsed = 0.0f;
	running = true;
}