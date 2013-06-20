#pragma once

#include "Entity/Component.h"
#include <iostream>
#include <boost/signal.hpp>

template<class Callee>
class Timer: public EntityComponent
{
public:
	Timer(Callee c, float dt = - 1.0f);						// default = -1 - don't start

	virtual void OnUpdate(VariantList *params);

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
	float start_time;
};

template<class Callee>
Timer<Callee>::Timer(Callee c, float dt)
{
	elapsed = 0.0f;
	interval = dt;
	start_time = GetBaseApp()->GetTickTimingSystem(TIMER_GAME)/1000.0f;
	running = false;

	sig.connect(c);

	if(interval >= 0.0f)
		start(interval);

	// TODO Maybe attach ourselves to some Entity rather than to root? This can disable timer appropriately...
	GetBaseApp()->m_sig_update.connect(1, boost::bind(&Timer<Callee>::OnUpdate, this, _1));
}

template<class Callee>
void Timer<Callee>::OnUpdate(VariantList *params){
	// TODO if not running?!
	elapsed = GetBaseApp()->GetTickTimingSystem(TIMER_GAME)/1000.0f - start_time;
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
	start_time = GetBaseApp()->GetTickTimingSystem(TIMER_GAME)/1000.0f;
	running = true;
}