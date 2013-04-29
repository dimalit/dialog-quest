#pragma once

#include <hgeAnim.h>
#include <hgerect.h>

#include "ScreenItem.h"
#include "WantFrameUpdate.h"
#include "UserInputDispatcher.h"

class Widget: public ScreenItem,
	WantFrameUpdate,
	public hgeRect		// !!! should be at least protected
{
public:
	Widget(float x, float y);
	~Widget(void);

	// screen representation
	virtual void Render();
	virtual void Update(float dt){};

	// mouse handling
	virtual bool isPointIn(float mx, float my);
	virtual void onDbClick();
	virtual void onDrag(float dx, float dy);
	virtual void onDragStart();
	virtual void onDragEnd();
	virtual void onMouseOver();
	virtual void onMouseOut();
	
	// keyboard
	virtual void onChar(int chr);
	virtual void onFocusLose();

	// position and dimensions
	virtual void move(float dx, float dy);
	virtual void setX(float x);
	virtual void setY(float y);
	virtual void rotate(float r);
	virtual void setRotation(float r);
	hgeRect getRect(){return hgeRect(*this);}
	float getX(){return x;}
	float getY(){return y;}
	float getRotation(){return rot;}

	// state
	bool getVisible();
	void setVisible(bool v);
	bool getEnabled();
	void setEnabled(bool e);
	bool getChecked();
	void setChecked(bool c);
	
	// behavior
	bool getOverResponse(){return over_response;}
	void setOverResponse(bool r){over_response = r;}
	bool getClickResponse(){return click_response;}
	void setClickResponse(bool r){click_response = r;}
	bool getDragResponse(){return drag_response;}
	void setDragResponse(bool r){drag_response = r;}

protected:
	// state
	bool enabled, visible, checked, over;
	// behavior
	bool over_response, click_response, drag_response;

	// utilitary
	virtual void update_view(){};
	void takeCharFocus();
	void giveCharFocus();

	enum view_type {VIEW_NORMAL, VIEW_OVER, VIEW_CHECKED, NORMAL2CHECKED};
private:
	// no value semantic!
	Widget& operator=(const Widget&){assert(false);}
	Widget(const Widget&){assert(false);}
};
