#pragma once

#include <hgerect.h>
#include "MouseInput.h"

class WantMouseInput
{
private:
	friend class MouseInput;
	virtual void onMouseOver(float x, float y){}
	virtual void onMouseOut(){}
	virtual void onMouseMove(float x, float y){}
	virtual void onDrag(float dx, float dy){}
	virtual void onDragStart(float x, float y){}
	virtual void onDragEnd(float x, float y){}

	virtual bool isPointIn(float x,  float y) = 0;
//	bool mouse_over;
protected:
	// class only for derivation
//	bool isMouseOver(){return mouse_over;}
	WantMouseInput();
	~WantMouseInput();
};
