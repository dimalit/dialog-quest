#include <cassert>

#include "MenuWidget.h"

MenuWidget::MenuWidget(float x, float y)
:font("font1.fnt")
{
	focused_item = -1;
	is_visible = 0;

	rect = hgeRect(x, y, 0, 0);

	color.SetHWColor(0xFFFFE060);
	light_color.SetHWColor(0xFFFFFFFF);
	shadow.SetHWColor(0x30000000);
	offset = 2;
	timer = 0;
	frame = 2;
	spf = 0.1f;

	recompute_dimensions();
}

MenuWidget::~MenuWidget()
{
}

void MenuWidget::insertItem(std::string text, int pos)
{
	int size = items_texts.size();
	assert(pos <= size);

	if(pos < 0)pos = size;
	
	items_texts.insert(items_texts.begin()+pos, text);
	items_checked.insert(items_checked.begin()+pos, false);
	items_rects.insert(items_rects.begin()+pos, hgeRect());
	

	recompute_dimensions();
}
void MenuWidget::eraseItem(int pos)
{
	assert(pos >= 0 && pos < items_texts.size());

	items_texts.erase(items_texts.begin()+pos);
	items_checked.erase(items_checked.begin()+pos);
	items_rects.erase(items_rects.begin()+pos);

	if(focused_item >= items_texts.size())
		focused_item = items_texts.size() - 1;

	recompute_dimensions();
}
void MenuWidget::setItemText(std::string text, int pos)
{
}
void MenuWidget::setItemChecked(bool ch, int pos)
{
	if(pos < 0)
		return;
	items_checked.at(pos) = ch;
}
void MenuWidget::uncheckAll()
{
	for(unsigned i=0; i<items_checked.size(); i++)
		items_checked.at(i) = false;
}

void MenuWidget::setFocusedItem(int pos)
{
	int size = items_texts.size();
	assert(pos < size);
	if(focused_item != pos){
		// begin animation
		frame = 0; timer = 0;
	}
	focused_item = pos;
}

void MenuWidget::show()
{
	if(is_visible)return;
	is_visible = true;
}

void MenuWidget::hide()
{
	if(!is_visible)return;
	is_visible = false;
}

void MenuWidget::Update(float dt){
	if(frame < offset){
		timer += dt;
		frame = timer / spf;
	}
}

void MenuWidget::Render()
{
	if(!is_visible)return;
	for(int i=0; i<items_texts.size(); i++){
		float offset = 0;
		hgeColor color = this->color;
		if(items_checked[i]){
			offset = -1;
			color = this->light_color;
		}
		else if(focused_item == i){
			offset = frame;
			color = this->light_color;
		}

		font.SetColor(shadow.GetHWColor());
		font.Render(items_rects[i].x1 + 3 + offset, items_rects[i].y1 + 3 + offset, HGETEXT_LEFT, items_texts[i].c_str());
		font.SetColor(color.GetHWColor());
		font.Render(items_rects[i].x1 - offset, items_rects[i].y1 - offset, HGETEXT_LEFT, items_texts[i].c_str());
	}
}

bool MenuWidget::isPointIn(float x, float y){
	return rect.TestPoint(x, y);
}

int  MenuWidget::getItemInPoint(float x, float y){
	// loop throught items
	for(int i=0; i<items_rects.size(); i++){
		if(items_rects[i].TestPoint(x, y))
			return i;
	}
	// return -1 if no such item
	return -1;
}

///////////////// PRIVATE /////////////////
void MenuWidget::recompute_dimensions(){
	const float x = rect.x1, y = rect.y1;

	// h = summ, w = max
	float dh = font.GetHeight();
	float h = dh * items_texts.size();

	float w = 0;
	// update max
	for(int i=0; i<items_texts.size(); i++){
		float cur_w = font.GetStringWidth(items_texts[i].c_str());
		if(cur_w > w)
			w = cur_w;
	}

	// update rects
	for(int i=0; i<items_rects.size(); i++){
		float cur_w = font.GetStringWidth(items_texts[i].c_str());
		items_rects[i] = hgeRect(x + (w - cur_w) / 2, y + dh * i, x + w - (w - cur_w) / 2, y + dh * (i+1));		// center
	}

	rect = hgeRect(x, y, x+w, y+h);
}