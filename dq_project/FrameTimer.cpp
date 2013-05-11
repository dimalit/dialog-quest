#include "PlatformPrecomp.h"

#include "FrameTimer.h"

#include <algorithm>

FrameTimer FrameTimer::theTimer;

FrameTimer::FrameTimer(void)
{
}

FrameTimer::~FrameTimer(void)
{
}

void FrameTimer::Update(float dt){
	class CallUpdate{
		float dt;
	public:
		CallUpdate(float dt){this->dt = dt;}
		void operator()(WantFrameUpdate* c){
		c->Update(dt);
		}
	};
	std::for_each(clients.begin(), clients.end(), CallUpdate(dt));
}