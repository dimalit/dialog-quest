#pragma once
#include <hgeFont.h>
#include "widget.h"

class TextWidget: public virtual Widget
{
public:
	TextWidget(std::string text, float x = 0, float y = 0);
	~TextWidget(void);
	std::string getText(){return text;}
	void setText(std::string t){
		text = t;
		float w = fnt_normal->GetStringWidth(text.c_str());
		float h = fnt_normal->GetHeight()*count_lines(text);
		x1 = x - w/2; y1 = y - h/2; x2 = x + w/2; y2 = y + h/2;
	}
	virtual void Update(float dt);
	virtual void Render();
protected:
	void setFont(view_type type, std::string file);
	virtual void update_view();
private:
	hgeFont *fnt;
	hgeFont *fnt_normal, *fnt_over, *fnt_disabled,
		 *fnt_checked, *fnt_checked_over;
	std::string text;
	// utilitary
	int count_lines(const std::string& s);
};
