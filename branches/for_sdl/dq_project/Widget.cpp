#include "Widget.h"

#include <iostream>

Widget::Widget(float x, float y)
{
	this->setParent(parent_visual);
	this->x = x; this->y = y;
	this->rot = 0.0f;

	enabled = true;
	visible = true;
	checked = false;
	over = false;

	over_response = false;
	click_response = false;
	drag_response = false;
}

Widget::~Widget(void)
{
}

// mouse handling
bool Widget::isPointIn(float mx, float my){
	return TestPoint(mx, my);
}
void Widget::onDbClick(){
	MouseInputObject::onDbClick();
}
void Widget::onDrag(float dx, float dy){
	MouseInputObject::onDrag(dx, dy);

	if(drag_response){
		move(dx, dy);
	}
}
void Widget::onDragStart(){
	MouseInputObject::onDragStart();
}
void Widget::onDragEnd(){
	MouseInputObject::onDragEnd();

	if(click_response){
		checked = !checked;
		update_view();
	}
}

void Widget::onMouseOver(){
	MouseInputObject::onMouseOver();
	
	if(over_response){
		over = true;
		update_view();
	}
}
void Widget::onMouseOut(){
	MouseInputObject::onMouseOut();
	
	if(over_response){
		over = false;
		update_view();
	}
}

// keyboard
void Widget::onChar(int chr){
	CharInputObject::onChar(chr);

}
void Widget::onFocusLose(){
	CharInputObject::onFocusLose();
}

// for internal use only
void Widget::takeCharFocus(){
	UserInputDispatcher::getInstance()->setCharFocus(this);
}
void Widget::giveCharFocus(){
	UserInputDispatcher::getInstance()->setCharFocus(0);
}

// position and dimensions
void Widget::move(float dx, float dy){
	x+=dx; y+=dy;
	x1 += dx; y1 += dy;
	x2 += dx; y2 += dy;
}
void Widget::setX(float tx){
	float dx = tx - x;
	move(dx, 0);
}
void Widget::setY(float ty){
	float dy = ty - y;
	move(0, dy);
}

void Widget::rotate(float r){
	rot += r;
}

void Widget::setRotation(float r){
	rot = r;
}

bool Widget::getVisible(){
	return visible;
}
void Widget::setVisible(bool v){
	visible = v;
}

bool Widget::getEnabled(){
	return enabled;
}
void Widget::setEnabled(bool e){
	enabled = e;
	update_view();
}
bool Widget::getChecked(){
	return checked;
}
void Widget::setChecked(bool c){
	checked = c;
	update_view();
}

void Widget::Render(){

	float d = 0;
/*
	hge->Gfx_RenderLine(x1 - d, y1 - d, x2 + d, y1 - d);
	hge->Gfx_RenderLine(x2 + d, y1 - d, x2 + d, y2 + d);
	hge->Gfx_RenderLine(x2 + d, y2 + d, x1 - d, y2 + d);
	hge->Gfx_RenderLine(x1 - d, y2 + d, x1 - d, y1 - d);
*/

}