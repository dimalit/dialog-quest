#include "FrameTimer.h"
#include "WantFrameUpdate.h"

WantFrameUpdate::WantFrameUpdate()
{
	FrameTimer* timer = FrameTimer::getInstance();
	timer->addClient(this);
}

WantFrameUpdate::~WantFrameUpdate()
{
	FrameTimer* timer = FrameTimer::getInstance();
	timer->removeClient(this);
}
