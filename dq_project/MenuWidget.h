#pragma once

#include <vector>
#include <string>

#include <hgefont.h>
#include <hgecolor.h>

#include "WantFrameUpdate.h"
#include "Visual.h"

class MenuWidget: WantFrameUpdate, public Visual
{
public:
	MenuWidget(float x = 0, float y = 0);
	~MenuWidget(void);
	int getNumItems(){return items_texts.size();}
	void insertItem(std::string text=std::string("MenuItem"), int pos = -1);		// -1 means end
	void eraseItem(int pos);
	void setItemText(std::string text, int pos);
	std::string getItemText(int pos);
	bool isItemChecked(int pos){return items_checked.at(pos);}
	void setItemChecked(bool ch, int pos);
	void uncheckAll();
	void setFocusedItem(int pos);
	int  getFocusedItem(){return focused_item;}
	void show();
	void hide();
	void move(float x, float y){rect.x1 = x; rect.y1 = y; recompute_dimensions();}
	hgeRect getRect(){return rect;}

	bool isPointIn(float x, float y);
	int  getItemInPoint(float x, float y);

	void Render();
	void Update(float dt);
private:
	void recompute_dimensions();
private:
	std::vector<std::string> items_texts;
	std::vector<bool>		 items_checked;
	std::vector<hgeRect>	 items_rects;
	int						 focused_item;		// -1 if no one

	bool is_visible;

	hgeRect rect;
	hgeFont font;
	hgeColor color, shadow;
	hgeColor light_color;
	float offset;
	float spf;
	float frame;
	float timer;
};
