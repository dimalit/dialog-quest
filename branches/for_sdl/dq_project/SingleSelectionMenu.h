#pragma once

#include "UserInputDispatcher.h"
#include "MenuWidget.h"

class SingleSelectionMenu: RawInputObject
{
public:
	SingleSelectionMenu();
	~SingleSelectionMenu();
	void setWidget(MenuWidget* w){widget = w;}
	// update input
	virtual void onMouseMove(float x, float y, input_state state);
	virtual void onMouseDown(int btn, input_state state);
	virtual void onMouseUp(int btn, input_state state);
	int getSelection(){return checked_item;}
protected:
	virtual void onChangeSelection(){}
private:
	MenuWidget* widget;
	int checked_item;
};
