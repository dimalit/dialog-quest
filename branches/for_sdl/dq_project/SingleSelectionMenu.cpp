#include <cassert>

#include "main.h"
#include "SingleSelectionMenu.h"

SingleSelectionMenu::SingleSelectionMenu()
{
	widget = 0;
	checked_item = -1;
	UserInputDispatcher::getInstance()->addClient(this);
}

SingleSelectionMenu::~SingleSelectionMenu()
{
	UserInputDispatcher::getInstance()->removeClient(this);
}

void SingleSelectionMenu::onMouseMove(float mx, float my, input_state state)
{
	int prev_item = widget->getFocusedItem();

	// handle mouse movement
	int over_item = widget->getItemInPoint(mx, my);
	if(prev_item != over_item)
		widget->setFocusedItem(over_item);
}

void SingleSelectionMenu::onMouseDown(int btn, input_state state){
	if(btn != 0)return;

	// handle mouse buttons
	int over_item = widget->getItemInPoint(state.mx, state.my);
	widget->uncheckAll();
	widget->setItemChecked(true, over_item);
//	bool change = (checked_item != over_item);
	checked_item = over_item;
//	if(change)
	onChangeSelection();
}

void SingleSelectionMenu::onMouseUp(int btn, input_state state){
	if(btn==0)
		widget->uncheckAll();
}