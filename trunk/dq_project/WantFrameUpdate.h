#pragma once

class FrameTimer;

class WantFrameUpdate
{
private:
	friend class FrameTimer;
	virtual void Update(float dt) = 0;

protected:
	// class only for derivation
	WantFrameUpdate();
	~WantFrameUpdate();
};
