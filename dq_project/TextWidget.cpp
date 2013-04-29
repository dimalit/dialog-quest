#include <cassert>
#include "TextWidget.h"

int TextWidget::count_lines(const std::string& s){
	int cnt = 1;
	for(unsigned int i=0; i<s.size(); i++){
		if(s[i]=='\n')cnt++;
	}
	// discard the last one
	if(s.size()>0 && s[s.size()-1]=='\n')
		cnt--;
	return cnt;
}

void TextWidget::setFont(view_type type, std::string file){
	switch(type){
	case VIEW_NORMAL:
		fnt_normal = new hgeFont(file.c_str());
		if(fnt_normal->GetHeight()==0){delete fnt_normal; fnt_normal = 0;}
		break;
	case VIEW_OVER:
		fnt_over = new hgeFont(file.c_str());
		if(fnt_over->GetHeight()==0){delete fnt_over; fnt_over = 0;}
		break;
	case VIEW_CHECKED:
		fnt_checked = new hgeFont(file.c_str());
		if(fnt_checked->GetHeight()==0){delete fnt_checked; fnt_checked = 0;}
		break;
	}// sw
}

TextWidget::TextWidget(std::string text, float x, float y)
:Widget(x, y)
{
	this->text = text;

	fnt_normal		= 0;
	fnt_over		= 0;
	fnt_disabled	= 0;
	fnt_checked		= 0;
	fnt_checked_over= 0;
}

TextWidget::~TextWidget(void)
{
	if(fnt_normal)
		delete fnt_normal;
	if(fnt_over)
		delete fnt_over;
	if(fnt_disabled)
		delete fnt_disabled;
	if(fnt_checked)
		delete fnt_checked;
	if(fnt_checked_over)
		delete fnt_checked_over;
}

void TextWidget::Update(float dt){
}

void TextWidget::Render(){
	if(visible){
		fnt->SetRotation(rot);
		fnt->Render(x, y1, HGETEXT_CENTER, text.c_str());
		Widget::Render();
	}
}

void TextWidget::update_view(){
	fnt = fnt_normal;
	if(enabled && !over && checked && fnt_checked)
		fnt = fnt_checked;
	else if(enabled && over && !checked && fnt_over)
		fnt = fnt_over;
	else if(enabled && over && checked)
		fnt = fnt_checked_over;

	float w = fnt->GetStringWidth(text.c_str());
	float h = fnt->GetHeight() * count_lines(text);
	x1 = x - w/2; y1 = y - h/2; x2 = x + w/2; y2 = y + h/2;
}